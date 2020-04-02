//
// Created by gonzaga on 4/21/16.
//

#ifndef SIMDIV_ATTRIBUTE_H
#define SIMDIV_ATTRIBUTE_H

#include <string>

using namespace std;


class Attribute
{
public:
    string name;
    string type;
    int size;

    int headerIndex;


    Attribute(const string &name, const string &type, int size, int headerIndex) : name(name), type(type), size(size),
                                                                                   headerIndex(headerIndex)
    {}


    Attribute()
    { }
};


#endif //SIMDIV_ATTRIBUTE_H
