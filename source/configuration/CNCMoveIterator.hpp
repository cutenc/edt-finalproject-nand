/*
 * CNCMoveIterator.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef CNCMOVEITERATOR_HPP_
#define CNCMOVEITERATOR_HPP_

#include <istream>
#include <fstream>
#include <iterator>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "common/Rototraslation.hpp"
#include "common/Utilities.hpp"

class CNCMove {
	
public:
	CNCMove() { }
	CNCMove(const Rototraslation &stock, const Rototraslation &cutter) :
		STOCK(stock), CUTTER(cutter) { }
	virtual ~CNCMove() { }
	
	Rototraslation STOCK, CUTTER;
	
	friend std::ostream & operator<<(std::ostream &os, const CNCMove &move) {
		os << "STOCK(" << move.STOCK << "); CUTTER(" << move.CUTTER << ")";
		
		return os;
	}
	
private:
	static Point3D buildTraslation(std::vector< std::string >::iterator it) {
		
		double X = boost::lexical_cast<double>(*it++);
		double Y = boost::lexical_cast<double>(*it++);
		double Z = boost::lexical_cast<double>(*it++);
		
		return Point3D(X, Y, Z);
		
	}

	static EulerAngles buildRotation(std::vector< std::string >::iterator it) {
		
		double ALPHA = boost::lexical_cast<double>(*it++);
		double BETA = boost::lexical_cast<double>(*it++);
		double GAMMA = boost::lexical_cast<double>(*it++);
		
		return EulerAngles(ALPHA, BETA, GAMMA);
		
	}

	static const Rototraslation buildRototraslation(std::vector< std::string >::iterator it) {
		
		// then build rotation and traslation based on the split
		Point3D traslation = CNCMove::buildTraslation(it);
		EulerAngles rotation = CNCMove::buildRotation(it + 3);
		
		return Rototraslation(traslation, rotation);
	}

	friend std::istream &operator>>(std::istream &is, CNCMove &move) {
		FileUtils::ReadData data;
		try {
			data = FileUtils::readNextValidLine(is);
		} catch (const std::runtime_error &e) {
			/* we are not sure that there is another valid line before EOF
			 * so we have to catch any exception rised by readNextValidLine
			 * and simply return from the method (no need to set failbit
			 * because we simply reached eof)
			 */
			return is;
		}
		
		// now we have to decode given line into a couple of Rototraslations
		
		/* it is not possible to use regexp here (apart from checking
		 * string-correctness) because a valid regex would be "(?:\\s*([\\d\\.-]+);?){12}"
		 * but in boost repeated captures are all discarded except for the last
		 * one: this means no way to get all saved numbers.
		 */
		
		/* the fastest approach is to to tokenize the string in a whole and then
		 * go for conversion of every token (i.e. number)
		 */
		std::vector< std::string > tokens;
		boost::split(tokens,
				data.validLine,
				boost::is_any_of("; "),
				boost::token_compress_on
		);
		
		if(tokens.size() == 12) {
		
			// then build rototraslation
			try {
				Rototraslation rt1, rt2;
				rt1 = CNCMove::buildRototraslation(tokens.begin());
				rt2 = CNCMove::buildRototraslation(tokens.begin() + 6);
				move = CNCMove(rt1, rt2);
				
			} catch (const std::exception &e) {
				std::cerr << "bad rototraslation spec: " << e.what() << std::endl;
				is.seekg(data.lastReadPos, std::ios_base::beg);
				is.setstate(std::ios_base::failbit);
			}
			
		} else {
			// given line seems not to be a rototraslation -> states the error
			std::cerr << "not enough tokens for a rototraslation: " << tokens.size() << std::endl;
			is.seekg(data.lastReadPos, std::ios_base::beg);
			is.setstate(std::ios_base::failbit);
		}

		return is;
	}
	
};

class CNCMoveIterator : public std::istream_iterator<CNCMove> {
	
	/* this variable is needed because we have to keep the object pointed
	 * to by the smart_pointer alive until the iterator will be deleted
	 */
	boost::shared_ptr< std::ifstream > stream;
	
public:
	CNCMoveIterator(boost::shared_ptr<std::ifstream> stream) : 
		std::istream_iterator<CNCMove>(*stream), stream(stream) {
	}
	
	CNCMoveIterator() : std::istream_iterator<CNCMove>() {
	}
	
	virtual ~CNCMoveIterator() { }
	
};


#endif /* CNCMOVEITERATOR_HPP_ */
