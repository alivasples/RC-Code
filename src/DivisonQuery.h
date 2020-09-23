//
// Created by gonzaga on 1/23/16.
//

#ifndef SIMDIV_DIVISONQUERY_H
#define SIMDIV_DIVISONQUERY_H

#include <string>
#include "Attribute.h"

using namespace std;

class DivisonQuery
{
public:

    Attribute skill;
    Attribute requirement;
    string comparator;
    double threshold;
    string thresOperator;

    DivisonQuery(const Attribute &skill, const Attribute &requirement, string comparator, float threshold) :
		skill(skill),
		requirement(requirement),
		comparator(comparator),
		threshold(threshold),
		thresOperator("+")
    { }


    DivisonQuery()
    { }

    bool isTraditionalCompare();

    double getOperatedFactor(const double& value);
};


#endif //SIMDIV_DIVISONQUERY_H
