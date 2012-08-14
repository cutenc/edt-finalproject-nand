/*
 * CutterDescription.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef CUTTERDESCRIPTION_HPP_
#define CUTTERDESCRIPTION_HPP_

#include "Geometry.hpp"
#include "common/Color.hpp"

class CutterDescription {
public:
	CutterDescription(const GeometryPtr desc, const Color &color);
	virtual ~CutterDescription();
	
	const GeometryPtr getGeometry() const;
	Color getColor() const;
	
private:
	const GeometryPtr DESCRIPTION;
	const Color COLOR;
};

#endif /* CUTTERDESCRIPTION_HPP_ */
