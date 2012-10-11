/*
 * IntersectionResult.hpp
 *
 *  Created on: 11/ott/2012
 *      Author: socket
 */

#ifndef INTERSECTIONRESULT_HPP_
#define INTERSECTIONRESULT_HPP_

#include <ostream>

#include <boost/chrono.hpp>

class IntersectionResult {
	
public:
	/**
	 * Gives an estimation of how much volume has been removed during the
	 * milling operation
	 */
	double waste;
	
	u_long analyzed_leaves;
	u_long purged_leaves;
	
	/**
	 * counts the number of times a leaf has been deleted after more than
	 * one partial erosions (that is it has been deleted before beeing found
	 * totally inside the cutter).
	 */
	u_long lazy_purged_leaves;
	u_long pushed_leaves;
	
	u_long updated_data_leaves;
	
	/**
	 * counts number of leaves that bounding-box intersection test states are 
	 * intersecting but, at the end, distance misuration
	 * finds no corner inside cutter blade. In other words it counts the
	 * number of leaves that has been analyzed but the corners of which were
	 * always outside cutter blade (it may happen even for a too shallow
	 * depth limit)  
	 */
	u_long intersection_approx_errors;
	
	u_long intersection_approx_skips;
	
	boost::chrono::microseconds elapsedTime;
	
	IntersectionResult();
	virtual ~IntersectionResult();
	
	boost::chrono::nanoseconds meanTimePerLeaf() const;
	
	IntersectionResult & operator+=(const IntersectionResult &other);
	
	const IntersectionResult operator+(const IntersectionResult &other) const;
	
	static std::string IntersectionResult::getPrintHeader();
	
	friend std::ostream & operator<<(std::ostream &os, const IntersectionResult &res);
	
};


#endif /* INTERSECTIONRESULT_HPP_ */
