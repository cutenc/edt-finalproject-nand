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
	
	unsigned long analyzed_leaves;
	unsigned long purged_leaves;
	
	unsigned long pushed_leaves;
	
	unsigned long updated_data_leaves;
	
	boost::chrono::microseconds elapsedTime;
	
	IntersectionResult();
	virtual ~IntersectionResult();
	
	boost::chrono::nanoseconds meanTimePerLeaf() const;
	
	IntersectionResult & operator+=(const IntersectionResult &other);
	
	const IntersectionResult operator+(const IntersectionResult &other) const;
	
	static std::string getPrintHeader();
	
	friend std::ostream & operator<<(std::ostream &os, const IntersectionResult &res);
	
};


#endif /* INTERSECTIONRESULT_HPP_ */
