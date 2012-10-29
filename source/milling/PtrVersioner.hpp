/*
 * PtrVersioner.hpp
 *
 *  Created on: 07/ott/2012
 *      Author: socket
 */

#ifndef PTRVERSIONER_HPP_
#define PTRVERSIONER_HPP_

#include <cassert>
#include <stdexcept>
#include <utility>

#include <boost/shared_ptr.hpp>

template < typename DataT >
class PtrVersioner {

public:
	
	typedef unsigned long version_type;
	
	struct VersionInfo {
		static const version_type NULL_VERSION = 0;
		
		VersionInfo(version_type reader, version_type writer) :
			readerVersion(reader), writerVersion(writer)
		{ }
		
		virtual ~VersionInfo() { }
		
		const version_type readerVersion;
		const version_type writerVersion;
		
		bool isReaderActive() const {
			return readerVersion == NULL_VERSION;
		}
	};
	
private:
	static const unsigned char ARRAY_SIZE = 3;
	
	typedef std::pair< version_type, boost::shared_ptr< DataT > > VersionedPtr;
	
	enum State {
		INITIALIZED,
		ONE_VERSION,
		TWO_VERSIONS,
		THREE_VERSIONS
	};
	
private:
	VersionedPtr versionArray[ARRAY_SIZE];
	char readerV, writerV, writerPrevV, free;
	State state;
	
public:
	PtrVersioner() {
		readerV = writerV = writerPrevV = free = -1;
		state = INITIALIZED;
	}
	
	virtual ~PtrVersioner() { }
	
	void setData(DataT * data, const VersionInfo &v) {
		
		// double modification check (not allowed)
		assert((state == INITIALIZED) || (v.writerVersion > versionArray[writerV].first));
		
		/* if reader is active, asked version should be greater
		 * than the oldest stored
		 */
		assert((!v.isReaderActive()) || (v.readerVersion >= versionArray[readerV].first));
		
		switch (state) {
		case INITIALIZED:
			
			/* this is the first time that data is set so:
			 */
			readerV = writerV = writerPrevV = 0;
			free = 1;
			
			state = ONE_VERSION;
			
			break;
			
		case ONE_VERSION:
			/* there is only one value inserted -> insert another one
			 * (we have always to keep at least two versions: the
			 * 'already committed' one and the current which is being
			 * built)
			 */
			char nextFree = (writerV + 1) % ARRAY_SIZE;
			writerV = free;
			free = nextFree;
			
			state = TWO_VERSIONS;
			break;
			
		case TWO_VERSIONS:
			assert(readerV == writerPrevV);
			assert((free >= 0) && (free < ARRAY_SIZE));
			
			/* the outcome here depends on wether reader is running or
			 * not and at what version
			 */
			if (v.isReaderActive() && (v.readerVersion < versionArray[writerV].first)) {
				
				/* in order to satisfy reader I have to keep both
				 * readerV and writerV => THREE_VERSIONS
				 */ 
				writerPrevV = writerV;
				writerV = free;
				free = -1;
				// readerV keep pointing to the same location
				
				state = THREE_VERSIONS;
					
			} else {
				/* writerV version is enough to satisfy reader so
				 * i can forget the older one (readerV)
				 * - OR -
				 * reader is not running at all so we will remain in
				 * current state making readerV & writerPrevV pointing
				 * to current writerV
				 */
				arrayDeleter(readerV);
				
				/* using writerPrevV as tmp variable (see assertion at the
				 * beginning of the case)
				 */ 
				readerV = writerV;
				writerV = free;
				free = writerPrevV;
				writerPrevV = readerV;
				
				state = TWO_VERSIONS;
			}
			
			break;
			
		case THREE_VERSIONS:
			assert(free == -1);
			assert((readerV != writerV) && (readerV != writerPrevV) && (writerPrevV != writerV));
			
			/* the outcome depends wether reader is active or not and
			 * what version is requesting
			 */
			if ((v.isReaderActive()) && (v.readerVersion < versionArray[writerV].first)) {
				
				if (v.readerVersion < versionArray[writerPrevV].first) {
					/* this means that reader is asking for a version
					 * between readerV (incl) and writerPrevV (escl);
					 * i have to keep readerV and put new data on
					 * writePrevV (forgetting it)
					 */
					arrayDeleter(writerPrevV);
					std::swap(writerV, writerPrevV);
					
					state = THREE_VERSIONS;
					
				} else {
					/* reader is asking for a version between writerPrevV
					 * (incl) and writerV (escl) so I can forget about
					 * readerV
					 */
					arrayDeleter(readerV);
					
					char tmp = readerV;
					readerV = writerPrevV;
					writerPrevV = writerV;
					writerV = tmp;
					
					state = THREE_VERSIONS;
					
				}
				
			} else {
				/* reader is not active
				 * - OR -
				 * reader is asking for a version higher or equal
				 * than writerV so I can forget about readerV and
				 * writerPrevV keeping just writerV and current one
				 */
				
				arrayDeleter(readerV);
				arrayDeleter(writerPrevV);
				
				free = writerPrevV;
				writerPrevV = writerV;
				writerV = readerV;
				readerV = writerPrevV;
				
				state = TWO_VERSIONS;
			}
			
			break;
			
		default:
			throw std::runtime_error("state not registered");
			break;
		}
		
		arrayInserter(writerV, v.writerVersion, data);
		
	}
	
