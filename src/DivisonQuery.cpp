//
// Created by gonzaga on 1/23/16.
//

#include "DivisonQuery.h"

bool DivisonQuery::isTraditionalCompare(){
	if(comparator[0] == '<' or comparator[0] == '>' or comparator[0] == '=') return true;
	return false;
}

double DivisonQuery::getOperatedFactor(const double & value){
	if(thresOperator == "+") return value + threshold;
	if(thresOperator == "-") return value - threshold;
	if(thresOperator == "*") return value * threshold;
	if(threshold > 0 && thresOperator == "/") return value / threshold;
	return value;
}
