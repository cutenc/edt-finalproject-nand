/*
 * IntersectionResult.cpp
 *
 *  Created on: 11/ott/2012
 *      Author: socket
 */

#include "IntersectionResult.hpp"

#include <sstream>

IntersectionResult::IntersectionResult() :
	waste(0), analyzed_leaves(0), purged_leaves(0), lazy_purged_leaves(0), 
	pushed_leaves(0), updated_data_leaves(0), intersection_approx_errors(0), 
	intersection_approx_skips(0), elapsedTime(0)
{ }

IntersectionResult::~IntersectionResult() { }

boost::chrono::nanoseconds IntersectionResult::meanTimePerLeaf() const {
	if (this->analyzed_leaves == 0)
		return boost::chrono::nanoseconds(0);
	
	boost::chrono::nanoseconds us = elapsedTime;
	return boost::chrono::nanoseconds(us.count() / this->analyzed_leaves);
}

IntersectionResult & IntersectionResult::operator+=(const IntersectionResult &other) {
	waste += other.waste;
	analyzed_leaves += other.analyzed_leaves;
	purged_leaves += other.purged_leaves;
	lazy_purged_leaves += other.lazy_purged_leaves;
	pushed_leaves += other.pushed_leaves;
	updated_data_leaves += other.updated_data_leaves;
	intersection_approx_errors += other.intersection_approx_errors; 
	intersection_approx_skips += other.intersection_approx_skips;
	elapsedTime += other.elapsedTime;
	
	return *this;
}

const IntersectionResult IntersectionResult::operator+(const IntersectionResult &other) const {
	return IntersectionResult(*this) += other;
}

std::string IntersectionResult::getPrintHeader() {
	std::stringstream ss;
	ss << "waste"
			<< "\t#analyzed leaves"
			<< "\t#purged"
			<< "\t#lazy purged"
			<< "\t#updated_data_leaves"
			<< "\t#pushed"
			<< "\t#approx_error"
			<< "\t#approx_skips"
			<< "\t#elapsed time (ms)"
			<< "\t#mean time per leaf (us)";
	return ss.str();
}

std::ostream & operator<<(std::ostream &os, const IntersectionResult &res) {
	os << res.waste
			<< "\t" << res.analyzed_leaves
			<< "\t" << res.purged_leaves
			<< "\t" << res.lazy_purged_leaves
			<< "\t" << res.updated_data_leaves
			<< "\t" << res.pushed_leaves
			<< "\t" << res.intersection_approx_errors
			<< "\t" << res.intersection_approx_skips
			<< "\t" << res.elapsedTime.count() / 1000.0
			<< "\t" << res.meanTimePerLeaf().count() / 1000.0
	;
	
	return os;
}