	void deleteData(const VersionInfo &v) {
		setData(NULL, v);
	}
	
	boost::shared_ptr< DataT > getReaderData(const VersionInfo &v) {
		assert(readerV >= 0);
		assert(v.isReaderActive());
		assert(v.readerVersion >= getVersion(readerV));
		
		boost::shared_ptr< DataT > ptr = getReaderCompliantPtr(v.readerVersion);
		
		// then perform any deletion on saved data
		switch (state) {
		case INITIALIZED:
			throw std::runtime_error("There's no values yet");
			break;
			
		case ONE_VERSION:
			// nothing to do here
			assert(readerV == writerV);
			assert(writerV == writerPrevV);
			break;
			
		case TWO_VERSIONS:
			assert(readerV == writerPrevV);
			assert((free >= 0) && (free < ARRAY_SIZE));
			
			if (v.writerVersion > getVersion(writerV)) {
				/* our writerV is already committed so we can forget about
				 * readerV/writerPrevV falling to a ONE_VERSION state
				 */
				
				arrayDeleter(readerV);
				readerV = writerPrevV = writerV;
				// free is already pointing to a valid position
				
				state = ONE_VERSION;
				
			}
			break;
			
		case THREE_VERSIONS:
			assert(free == -1);
			assert((readerV != writerV) && (readerV != writerPrevV) && (writerPrevV != writerV));
			
			// reader version can be deleted by default
			arrayDeleter(readerV);
			free = readerV;
			readerV = writerPrevV;
			
			state = TWO_VERSIONS;
			
			if (v.writerVersion > getVersion(writerV)) {
				// even writerPrevV is useless so delete it
				arrayDeleter(writerPrevV);
				readerV = writerPrevV = writerV;
				// free is already ok
				
				state = ONE_VERSION;
			}
			
			break;
				
		default:
			throw std::runtime_error("state not registered");
			break;
		}
		
		return ptr;
	}
	
	
	
