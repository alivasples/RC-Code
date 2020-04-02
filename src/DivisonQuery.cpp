//
// Created by gonzaga on 1/23/16.
//

#include "DivisonQuery.h"

bool DivisonQuery::isTraditionalCompare(){
	if(comparator[0] == '<' or comparator[0] == '>' or comparator[0] == '=') return true;
	return false;
}
