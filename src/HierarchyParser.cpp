/*
 * HierarchyParser.cpp
 *
 *  Created on: 1 de nov de 2019
 *      Author: alexis
 */
#include "HierarchyParser.h"
#define DEBUG_MSG(msg) if(isDebugMode){cout<<msg;}

// ========================== CLASS Operation ==============================
// Constructors
Operation::Operation(){
	this->firstPositive = true;
	this->firstComp = -1;
	this->secondPositive = true;
	this->secondComp = -1;
}

Operation::Operation(int firstComp, string connector, int secondComp){
	this->firstPositive = true;
	this->firstComp = firstComp;
	this->connector = connector;
	this->secondPositive = true;
	this->secondComp = secondComp;
	this->singleOperand = false;
}

Operation::Operation(bool firstPositive, int firstComp, string connector, bool secondPositive, int secondComp, bool singleOperand){
	this->firstPositive = firstPositive;
	this->firstComp = firstComp;
	this->connector = connector;
	this->secondPositive = secondPositive;
	this->secondComp = secondComp;
	this->singleOperand = singleOperand;
}

// Getters and Setters
void Operation::setConnector(string connector){
	this->connector = connector;
}

string Operation::getConnector(){
	return connector;
}

void Operation::setFirstComp(int firstComp){
	this->firstComp = firstComp;
}

int Operation::getFirstComp(){
	return firstComp;
}

void Operation::setSecondComp(int secondComp){
	this->secondComp = secondComp;
}

int Operation::getSecondComp(){
	return secondComp;
}

void Operation::setResult(bool result){
	this->result = result;
}

int Operation::getResult(){
	return result;
}

// Is getters
bool Operation::isSingleOperand(){
	return singleOperand;
}

bool Operation::isFirstPositive(){
	return firstPositive;
}

bool Operation::isSecondPositive(){
	return secondPositive;
}

// Other methods

void Operation::printOperation(){
	if(!firstPositive) cout<<"-";
	cout<<firstComp<<" ";
	cout<<connector<<" ";
	if(!secondPositive) cout<<"-";
	cout<<secondComp<<endl;
}

// ========================= CLASS OperationList =================================

// Getters and Setters
void OperationList::setDebugMode(bool isDebugMode){
	this->isDebugMode = isDebugMode;
}

void OperationList::setNrBaseOperands(int nrBaseOperands){
	this->nrBaseOperands = nrBaseOperands;
}

int OperationList::getNrBaseOperands(){
	return nrBaseOperands;
}

int OperationList::getNrOperations(){
	return operations.size();
}

Operation OperationList::getOperandAt(int idx){
	return operations[idx];
}

// ============================= HP FUNCTIONS ===================================
// Function wich recieves a string like (((1,or,2),or,3),and,(4,or,5))
// And stores into a vector [(1 or 2),(6 or 3), (4 or 5), (7 and 8)]
// 6, 7 and 8 are the order of resolution of internal brackets
int OperationList::stringToHOperations(string input){
    string token1 = "";
    string token2 = "";
    string token3 = "";
    bool operationPositive = true;
    bool token1Positive = true;
    bool token3Positive = true;
    bool singleOperand = true;
    int orderToken1 = 0;
    int orderToken3 = 0;
    int nrOpenBrackets = 0;
    int nrCommas = 0;
    for(int i=0; i<input.length(); i++){
        if(nrOpenBrackets == 0 and input[i] == ','){
            nrCommas++;
        }
        else{
            if(input[i] == '('){
                nrOpenBrackets++;
            }
            else if(input[i] == ')'){
                nrOpenBrackets--;
            }

            if(nrCommas == 0) token1 += input[i];
            else if(nrCommas == 1) token2 += input[i];
            else if(nrCommas == 2) token3 += input[i];
        }
    }

    // If the first term is negative
    if(token1[0] == '-'){
    	// Set the token as negative
		token1Positive = false;
		// and update the token to its value
		token1 = token1.substr(1);
    }

    // The same for token 3 (second term)
    // If the first term is negative
      if(token3[0] == '-'){
      	// Set the token as negative
  		token3Positive = false;
  		// and update the token to its value
  		token3 = token3.substr(1);
      }

    if(token1[0] == '(') orderToken1 = stringToHOperations(token1.substr(1,token1.length()-2));
    else orderToken1 = atoi(token1.c_str());

    if(nrCommas > 0){
    	singleOperand = false;
		if(token3[0] == '(') orderToken3 = stringToHOperations(token3.substr(1,token3.length()-2));
		else orderToken3 = atoi(token3.c_str());
    }

    //cout<<"Tokens: "<<token1<<" "<<token2<<" "<<token3<<endl;
    //cout<<"Order: "<<orderToken1<<" "<<token2<<" "<<orderToken3<<endl;
    operations.push_back(Operation(token1Positive, orderToken1, token2, token3Positive, orderToken3, singleOperand));
    return operations.size() + nrBaseOperands;
}

void OperationList::printOperations(){
	cout<<"Printing Ordered Operations..."<<endl;
	for(int i=0; i<operations.size(); i++){
		operations[i].printOperation();
	}
	cout<<"Ordered Operations printed"<<endl;
}

bool OperationList::getOperationResult(bitset<50> baseOperands){
	bool firstComp;
	bool secondComp;
	bool result;
	int idxFirstComp;
	int idxSecondComp;
	DEBUG_MSG("Calculating the operation...\n");
	//DEBUG_MSG(nrBaseOperands<<": "<<baseOperands<<endl);
	// REMEMBER: Logically ~(p xor q) = (p xand q).
	// This returns the same bit when is positive or reversed when negative
	// Normal loop
	for(int i=0; i<operations.size(); i++){
		// Temp store of index components
		idxFirstComp = operations[i].getFirstComp()-1;
		idxSecondComp= operations[i].getSecondComp()-1;
		//cout<<idxFirstComp<<" "<<operations[i].getConnector()<<" "<<idxSecondComp<<endl;
		// Use the value of first component
		if(idxFirstComp < nrBaseOperands) firstComp = !(operations[i].isFirstPositive() ^ baseOperands[idxFirstComp]);
		else firstComp = operations[idxFirstComp-nrBaseOperands].getResult();
		// Use the value of second component
		if(idxSecondComp < nrBaseOperands) secondComp = !(operations[i].isSecondPositive() ^ baseOperands[idxSecondComp]);
		else secondComp = operations[idxSecondComp-nrBaseOperands].getResult();
		// Compute the result of current operation
		if(operations[i].isSingleOperand()){
			result = firstComp;
			operations[i].setResult(result);
		}
		else if(operations[i].getConnector() == "and") {
			result = firstComp & secondComp;
			operations[i].setResult(result);
		}
		else if(operations[i].getConnector() == "or"){
			result = firstComp | secondComp;
			operations[i].setResult(result);
		}
		else cout<<"ERROR AT HierarchyParser: Operation not supported!"<<endl;
		//DEBUG_MSG(firstComp<<" "<<operations[i].getConnector()<<" "<<secondComp<<" = "<<result<<endl);
	}
	DEBUG_MSG("Operation calculated with result: "<<result<<endl);
	return result;
}