	boost::shared_ptr< DataT > getWriterData(const VersionInfo &v) {
		assert(writerV >= 0);
		assert(getVersion(writerV) != VersionInfo::NULL_VERSION);
		
		switch (state) {
		case INITIALIZED:
			throw std::runtime_error("There's no values yet");
			break;
			
		case ONE_VERSION:
			// nothing to do here
			assert(readerV == writerV);
			assert(writerV == writerPrevV);
			break;
			
		case TWO_VERSIONS:
			assert(readerV == writerPrevV);
			assert((free >= 0) && (free < ARRAY_SIZE));
			
			if (isWriterPrevVUseless(v))
			{
				/* keeping writerPrevV is useless so we can delete it
				 */
				arrayDeleter(readerV);
				readerV = writerPrevV = writerV;
				// free is already pointing to something valid;
				
				state = ONE_VERSION;
			}
			break;
		
		case THREE_VERSIONS:
			assert(free == -1);
			assert((readerV != writerV) && (readerV != writerPrevV) && (writerPrevV != writerV));
			
			if (isReaderVUseless(v)) {
				if (isWriterPrevVUseless(v)) {
					/* both readerV and writerPrevV are useless so we can
					 * remove both of them falling to the ONE_VERSION state
					 */
					arrayDeleter(readerV);
					arrayDeleter(writerPrevV);
					
					free = readerV;
					readerV = writerPrevV = writerV;
					
					state = ONE_VERSION;
				} else {
					/* I can remove only readerV
					 */
					arrayDeleter(readerV);
					
					free = readerV;
					readerV = writerPrevV;
					
					state = TWO_VERSIONS;
				}
			} /* else {
			 * all versions are useful we can do nothing
			} */
			
			break;
			
		default:
			throw std::runtime_error("state not registered");
			break;
		}
		
		return this->versionArray[writerV].second;
	}
	
private:
	
	bool isReaderVUseless(const VersionInfo &v) {
		return
		((!v.isReaderActive()) && (v.writerVersion > getVersion(writerPrevV)))
		||
		(v.isReaderActive() && (v.readerVersion >= getVersion(writerPrevV)));
	}
	
	bool isWriterPrevVUseless(const VersionInfo &v) {
		return
		/* reader is not running and current writing version is higher
		 * than our stored one (this means that when reader will start
		 * it will ask for a version between [writerV, +inf[
		 */
		((!v.isReaderActive()) && (v.writerVersion > getVersion(writerV)))
		||
		/* reader is running but asking for a version higher than or equal to 
		 * our writerV
		 */
		(v.isReaderActive() && (v.readerVersion >= getVersion(writerV)));
	}
	
	void arrayInserter(int index, version_type version, DataT *data) {
		assert(index >= 0 && index < ARRAY_SIZE);
		
		this->versionArray[index].first = version;
		this->versionArray[index].second.reset(data);
	}
	
	void arrayDeleter(int index) {
		arrayInserter(index, VersionInfo::NULL_VERSION, NULL);
	}
	
	version_type getVersion(int index) {
		assert(index >= 0 && index < ARRAY_SIZE);
		
		return this->versionArray[index].first;
	}
	
	boost::shared_ptr< DataT > getPointer(int index) {
		assert(index >= 0 && index < ARRAY_SIZE);
		
		return this->versionArray[index].second;
	}
	
	boost::shared_ptr< DataT > getReaderCompliantPtr(version_type readerVersion) {
		assert(readerV >= 0);
		assert(readerVersion >= getVersion(readerV));
		
		switch (state) {
			case INITIALIZED:
				throw std::runtime_error("no values at all");
				break;
				
			case ONE_VERSION:
				assert(readerV == writerV);
				assert(writerV == writerPrevV);
				
				return getPointer(readerV);
				break;
				
			case TWO_VERSIONS:
				assert(readerV == writerPrevV);
				assert(writerPrevV != writerV);
				
				if (readerVersion < getVersion(writerV)) {
					return getPointer(readerV);
				} else {
					return getPointer(writerV);
				}
				
				break;
				
			case THREE_VERSIONS:
				assert(readerV != writerPrevV);
				assert(writerPrevV != writerV);
				assert(writerV != readerV);
				
				if (readerVersion < getVersion(writerPrevV)) {
					return getPointer(readerV);
				} else if (readerVersion < getVersion(writerV)) {
					return getPointer(writerPrevV);
				} else {
					return getPointer(writerV);
				}
				
				break;
				
			default:
				throw std::runtime_error("state not registered");
				break;
		}
		
		throw std::runtime_error("this point should not be reached");
	}
};

#endif /* PTRVERSIONER_HPP_ */
