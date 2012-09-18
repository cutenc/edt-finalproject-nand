/*
 * CutterDescription.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef CUTTERDESCRIPTION_HPP_
#define CUTTERDESCRIPTION_HPP_

#include <boost/shared_ptr.hpp>

#include "Geometry.hpp"
#include "common/Color.hpp"

class CutterDescription {
	
public:
	typedef boost::shared_ptr<CutterDescription> Ptr;
	typedef boost::shared_ptr<const CutterDescription> ConstPtr;
	
public:
	CutterDescription(const GeometryPtr &desc, const Color &color);
	virtual ~CutterDescription();
	
	const GeometryPtr getGeometry() const;
	Color getColor() const;
	
private:
	const GeometryPtr DESCRIPTION;
	const Color COLOR;
};

#endif /* CUTTERDESCRIPTION_HPP_ */
