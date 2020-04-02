#ifndef SRC_ARBSETOPERATIONS_H_
#define SRC_ARBSETOPERATIONS_H_


#include <HierarchyParser.h>
#include <ArboretumAPI.h>
#include <set>
using namespace std;

#define DEBUG_MSG(msg) if(isDebugMode){cout<<msg;}
#define DEBUG_INSTR(instr) if(isDebugMode){instr;}

typedef vector<myFloatResult*> VSlimFloatResults;
typedef vector<myCharArrayResult*> VSlimStringResults;
typedef vector<bpFloatResult*> VbpFloatResults;
typedef vector<bpStringResult*> VbpStringResults;


// OPERATIONS FOR SETS (UNION, INTERSECTION)
class SetOperations{
public:
	// Global function for printing a set of integers
	static void printSet(set<int> idsSet){
		cout<<"Set with "<<idsSet.size()<<" elements: "<<endl;
		for (set<int>::iterator it=idsSet.begin(); it!=idsSet.end(); ++it){
			std::cout << ' ' << *it;
		}
		cout<<endl;
	}

	// Generate intersection of two simple sets
	static set<int> unionSets(set<int>& set1, set<int>& set2){
	    set<int>::iterator itSet2;

	    for(set<int>::iterator itSet2=set2.begin(); itSet2!=set2.end(); ++itSet2){
	    	set1.insert(*itSet2);
	    }
	    set2.clear();
	    return set1;
	}

	// Generate intersection of a set and (stResult or sTOResult)
	template <class DType>
	static set<int> unionSets(set<int>& set1, DType resultSet){
		set<int>::iterator itSet1;
		for(int i=0; i<resultSet->GetNumOfEntries(); i++){
			set1.insert(resultSet->GetPair(i)->GetObject()->GetOID());
		}
		return set1;
	}

	// Generate intersection of a (stResult or sTOResult) and a set
	template <class DType>
	static set<int> unionSets(DType resultSet, set<int>& set2){
		set<int>::iterator itSet1;

		for(int i=0; i<resultSet->GetNumOfEntries(); i++){
			set2.insert(resultSet->GetPair(i)->GetObject()->GetOID());
		}
		return set2;
	}

	// Generate intersection of a (stResult or sTOResult) and (stResult or sTOResult)
	template <class DType1, class DType2>
	static set<int> unionSets(DType1 resultSet1, DType2 resultSet2){
		set<int> set1;
		set<int>::iterator itSet1;

		// Parse first resultSet to set
		set1 = unionSets(set1, resultSet1);
		// Start processing
		return unionSets(set1, resultSet2);
	}

	// Generate intersection of two simple sets
	static set<int> intersectSets(set<int>& set1, set<int>& set2, bool isFirst){
	    set<int> completeSet;
	    set<int>::iterator itSet2;
	    int currentID;

	    for(set<int>::iterator itSet2=set2.begin(); itSet2!=set2.end(); ++itSet2){
	    	currentID = *itSet2;
	        if(isFirst) completeSet.insert(currentID);
	        else if(set1.count(currentID) > 0) completeSet.insert(currentID);
	    }
	    set1.clear(); set2.clear();
	    return completeSet;
	}

	// Generate intersection of a (stResult or sTOResult) and a set
	template <class DType>
	static set<int> intersectSets(DType resultSet, set<int>& set2, bool isFirst){
	    set<int> completeSet;
	    int currentID;

	    for(int i=0; i<resultSet->GetNumOfEntries(); i++){
	    	currentID = resultSet->GetPair(i)->GetObject()->GetOID();
	        if(isFirst) completeSet.insert(currentID);
	        else if (set2.count(currentID)>0){
	            completeSet.insert(currentID);
	        }
	    }
	    set2.clear();
	    return completeSet;
	}


	// Generate intersection of a set and (stResult or sTOResult)
	template <class DType>
	static set<int> intersectSets(set<int>& set1, DType resultSet, bool isFirst){
	    return intersectSets(resultSet, set1, isFirst);
	}

