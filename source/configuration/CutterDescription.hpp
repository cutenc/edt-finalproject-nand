/**
 * @file CutterDescription.hpp
 *
 *  Created on: 13/ago/2012
 *      Author: socket
 */

#ifndef CUTTERDESCRIPTION_HPP_
#define CUTTERDESCRIPTION_HPP_

#include <boost/shared_ptr.hpp>

#include "Geometry.hpp"
#include "common/Color.hpp"

/**
 * @class CutterDescription
 *
 * geometry and color of the cutter
 */
class CutterDescription {
	
public:
	typedef boost::shared_ptr<CutterDescription> Ptr;
	typedef boost::shared_ptr<const CutterDescription> ConstPtr;
	
public:
	/**
	 * constructor
	 * @param desc size and shape of the cutter
	 * @param color the color of the cutter
	 */
	CutterDescription(const GeometryPtr &desc, const Color &color);

	/**
	 * destructor
	 */
	virtual ~CutterDescription();
	
	/**
	 *
	 * @return the geometry of the cutter
	 */
	const GeometryPtr getGeometry() const;

	/**
	 *
	 * @return the color of the cutter
	 */
	Color getColor() const;
	
private:
	const GeometryPtr DESCRIPTION;
	const Color COLOR;
};

#endif /* CUTTERDESCRIPTION_HPP_ */
