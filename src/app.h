//
// Created by gonzaga on 7/12/15.
//

#ifndef SIMDIV_APP_H
#define SIMDIV_APP_H

// Metric Tree includes
#include <ArboretumAPI.h>
#include <ArbSetOperations.h>
#include <DivisonQuery.h>
#include <sfp/simple_file_parser.h>
#include <HierarchyParser.h>
#include <bits/stdc++.h>
#include <ArbSetOperations.h>

using namespace std;

//TODO: dont limit group count
#define MAXGROUPS 10000

//---------------------------------------------------------------------------
// class TApp
//---------------------------------------------------------------------------

class TApp{

private:
	// For debug mode
	bool isDebugMode;

    //output time
    fstream fout;

    //variables to store filenames
    string dividend;
    string divisor;
    string condition;
    char* groups;
    int nrReqAtts;
    int nrRequisits; // nr of rows of divisor
    bool isForAll; // (0 = For Any), (1 = For All)

    // var for Hierarchy Operations
    OperationList operationList;

    //vector <stFloatArrayObject *> queryFloatArrayObjects;
    //vector <stCharArrayObject *> queryCharArrayObjects;

    // Vectors of atts values
    vector<vector<stFloatArrayObject*> > attsComplexFloatObjects;
    vector<vector<stCharArrayObject*> > attsComplexStringObjects;
    vector<vector<float> > attsSimpleFloatObjects;
    vector<vector<string> > attsSimpleStringObjects;
    //vector<multimap<float,int>> attsSimpleFloatObjects;
    //vector<multimap<string,int>> attsSimpleStringObjects;

    //DIVISION LOGIC ---------------------------------------------

    // map to store rows ids and the groups that this row belongs
    map<int,bitset<MAXGROUPS>*> groupsMap;

    //bitset to store the current valid groups
    bitset<MAXGROUPS> validGroups;


    map<int, vector<stFloatArrayObject*>*> groupsRows;

    //DIVISION LOGIC ---------------------------------------------



    //TREES --------------------------------------------------

    //vector to store attribute <name,type>
    vector<pair<string,string> > attributes;

    vector<DivisonQuery> queryAttributes;

    vector<stPlainDiskPageManager*> pageManagers;


    vector<stFloatArrayTree*> floatSlimTrees;
    vector<stCharArrayTree*> charSlimTrees;
    stGenericMetricTree * metricSlimTree;

    vector<bpFloatTree*> floatBPTrees;
    vector<bpStringTree*> stringBPTrees;


    vector<myFloatResult*> floatResults;
    vector<myCharArrayResult*> stringResults;
    vector<bpFloatResult*> bpFloatResults;
    vector<bpStringResult*> bpStringResults;
    //SLIM TREE --------------------------------------------------

    //void setNrReqs(int nrReqs);

    void LoadTree(string relationName, string attName, bool isSimple);

    void InputParser(char * fileName);

    void LoadComplexRequirementsFloat(simple_file_parser& sfp, int attSize, int attIndex, const char *requirementName);
    void LoadComplexRequirementsString(simple_file_parser& sfp, int attSize, int attIndex, const char *requirementName);
    void LoadSimpleRequirements(simple_file_parser& sfp, string attType, int attSize, int attIndex, const char *requirementName);
    void LoadRequirements(const char *reqName, bool isSimple);

    void ReadGroups(const char *filename);

    void IndexDivision();

    void FTSDivision();

    Attribute ReadAttributeDetails(string fileName, string att);


public:

    TApp(): nrReqAtts(0), isDebugMode(false) {}

    void setDebugMode(bool isDebugMode);

    void Init(char *query, char* groups_filename);

    void Run(string queryApproach);

    void Done();

};
#endif //SIMDIV_APP_H
