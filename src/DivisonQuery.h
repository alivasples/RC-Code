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


    DivisonQuery(const Attribute &skill, const Attribute &requirement, string comparator, float threshold) :
		skill(skill),
		requirement(requirement),
		comparator(comparator),
		threshold(threshold)
    { }


    DivisonQuery()
    { }

    bool isTraditionalCompare();
};


#endif //SIMDIV_DIVISONQUERY_H