	// Generate intersection of a (stResult or sTOResult) and (stResult or sTOResult)
	template <class DType1, class DType2>
	static set<int> intersectSets(DType1 resultSet1, DType2 resultSet2, bool isFirst){
		set<int> completeSet;
		set<int> set1;
		int currentID;
		// Parse first resultSet to set
		set1 = unionSets(set1, resultSet1);// Start processing
		for(int i=0; i<resultSet2->GetNumOfEntries(); i++){
			currentID = resultSet2->GetPair(i)->GetObject()->GetOID();
			if(set1.count(currentID)>0) completeSet.insert(currentID);
		}
		set1.clear();
		return completeSet;
	}
};

class ArbHierParser{
private:
	bool isDebugMode;
	OperationList operationList;
	VSlimFloatResults vSlimFloatResults;
	VSlimStringResults vSlimStringResults;
	VbpFloatResults vbpFloatResults;
	VbpStringResults vbpStringResults;
public:
	// METHODS
	// Constructor
	ArbHierParser(bool isDebugMode, OperationList operationList, VSlimFloatResults& vSlimFloatResults, VSlimStringResults& vSlimStringResults, VbpFloatResults& vbpFloatResults, VbpStringResults& vbpStringResults){
		this->isDebugMode = isDebugMode;
		this->operationList = operationList;
		this->vSlimFloatResults = vSlimFloatResults;
		this->vSlimStringResults = vSlimStringResults;
		this->vbpFloatResults = vbpFloatResults;
		this->vbpStringResults = vbpStringResults;
	}
	// Other methods
	set<int> getOperationResult(){
		// Vector with set of results of n operation
		vector<set<int>> results;
		set<int> currentResult;
		// Index of first and second component
		int idxFirstComp;
		int idxSecondComp;
		int nrBaseOperands = operationList.getNrBaseOperands();

		for(int i=0; i<operationList.getNrOperations(); i++){
			DEBUG_MSG("Operation "<<i+1<<": ");
			// Temp store of index components
			Operation currentOperand = operationList.getOperandAt(i);
			idxFirstComp = currentOperand.getFirstComp()-1;
			idxSecondComp= currentOperand.getSecondComp()-1;
			DEBUG_INSTR(currentOperand.printOperation());
			// Validate the index to pick up from where index belongs
			// Case base, there is a single operand and not composed with or/and
			if(currentOperand.isSingleOperand()){
				DEBUG_MSG("Single operation"<<endl);
				if(vSlimFloatResults[idxFirstComp] != NULL){results.push_back(SetOperations::unionSets(*(new set<int>()), vSlimFloatResults[idxFirstComp])); }
				else if(vSlimStringResults[idxFirstComp] != NULL){ results.push_back(SetOperations::unionSets(*(new set<int>()), vSlimStringResults[idxFirstComp])); }
				else if(vbpFloatResults[idxFirstComp] != NULL){ results.push_back(SetOperations::unionSets(*(new set<int>()), vbpFloatResults[idxFirstComp])); }
				else if(vbpStringResults[idxFirstComp] != NULL){ results.push_back(SetOperations::unionSets(*(new set<int>()), vbpStringResults[idxFirstComp])); }
				else cout<<"ERROR at ArbSetOperations: Result Type not supported.";
			}
			// When the first and the second component are in the original result list
			else if(idxFirstComp < nrBaseOperands and idxSecondComp < nrBaseOperands){
				if(vSlimFloatResults[idxFirstComp] != NULL){
					if(vSlimFloatResults[idxSecondComp] != NULL){
						DEBUG_MSG("Both complex floats"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimFloatResults[idxFirstComp], vSlimFloatResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vSlimFloatResults[idxFirstComp], vSlimFloatResults[idxSecondComp]));
					}
					else if(vSlimStringResults[idxSecondComp] != NULL){
						DEBUG_MSG("First complex float, second complex string"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimFloatResults[idxFirstComp], vSlimStringResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vSlimFloatResults[idxFirstComp], vSlimStringResults[idxSecondComp]));
					}
					else if(vbpFloatResults[idxSecondComp] != NULL){
						DEBUG_MSG("First complex float, second simple float"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimFloatResults[idxFirstComp], vbpFloatResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vSlimFloatResults[idxFirstComp], vbpFloatResults[idxSecondComp]));
					}
					else if(vbpStringResults[idxSecondComp] != NULL){
						DEBUG_MSG("First complex float, second simple string"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimFloatResults[idxFirstComp], vbpStringResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vSlimFloatResults[idxFirstComp], vbpStringResults[idxSecondComp]));
					}
					else cout<<"ERROR at ArbSetOperations: Result Type not supported.";
				}
				else if(vSlimStringResults[idxFirstComp] != NULL){
					if(vSlimFloatResults[idxSecondComp] != NULL){
						DEBUG_MSG("First complex string, second complex float"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimStringResults[idxFirstComp], vSlimFloatResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vSlimStringResults[idxFirstComp], vSlimFloatResults[idxSecondComp]));
					}
					else if(vSlimStringResults[idxSecondComp] != NULL){
						DEBUG_MSG("Both complex strings"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimStringResults[idxFirstComp], vSlimStringResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vSlimStringResults[idxFirstComp], vSlimStringResults[idxSecondComp]));
					}
					else if(vbpFloatResults[idxSecondComp] != NULL){
						DEBUG_MSG("First complex string, second simple float"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimStringResults[idxFirstComp], vbpFloatResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vSlimStringResults[idxFirstComp], vbpFloatResults[idxSecondComp]));
					}
					else if(vbpStringResults[idxSecondComp] != NULL){
						DEBUG_MSG("First complex string, second simple string"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimStringResults[idxFirstComp], vbpStringResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vSlimStringResults[idxFirstComp], vbpStringResults[idxSecondComp]));
					}
					else cout<<"ERROR at ArbSetOperations: Result Type not supported.";
				}
				else if(vbpFloatResults[idxFirstComp] != NULL){
					if(vSlimFloatResults[idxSecondComp] != NULL){
						DEBUG_MSG("First simple float, second complex float"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpFloatResults[idxFirstComp], vSlimFloatResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vbpFloatResults[idxFirstComp], vSlimFloatResults[idxSecondComp]));
					}
					else if(vSlimStringResults[idxSecondComp] != NULL){
						DEBUG_MSG("First simple float, second complex string"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpFloatResults[idxFirstComp], vSlimStringResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vbpFloatResults[idxFirstComp], vSlimStringResults[idxSecondComp]));
					}
					else if(vbpFloatResults[idxSecondComp] != NULL){
						DEBUG_MSG("Both simple floats"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpFloatResults[idxFirstComp], vbpFloatResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vbpFloatResults[idxFirstComp], vbpFloatResults[idxSecondComp]));
					}
					else if(vbpStringResults[idxSecondComp] != NULL){
						DEBUG_MSG("First simple float, second simple string"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpFloatResults[idxFirstComp], vbpStringResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vbpFloatResults[idxFirstComp], vbpStringResults[idxSecondComp]));
					}
					else cout<<"ERROR at ArbSetOperations: Result Type not supported.";
				}
				else if(vbpStringResults[idxFirstComp] != NULL){
					if(vSlimFloatResults[idxSecondComp] != NULL){
						DEBUG_MSG("First simple string, second complex float"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpStringResults[idxFirstComp], vSlimFloatResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vbpStringResults[idxFirstComp], vSlimFloatResults[idxSecondComp]));
					}
					else if(vSlimStringResults[idxSecondComp] != NULL){
						DEBUG_MSG("First simple string, second complex string"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpStringResults[idxFirstComp], vSlimStringResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vbpStringResults[idxFirstComp], vSlimStringResults[idxSecondComp]));
					}
					else if(vbpFloatResults[idxSecondComp] != NULL){
						DEBUG_MSG("First simple string, second simple float"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpStringResults[idxFirstComp], vbpFloatResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vbpStringResults[idxFirstComp], vbpFloatResults[idxSecondComp]));
					}
					else if(vbpStringResults[idxSecondComp] != NULL){
						DEBUG_MSG("Both simple string"<<endl);
						if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpStringResults[idxFirstComp], vbpStringResults[idxSecondComp], (i==0)));
						else results.push_back(SetOperations::unionSets(vbpStringResults[idxFirstComp], vbpStringResults[idxSecondComp]));
					}
					else cout<<"ERROR at ArbSetOperations: Result Type not supported.";
				}
				else cout<<"ERROR at ArbSetOperations: Result Type not supported.";

			}
			// When just the first component is in the result list (the other is in our result set list)
			else if(idxFirstComp < nrBaseOperands){
				if(vSlimFloatResults[idxFirstComp] != NULL){
					DEBUG_MSG("First complex float and second set"<<endl);
					if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimFloatResults[idxFirstComp], results[idxSecondComp-nrBaseOperands], (i==0)));
					else results.push_back(SetOperations::unionSets(vSlimFloatResults[idxFirstComp], results[idxSecondComp-nrBaseOperands]));
				}
				else if(vSlimStringResults[idxFirstComp] != NULL){
					DEBUG_MSG("First complex string and second set"<<endl);
					if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vSlimStringResults[idxFirstComp], results[idxSecondComp-nrBaseOperands], (i==0)));
					else results.push_back(SetOperations::unionSets(vSlimStringResults[idxFirstComp], results[idxSecondComp-nrBaseOperands]));
				}
				else if(vbpFloatResults[idxFirstComp] != NULL){
					DEBUG_MSG("First simple float and second set"<<endl);
					if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpFloatResults[idxFirstComp], results[idxSecondComp-nrBaseOperands], (i==0)));
					else results.push_back(SetOperations::unionSets(vbpFloatResults[idxFirstComp], results[idxSecondComp-nrBaseOperands]));
				}
				else if(vbpStringResults[idxFirstComp] != NULL){
					DEBUG_MSG("First simple string and second set"<<endl);
					if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(vbpStringResults[idxFirstComp], results[idxSecondComp-nrBaseOperands], (i==0)));
					else results.push_back(SetOperations::unionSets(vbpStringResults[idxFirstComp], results[idxSecondComp-nrBaseOperands]));
				}
				else cout<<"ERROR at ArbSetOperations: Result Type not supported.";
			}
			// When just the second component is in the result list (the other is in our result set list)
			else if(idxSecondComp < nrBaseOperands){
				if(vSlimFloatResults[idxSecondComp] != NULL){
					DEBUG_MSG("First set and second complex float"<<endl);
					if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(results[idxFirstComp-nrBaseOperands], vSlimFloatResults[idxSecondComp], (i==0)));
					else results.push_back(SetOperations::unionSets(results[idxFirstComp-nrBaseOperands], vSlimFloatResults[idxSecondComp]));
				}
				else if(vSlimStringResults[idxSecondComp] != NULL){
					DEBUG_MSG("First set and second complex string"<<endl);
					if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(results[idxFirstComp-nrBaseOperands], vSlimStringResults[idxSecondComp], (i==0)));
					else results.push_back(SetOperations::unionSets(results[idxFirstComp-nrBaseOperands], vSlimStringResults[idxSecondComp]));
				}
				else if(vbpFloatResults[idxSecondComp] != NULL){
					DEBUG_MSG("First set and second simple float"<<endl);
					if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(results[idxFirstComp-nrBaseOperands], vbpFloatResults[idxSecondComp], (i==0)));
					else results.push_back(SetOperations::unionSets(results[idxFirstComp-nrBaseOperands], vbpFloatResults[idxSecondComp]));
				}
				else if(vbpStringResults[idxSecondComp] != NULL){
					DEBUG_MSG("First set and second simple string"<<endl);
					if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(results[idxFirstComp-nrBaseOperands], vbpStringResults[idxSecondComp], (i==0)));
					else results.push_back(SetOperations::unionSets(results[idxFirstComp-nrBaseOperands], vbpStringResults[idxSecondComp]));
				}
				else cout<<"ERROR at ArbSetOperations: Result Type not supported.";
			}
			else{
				DEBUG_MSG("Both sets"<<endl);
				if(currentOperand.getConnector() == "and") results.push_back(SetOperations::intersectSets(results[idxFirstComp-nrBaseOperands],results[idxSecondComp-nrBaseOperands], (i==0)));
				else results.push_back(SetOperations::unionSets(results[idxFirstComp-nrBaseOperands],results[idxSecondComp-nrBaseOperands]));
			}
			DEBUG_INSTR(cout<<"Current result:";SetOperations::printSet(results.back());)
		}
		return results.back();
	}

}; // End namespace ArbHierParser


#endif

