/**
 * @file StockDescriptor.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef STOCKDESCRIPTOR_HPP_
#define STOCKDESCRIPTOR_HPP_

#include <boost/shared_ptr.hpp>

#include "Geometry.hpp"

/**
 * @class StockDescription
 *
 * geometry and color of a Stock
 */
class StockDescription {
	
public:
	
	typedef boost::shared_ptr<StockDescription> Ptr;
	typedef boost::shared_ptr<const StockDescription> ConstPtr;
	
public:
	/**
	 * costructor
	 * @param desc the description
	 */
	StockDescription(const RectCuboidPtr &desc);
	virtual ~StockDescription();
	
	/**
	 *
	 * @return the geometry (RectCuboid)
	 */
	const RectCuboidPtr getGeometry() const;
	
private:
	const RectCuboidPtr DESCRIPTION;
};

#endif /* STOCKDESCRIPTOR_HPP_ */
