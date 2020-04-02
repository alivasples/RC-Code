/*
 * HierarchyParser.h
 *
 *  Created on: 1 de nov de 2019
 *      Author: alexis
 */

#ifndef SRC_HIERARCHYPARSER_H_
#define SRC_HIERARCHYPARSER_H_

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>
#include <vector>
#include <bitset>
#include <set>
using namespace std;

class Operation{
	private:
		string connector;
		bool singleOperand;
		bool firstPositive;
		bool secondPositive;
		int firstComp;
		int secondComp;
		bool result;
	public:
		// Constructors
		Operation();
		Operation(int firstComp, string connector, int secondComp);
		Operation(bool firstPositive, int firstComp, string connector, bool secondPositive, int secondComp, bool singleOperand);
		// Getters and setters
		void setConnector(string connector);
		string getConnector();
		void setFirstComp(int firstComp);
		int getFirstComp();
		void setSecondComp(int secondComp);
		int getSecondComp();
		void setResult(bool result);
		int getResult();
		// Is's getters
		bool isSingleOperand();
		bool isFirstPositive();
		bool isSecondPositive();
		// Other
		void printOperation();
};

class OperationList{
	private:
		bool isDebugMode;
		int nrBaseOperands;
		vector<Operation> operations;
	public:
		// Constructors
		OperationList(): isDebugMode(false) {};
		// Setter and getters
		void setDebugMode(bool isDebugMode);
		void setNrBaseOperands(int nrBaseOperands);
		int  getNrBaseOperands();
		int getNrOperations();
		Operation getOperandAt(int idx);
		int stringToHOperations(string sOperation);
		bool getOperationResult(bitset<50> baseOperands);
		// Other basic methods
		void printOperations();
		// Methods for arboretum structures
		// set<int> getOperationResult(VSlimFloatResults& floatResults, VSlimStringResults& stringResults, VbpFloatResults& bpFloatResults, VbpStringResults& bpStringResults);

};



#endif /* SRC_HIERARCHYPARSER_H_ */
