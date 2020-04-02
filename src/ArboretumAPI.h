//
// Created by gonzaga on 1/20/16.
//

#ifndef CREATEINDEX_DIVISIONTREE_H
#define CREATEINDEX_DIVISIONTREE_H

#include <string>
#include <cstring>
#include <array>

#include <arboretum/stMetricTree.h>
#include <arboretum/stPlainDiskPageManager.h>
#include <arboretum/stDiskPageManager.h>
#include <arboretum/stMemoryPageManager.h>
#include <arboretum/stSlimTree.h>
#include <arboretum/stMetricTree.h>
// #include <arboretum/stBasicObjects.h>
// #include <arboretum/stBasicMetricEvaluators.h>
// #include <arboretum/stMultiObject.h>
#include <util/BasicArrayObject.h>
#include <hermes/EuclideanDistance.h>
#include <hermes/DTWDistance.h>
#include <arboretum/stBTree.h>

#define MAXCHARARRAY 500

using namespace std;
// =====================================================================================================================
/*
typedef struct {
	bool operator()(array<char, MAXCHARARRAY> string1, array<char, MAXCHARARRAY> string2) const{
		int i = 0;
		while(i < MAXCHARARRAY && string1.at(i) == string2.at(i) && string1.at(i) != '\0' && string2.at(i) != '\0') i++;
		if (string1.at(i) < string2.at(i)) return true;
		return false;
	}
} BPlusManagerComparator;
*/
struct MyString{
	int rowId;
	char key[MAXCHARARRAY];
	MyString(){}
	MyString(int rowId, string str){
		this->rowId = rowId;
		strcpy(key, str.c_str());
	}
	bool operator==(MyString string2) const{
		int i=0;
		while(i < MAXCHARARRAY && key[i] == string2.key[i] && key[i] != '\0' && string2.key[i] != '\0') i++;
		if (key[i] == string2.key[i]) return true;
		return false;
	}
	bool operator<(MyString string2) const{
		int i=0;
		while(i < MAXCHARARRAY && key[i] == string2.key[i] && key[i] != '\0' && string2.key[i] != '\0') i++;
		if (key[i] < string2.key[i]) return true;
		return false;
	}
	bool operator<=(MyString string2) const{
		int i=0;
		while(i < MAXCHARARRAY && key[i] == string2.key[i] && key[i] != '\0' && string2.key[i] != '\0') i++;
		if (key[i] <= string2.key[i]) return true;
		return false;
	}
	bool operator>(MyString string2) const{
		int i=0;
		while(i < MAXCHARARRAY && key[i] == string2.key[i] && key[i] != '\0' && string2.key[i] != '\0') i++;
		if (key[i] > string2.key[i]) return true;
		return false;
	}
	bool operator>=(MyString string2) const{
		int i=0;
		while(i < MAXCHARARRAY && key[i] == string2.key[i] && key[i] != '\0' && string2.key[i] != '\0') i++;
		if (key[i] >= string2.key[i]) return true;
		return false;
	}
};

typedef struct {
	bool operator()(MyString string1, MyString string2) const{
		int i=0;
		while(i < MAXCHARARRAY && string1.key[i] == string2.key[i] && string1.key[i] != '\0' && string2.key[i] != '\0') i++;
		if (string1.key[i] < string2.key[i]) return true;
		return false;
	}
} BPlusManagerComparator;

////MULTIDIMENSIONAL INT -------------------------------------------------------------------
typedef BasicArrayObject<int> stIntArrayObject;
typedef EuclideanDistance<stIntArrayObject> stIntArrayEvaluator;
typedef stSlimTree < stIntArrayObject, stIntArrayEvaluator > stIntArrayTree;
typedef stResult < stIntArrayObject > myIntResult;

////MULTIDIMENSIONAL FLOAT -------------------------------------------------------------------
typedef BasicArrayObject<float> stFloatArrayObject;
typedef EuclideanDistance<stFloatArrayObject> stFloatArrayEvaluator;
typedef stSlimTree < stFloatArrayObject, stFloatArrayEvaluator > stFloatArrayTree;
typedef stResult < stFloatArrayObject > myFloatResult;

////MULTIDIMENSIONAL CHARS -------------------------------------------------------------------
typedef BasicArrayObject<char> stCharArrayObject;
typedef DTWDistance<stCharArrayObject> stCharArrayEvaluator;
typedef stSlimTree < stCharArrayObject, stCharArrayEvaluator > stCharArrayTree;
typedef stResult <stCharArrayObject > myCharArrayResult;

////MULTIDIMENSIONAL STRING -------------------------------------------------------------------
typedef BasicArrayObject<string> stStringArrayObject;
typedef EuclideanDistance<stStringArrayObject> stStringArrayEvaluator;
typedef stSlimTree < stStringArrayObject, stStringArrayEvaluator > stStringArrayTree;
typedef stResult <stStringArrayObject > myStringArrayResult;

////METRIC ------------------------------------------------------------------------------------
typedef BasicArrayObject<float> stGenericMetricObject;
typedef EuclideanDistance<stGenericMetricObject> stGenericMetricEvaluator;
typedef stSlimTree <stGenericMetricObject, stGenericMetricEvaluator> stGenericMetricTree;
typedef stResult <stGenericMetricObject> myGenericMetricResult;


// BPLUS Trees
typedef stBPlusTree<int,stIntArrayObject> bpIntTree;
typedef stTOResult<stIntArrayObject, int> bpIntResult;

typedef stBPlusTree<float,stIntArrayObject> bpFloatTree;
typedef stTOResult<stIntArrayObject, float> bpFloatResult;

//typedef stBPlusTree<array<char,MAXCHARARRAY>,stIntArrayObject,BPlusManagerComparator> bpStringTree;
//typedef stTOResult<stIntArrayObject, array<char,MAXCHARARRAY> > bpStringResult;
typedef stBPlusTree<MyString,stIntArrayObject,BPlusManagerComparator> bpStringTree;
typedef stTOResult<stIntArrayObject, MyString> bpStringResult;



#endif //CREATEINDEX_DIVISIONTREE_H
