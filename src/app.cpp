//
// Created by gonzaga on 7/12/15.
// Updated by alivasples on 26/10/19.
//

#include "app.h"

#define MYFILEERR(msg) cerr << endl << "ERROR:" << __FILE__ << ":" << __LINE__ << ": " << msg << endl; exit(1)
#define MYERR(msg) cerr << endl << "[ERROR] " << __LINE__ << ": " << msg << endl; exit(1)
#define DEBUG_MSG(msg) if(isDebugMode){cout<<msg;}

#pragma hdrstop
#pragma package(smart_init)


// METHODS

// Function that returns euclidian distance of two coords
double Euclidian(float *o1, float *o2)
{
    double distance = 0;

    for(int i=0; i < 2; i++)
    {
        distance += pow((o1[i] - o2[i]),2.0);
    }

    return sqrt(distance);
};




// Just for setting the debug mode
void TApp::setDebugMode(bool isDebugMode){
	this->isDebugMode = isDebugMode;
	operationList.setDebugMode(isDebugMode);
}

// Load values from Query and groups of tuples
void TApp::Init(char *query, char* groups_filename)
{
	// Load Dividend, divisor and Attributes L1 and L2
    InputParser(query);
    // Save the filename who has the groups
    groups = groups_filename;
}


// This process will execute the similarity division
void TApp::Run(string queryApproach)
{
	// Create / Open File [queryApproach.result]
	string mod = (isForAll?"_all":"_any");
    fout.open((queryApproach+mod+".result").c_str(), ios::out | ios::app);
    // Read the groups and map de i'th tuple to its group
    ReadGroups(groups);

    // Start time for execution time
    clock_t start;
    double duration;
    start = clock();

    // Select the approach and treat it as it corresponds
    if(queryApproach.compare("index") == 0)
    {
        IndexDivision();
    }
    else if(queryApproach.compare("fts") == 0)
    {
        FTSDivision();
    }

    // Calcule the duration of the program execution
    duration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
    // Show and save the execution time
    cout << "duration: "<< duration << endl;
    fout << duration << endl;
}


// This process Reads from a file the Dividend (file-name), divisor (file-name) and attributes L1/L2 names and thresholds
void TApp::InputParser(char * fileName)
{
    DEBUG_MSG("Reading Query\n");
    simple_file_parser sfp;

    if(!sfp.open(fileName))
    {
        MYERR("Cannot read file: '" << fileName << "'");
    }

    sfp.set_single_char_tokens(";");

    //read data header -----------------------------------------------
    sfp.get_next_line();

    //dividend and divisor relation names
    dividend = sfp.get_token(0);
    divisor = sfp.get_token(2);
    nrReqAtts = atoi(sfp.get_token(4).c_str());
    isForAll = (sfp.get_token(6)=="all"?true:false);
    // connected condition. Example: (1,or,2),and,3
    sfp.get_next_line();
    operationList.setNrBaseOperands(nrReqAtts);
    operationList.stringToHOperations(sfp.get_token(0));
    // Printing data
    DEBUG_MSG("  Dividend: "<<dividend<<endl);
    DEBUG_MSG("  divisor: "<<divisor<<endl);
    DEBUG_MSG("  Nr of base requirements attributes: "<<nrReqAtts<<endl);
    DEBUG_MSG("  Type: "<<(isForAll?"For All":"For Any")<<endl);
    DEBUG_MSG("  Operation: "<<sfp.get_token(0)<<endl);
    // Just for depuring
    // operationList.printOperations();

    // Reading attributes of L1 vs L2 and threshold for comparing
    while(sfp.get_next_line())
    {
        //read skill
        Attribute skill = ReadAttributeDetails(dividend,sfp.get_token(0));

        //read requirement
        Attribute requirement = ReadAttributeDetails(divisor,sfp.get_token(2));

        //skill,requirement, threshold
        DivisonQuery tmp(skill,requirement,sfp.get_token(4), strtof(sfp.get_token(6).c_str(),NULL) );
        // Add the query to the vector of query attributes
        queryAttributes.push_back(tmp);
    }

    sfp.close();
    DEBUG_MSG("Query successfully Read\n");
}

// Function who loads complex float requirements
void TApp::LoadComplexRequirementsFloat(simple_file_parser& sfp, int attSize, int attIndex, const char *requirementName){
	stFloatArrayObject *tmpFloat;
	string values; // It will store the current line attribute value

	// From now on, the only token will be ; (separation of data elements)
	sfp.set_single_char_tokens(";");


	// For every data line
	int rowId = 0;
	// We will consider as example the first data line (a;0.1,1.0;)
	while(sfp.get_next_line())
	{
		float f[attSize]; // Because it could be a multidimensional attribute, like in the example
		vector<float> floatValues; //for last arboretum version
		// In the line of example (el): el[2] = "0.1,1.0"
		values = sfp.get_token(attIndex * 2);

		//parse values
		std::istringstream iss(values); // Copy of value string "0.1,1.0"
		std::string token; // Where we store the current sub-value
		int i = 0;
		// While there exists a comma (it will be another data)
		while (std::getline(iss, token, ','))
		{
			// Casting the current sub-value to float
			f[i] = atof(token.c_str());
			floatValues.push_back(f[i]);
			i++;
		}

		// Create a new float array object containing the float values of current attribute
		// old arboretum
		//tmpFloat = new stFloatArrayObject(attSize, f);
		// last arboretum
		tmpFloat = new stFloatArrayObject(rowId,floatValues);

		// Add the float array object to the objects vector
		attsComplexFloatObjects.back().push_back(tmpFloat);
		// Next row
		rowId++;
	}

	// Just closing the file parsed
	sfp.close();
	// Leave a message in order to depuring
	DEBUG_MSG("Requirements successfully loaded\n");
}

// Function who loads complex string requirements
void TApp::LoadComplexRequirementsString(simple_file_parser& sfp, int attSize, int attIndex, const char *requirementName){
/*
	stCharArrayObject *tmpChar;
	string values; // It will store the current line attribute value

	// From now on, the only token will be ; (separation of data elements)
	sfp.set_single_char_tokens(";");

	// For every data line
	// We will consider as example the first data line (a;0.1,1.0;)
	while(sfp.get_next_line())
	{
		stCharArrayObject *tmpChar;
		// Just read the corresponding value to attribute order
		values = sfp.get_token(attIndex * 2);

		// Create a new char array object containing the char string value of current attribute
		tmpChar = new stCharArrayObject(attSize, values.c_str());
		// Add the char string object to the objects vector
		attsComplexStringObjects.back().push_back(tmpChar);
	}
*/
	// Just closing the file parsed
	sfp.close();
	// Leave a message in order to depuring
	DEBUG_MSG("Requirements successfully loaded\n");
}

// Function who loads simple float requirements
void TApp::LoadSimpleRequirements(simple_file_parser& sfp, string attType, int attSize, int attIndex, const char *requirementName){
	float floatValue;
	string strValue; // It will store the current line attribute value
	int rowId = 0;

	// From now on, the only token will be ; (separation of data elements)
	sfp.set_single_char_tokens(";");

	// For every data line
	// We will consider as example the first data line (a;0.1;)
	while(sfp.get_next_line())
	{
		// Go to the position of att value and store it
		strValue = sfp.get_token(attIndex * 2);
		if(attType == "float"){
			// Convert the string value to float
			floatValue = atof(strValue.c_str());
			// Add the new float value to the vector
			attsSimpleFloatObjects.back().push_back(floatValue);
			// Add the new float value to the tree
			//attsSimpleFloatObjects.back().insert(make_pair(floatValue, rowId));
		}
		else {
			// Add the new float value to the vector
			attsSimpleStringObjects.back().push_back(strValue);
			// Add the new string value to the tree
			//attsSimpleStringObjects.back().insert(make_pair(strValue, rowId));
		}
		rowId++;
	}

	// Just closing the file parsed
	sfp.close();
	// Leave a message in order to depuring
	DEBUG_MSG("Requirements successfully loaded\n");
}

// General function to load requirements
void TApp::LoadRequirements(const char *requirementName, bool isSimple)
{
	// The divisor file could store this lines (example)
	// ATT1(string,10);ATT2(float,2);
	// a;0.1,1.0;
	// a;0.15,15.0;
	// a;10.5,5.10;
	// For the example, let's assume that requirementName = ATT2

	// ========================== GENERAL STARTING ===================================
	// Leave a message in order to depuring
	DEBUG_MSG("Loading requirements of "<<requirementName << endl);
    // Defining our work variables
    int attIndex; // Order of attribute, for the example ATT2 is order 1 in header
    string attType; // type of attribute, for the example it is float
    int attSize; // size of attribute, for the example it is 2 (next to type)
    simple_file_parser sfp; // Just the file parser

    // Validate that file can be read
    if(!sfp.open(divisor))
    {
        MYERR("Cannot read file: '" << requirementName << "'");
    }
    // Set the tokens for separate input line
    sfp.set_single_char_tokens(";(),");

    // ===========================  READING THE HEADER ===============================
    // Read first line of document considering the tokens
    // Then we will have {"ATT1","(","string",",","10",")",";","ATT2","(","float",",","2",")",";"}
    sfp.get_next_line();

    int i = 0;
    // Linear searching of attribute name Bi
    while(sfp.get_token(i).compare(requirementName) != 0)
    {
        i+=7; // Jump to the next attribute name
    }
    // Set the metadata
    attType = sfp.get_token(i+2); // For the ATT2, it would be float
    attSize = sfp.get_token_uint(i+4); // For the ATT2, it would be 2
    attIndex = i/7; // For the ATT2, it would be 1

    // =============================  READING THE DATA =================================
    attsComplexFloatObjects.push_back(vector<stFloatArrayObject*>());
    attsComplexStringObjects.push_back(vector<stCharArrayObject*>());
    attsSimpleFloatObjects.push_back(vector<float>());
    attsSimpleStringObjects.push_back(vector<string>());
    if(isSimple){
    	LoadSimpleRequirements(sfp, attType, attSize, attIndex, requirementName);
    }
    else{
    	if(attType == "float") LoadComplexRequirementsFloat(sfp, attSize, attIndex, requirementName);
    	else LoadComplexRequirementsString(sfp, attSize, attIndex, requirementName);
    }
}

void TApp::ReadGroups(const char *filename)
{
	// ========================== GENERAL STARTING ===================================
	/* Let's have the next example
	 * filename = "TG.data"
	 * 0 0
	 * 1 0
	 * 2 1
	 * 3 0,1
	 * */
	// Show a message for depuration
	DEBUG_MSG("Reading groups\n");
    //TODO: long long int rid
    // Defining the vars for row and group ids
    int rowId, groupId;
    // Just a file parser
    simple_file_parser sfp;
    // Just openning the groups file
    sfp.open(filename);
    // Setting the tokens separators
    sfp.set_single_char_tokens("\t ,");

    // =========================== GROUPS MAPPING ====================================
    // For every line of content (rowId, groupId)
    while(sfp.get_next_line())
    {
    	// For current rowId, create a map to all groups (initiallized with 0)
        bitset<MAXGROUPS> bits;
        bits.reset();
        // Get the row id (the first token)
        rowId = sfp.get_token_uint(0);
        // All next tokens will be groups whom rowId belongs to
        for (int i = 1; i < sfp.get_num_tokens(); i+=2)
        {
        	// Get the token and map its group as true
            groupId = sfp.get_token_uint(i);
            bits[groupId] = true;
        }
        // Finally, add the whole vector of groups related to current rowId
        groupsMap[rowId] = new bitset<MAXGROUPS>(bits);
    }


    // Just closing the file parsed
    sfp.close();
    // Show a message for depuration
    DEBUG_MSG("Groups successfully read\n");
}

// Procedure that loads the SlimTree of relation (relationName) based on attribute index (attName)
// and simple/complex data (for choosing B+tree or SlimTree)
void TApp::LoadTree(string relationName, string attName, bool isSimple)
{
	// ========================== GENERAL STARTING ===================================
	// __Just for depuration__
	DEBUG_MSG("Loading Tree... ");
    // Vars definition
    simple_file_parser sfp; // Just a file parser
    string attType; // Stores the type of attribute indexed
    char str[100]; // Stores the file name of indexed (relationName_attName)
    strcpy(str,(relationName + "_" + attName).c_str());
    // __Just for depuration__
    DEBUG_MSG("... from file: "<<str<<endl);

	// ========================== READING HEADER ===================================
	// Create a page manager of indexed file
    pageManagers.push_back(new stPlainDiskPageManager(str));
    // Open original dividend file
    sfp.open(dividend);
    // Just setting the tokens separators
    sfp.set_single_char_tokens(";(), ");
    // Just reading the header of dividend
    sfp.get_next_line();
    // For every attribute in header
    for (int i = 0; i < sfp.get_num_tokens(); i+=7)
    {
    	// Stores the type of attribute that is indexed
        if(sfp.get_token(i).compare(attName) == 0)
        {
            attType = sfp.get_token(i+2);
        }
    }

    // Create B+Tree for simple attributes
    if(isSimple)
    {
    	// Float for float attribute
    	if(attType.compare("float") == 0)
		{
			floatBPTrees.push_back(new bpFloatTree(pageManagers.back()));
			stringBPTrees.push_back(NULL);
			floatSlimTrees.push_back(NULL);
			charSlimTrees.push_back(NULL);
			// __Just for depuration__
			DEBUG_MSG("B+ Float Tree Successfully Loaded.\n");
		}
		// ... Other way, create a Char Array Tree
		else
		{
			floatBPTrees.push_back(NULL);
			stringBPTrees.push_back(new bpStringTree(pageManagers.back()));
			floatSlimTrees.push_back(NULL);
			charSlimTrees.push_back(NULL);
			// __Just for depuration__
			DEBUG_MSG("B+ String Tree Successfully Loaded.\n");
		}
    }
    else{
    	// Create a Float Array Tree if the type of indexed att is float ...
		if(attType.compare("float") == 0)
		{
			floatBPTrees.push_back(NULL);
			stringBPTrees.push_back(NULL);
			floatSlimTrees.push_back(new stFloatArrayTree(pageManagers.back()));
			charSlimTrees.push_back(NULL);
			// __Just for depuration__
			DEBUG_MSG("Slim Float Tree Successfully Loaded.\n");
		}
		// ... Other way, create a Char Array Tree
		else
		{
			floatBPTrees.push_back(NULL);
			stringBPTrees.push_back(NULL);
			floatSlimTrees.push_back(NULL);
			charSlimTrees.push_back(new stCharArrayTree(pageManagers.back()));
			// __Just for depuration__
			DEBUG_MSG("Slim Char Tree Successfully Loaded.\n");
		}
    }

}


void TApp::IndexDivision()
{
	// __Just for depuring__
	DEBUG_MSG("Index Division Starting"<<endl);
	// Temporary vector of groups status from a specific row id
    bitset<MAXGROUPS> tmp;
    // Result ids of all reqs
    set<int> matchReqsIds;
    set<int>::iterator itReq;
    // All groups start as valid ones (true) when is for all, because set will be intersected and 1 and X is X
    if(isForAll) validGroups.set();
    // All groups start as not valid ones (false) when is for any, because set will be joined and 1 or X is X
    else validGroups.reset();
    // Assign size to results vectors
    floatResults.resize(nrReqAtts, NULL);
    stringResults.resize(nrReqAtts, NULL);
    bpFloatResults.resize(nrReqAtts, NULL);
    bpStringResults.resize(nrReqAtts, NULL);

    // For every pair of tuples to compare L1[Ai] vs L2[Bi]
    for(DivisonQuery attribute : queryAttributes)
    {
    	// Load the SlimTree for L1[Ai] (attribute) from source file (dividend)
        LoadTree(dividend, attribute.skill.name, attribute.isTraditionalCompare());
        // Load the list of requirements L2[Bi] (data)
        LoadRequirements(attribute.requirement.name.c_str(), attribute.isTraditionalCompare());
    }
    nrRequisits = attsComplexFloatObjects[0].size() + attsComplexStringObjects[0].size() + attsSimpleFloatObjects[0].size() + attsSimpleStringObjects[0].size();

	// For every divisor requirment of L2 (Bi)
    for(int currentReq=0; currentReq<nrRequisits; currentReq++){
    	// The current Attribute index
    	int currentAtt = 0;
    	// For every query of attributes t1[Ai], t2[Bi] to compare in tuple
		for (DivisonQuery attribute : queryAttributes){
			DEBUG_MSG("Req Row: "<<currentReq+1<<", Att col: "<<currentAtt+1<<endl);
			// Simple Float
			if(floatBPTrees[currentAtt] != NULL){
				// Query
				if(attribute.comparator == "=") bpFloatResults[currentAtt] = floatBPTrees[currentAtt]->QueryEqual(attsSimpleFloatObjects[currentAtt][currentReq] +attribute.threshold);
				else if(attribute.comparator == "<") bpFloatResults[currentAtt] = floatBPTrees[currentAtt]->QueryLessThan(attsSimpleFloatObjects[currentAtt][currentReq] +attribute.threshold);
				else if(attribute.comparator == "<=") bpFloatResults[currentAtt] = floatBPTrees[currentAtt]->QueryLessThanOrEqual(attsSimpleFloatObjects[currentAtt][currentReq] +attribute.threshold);
				else if(attribute.comparator == ">") bpFloatResults[currentAtt] = floatBPTrees[currentAtt]->QueryGreaterThan(attsSimpleFloatObjects[currentAtt][currentReq] +attribute.threshold);
				else if(attribute.comparator == ">=") bpFloatResults[currentAtt] = floatBPTrees[currentAtt]->QueryGreaterThanOrEqual(attsSimpleFloatObjects[currentAtt][currentReq] +attribute.threshold);
				DEBUG_MSG("This condition is satisfied by "<<bpFloatResults[currentAtt]->GetNumOfEntries()<<" float entries"<<endl);
				/*// Loop
				for(int i=0; i<bpFloatResults[currentAtt]->GetNumOfEntries(); i++){
					cout<<"holis "<< bpFloatResults[currentAtt]->GetPair(i)->GetObject()->GetOID() <<endl;
				}*/
			}
			// Simple String
			else if(stringBPTrees[currentAtt] != NULL){
				// Cast data
				//array<char,MAXCHARARRAY> searchKey;
				//memcpy(searchKey.data(),attsSimpleStringObjects[currentAtt][currentReq].c_str(),attsSimpleStringObjects[currentAtt][currentReq].length()+1);
				//cout<<".. "<<searchKey.data()<<endl;
				// QuerysearchKey
				bpStringResults[currentAtt] = stringBPTrees[currentAtt]->QueryEqual(MyString(0,attsSimpleStringObjects[currentAtt][currentReq]));

				DEBUG_MSG("This condition is satisfied by "<<bpStringResults[currentAtt]->GetNumOfEntries()<<" string entries"<<endl);
				/*// Loop
				for(int i=0; i<bpStringResults[currentAtt]->GetNumOfEntries(); i++){
					cout<<"holis "<< bpStringResults[currentAtt]->GetPair(i)->GetObject()->GetOID() <<endl;
					cout<<"bolis "<< bpStringResults[currentAtt]->GetPair(i)->GetKey().key <<endl;
				}*/
			}
			// Complex Float
			else if(floatSlimTrees[currentAtt] != NULL){
				// Range Query
				floatResults[currentAtt] = floatSlimTrees[currentAtt]->RangeQuery(attsComplexFloatObjects[currentAtt][currentReq], attribute.threshold);
				// Loop
				/*
				for(int i=0; i<floatResults[currentAtt]->GetNumOfEntries(); i++){
					cout<<"holis c "<< floatResults[currentAtt]->GetPair(i)->GetObject()->GetOID() <<endl;
				}
				*/
			}
			currentAtt++;
		} // End For every query of attributes t1[Ai], t2[Bi] to compare in tuple
		// Now process the sets
		ArbHierParser arbHierparser(isDebugMode, operationList, floatResults, stringResults, bpFloatResults, bpStringResults);
		matchReqsIds = arbHierparser.getOperationResult();
		// For every row id who satisfied the current composed requisite
		for (set<int>::iterator it=matchReqsIds.begin(); it!=matchReqsIds.end(); ++it){
			// Store in a temp var, all groups who are visited
			// For example, for rows r1: 0 1 1 0 and r2: 1 0 1 0, at the end of execution
			// tmp should be r1|r2|...|rn, in this case r1|r2 = 1 1 1 0
			tmp = *groupsMap[*it] | tmp;
		}
		// Make the intersection of valid groups who satisfied the current requirement (For All Option)
		if(isForAll) validGroups = validGroups & tmp;
		// or Joined when "For Any"
		else validGroups = validGroups | tmp;
		// Fill with 0's the tmp var, for next requirement in loop
		tmp.reset();
    } // End For every divisor requirment of L2 (Bi)

	// Printing the valid groups
	cout<<"Final Valid Groups (Quotient): {";
	for(int i=0; i<MAXGROUPS; i++){
		if(validGroups.test(i)) cout<<i<<",";
	}
	cout<<"}"<<endl;
    // __Just for depuring__
	DEBUG_MSG("Index Division Success"<<endl);
}

// Procedure to execute the Division by Full Table Scan Method
void TApp::FTSDivision()
{
	// ========================== GENERAL STARTING ===================================
	// Defining vars
	simple_file_parser sfpDividend; // just a file parser for dividend
    long long unsigned int rowId = 0; // the row id of dividend tuple
    long long unsigned int nrDivRows;
    int divisorSize = 0; // the number of requirements (tuples of divisor)
    int currentAtt;
    int currentReq;
    bool matchReq;
    bitset<50> matchReqs;

    // Counting tuples from divisor
    ifstream file(divisor.c_str());
    string str;
    while (getline(file, str))
    {
        divisorSize++;
    }
    // Creating the matrix of Groups X Requirements (M = |TG| x |T2|)
    //todo: dont limit to 500 the number of requirements
    vector<bitset<500> > requirementsValidator(MAXGROUPS);

    // Just validate we can open the dividend file
    if (!sfpDividend.open(dividend))
    {
        MYERR("Cannot read file: '" << dividend << "'");
    }
    // Just setting the tokens separator
    sfpDividend.set_single_char_tokens(";");

    // ========================== VERIFY THE REQUIREMENTS ===================================
    // Just reading the header
    sfpDividend.get_next_line();
    // For every query of attributes to compare in L1[Ai] vs L2[Bi] with threshold (header)
    //todo: char atributes...
    for (DivisonQuery attribute : queryAttributes)
    {
    	// Load the requisit values of current attribute (Bi) from divisor
        LoadRequirements(attribute.requirement.name.c_str(), attribute.isTraditionalCompare());
    }
    nrRequisits = attsComplexFloatObjects[0].size() + attsComplexStringObjects[0].size() + attsSimpleFloatObjects[0].size() + attsSimpleStringObjects[0].size();
	// For every row in Dividend
	while(sfpDividend.get_next_line())
	{
		// For every tuple requirement of divisor (t2 de T2), t2[Bi]
		for(currentReq = 0; currentReq < nrRequisits; currentReq++)
		{
			// Initialize current attribute order
			currentAtt = 0;
			DEBUG_MSG("Div Row: "<<rowId+1<<", Req Id: "<<currentReq+1<<endl);
			// For every query of attributes t1[Ai], t2[Bi] to compare in tuple
			for (DivisonQuery attribute : queryAttributes){
				// Read float row values
				string values = sfpDividend.get_token(attribute.skill.headerIndex * 2);

				// When Attribute is simple
				if(attribute.isTraditionalCompare()){
					// Simple float attribute
					if(attribute.skill.type == "float"){
						float floatValue = atof(values.c_str());
						DEBUG_MSG(floatValue<<" "<<attribute.comparator<<" "<<attsSimpleFloatObjects[currentAtt][currentReq]<<endl);
						if((attribute.comparator == "=" and floatValue == attsSimpleFloatObjects[currentAtt][currentReq]+attribute.threshold)
								or (attribute.comparator == "<" and floatValue < attsSimpleFloatObjects[currentAtt][currentReq]+attribute.threshold)
								or (attribute.comparator == ">" and floatValue > attsSimpleFloatObjects[currentAtt][currentReq]+attribute.threshold)
								or (attribute.comparator == "<=" and floatValue <= attsSimpleFloatObjects[currentAtt][currentReq]+attribute.threshold)
								or (attribute.comparator == ">=" and floatValue >= attsSimpleFloatObjects[currentAtt][currentReq]+attribute.threshold)
								){
							matchReqs[currentAtt] = true;
						}
						else matchReqs[currentAtt] = false;
					}
					// Simple string attribute
					else{
						if(attribute.comparator == "=" and values == attsSimpleStringObjects[currentAtt][currentReq]){
							matchReqs[currentAtt] = true;
						}
						else matchReqs[currentAtt] = false;
					}
				}
				// When Attribute is complex
				else{
					// Temporal array for skill (Ai)
					float f[attribute.skill.size];
					// for last arboretum version
					vector<float> floatValues;
					// Float Array Object for the current dividend row
					stFloatArrayObject *currentRow;
					// Parse values and fill the array f of current skill (Ai)
					std::istringstream iss(values);
					std::string token;
					int i = 0;
					while (std::getline(iss, token, ','))
					{
						f[i] = atof(token.c_str());
						floatValues.push_back(f[i]);
						i++;
					}
					// Create a float array object for current row, having the value of current tuple[Ai]
					//currentRow = new stFloatArrayObject(attribute.skill.size, f);
					currentRow = new stFloatArrayObject(1, floatValues);
					// The distance function for this float value will be euclidian
					//stBasicEuclideanMetricEvaluator<stFloatArrayObject> distanceFunction;
					EuclideanDistance<stFloatArrayObject> distanceFunction;
					// If the current requirement(Bi) is similar to the current row divisor (Ai)
					//if ((double)distanceFunction.GetDistance(currentRow, attsComplexFloatObjects[currentAtt][currentReq]) <= attribute.threshold)
					if ((double)distanceFunction.GetDistance(*currentRow, *attsComplexFloatObjects[currentAtt][currentReq]) <= attribute.threshold)
					{
						//matchReq = matchReq & true;
						matchReqs[currentAtt] = true;
					}
					else matchReqs[currentAtt] = false; //matchReq = matchReq & false;
				}

				currentAtt++;
			}// End For (every query of attributes t[Ai],t[Bi])

			// Compute a complete condition. Example: 1,or,(2,and,3)
			matchReq = operationList.getOperationResult(matchReqs);
			// If the requeriment have match
			if(matchReq){
				//getchar();
				// For every tuple group
				for (int idGroup = 0; idGroup < MAXGROUPS; idGroup++)
				{
					// Set as true the groups whom row belongs to
					if (groupsMap[rowId]->test(idGroup))
					{
						requirementsValidator[idGroup][currentReq] = true;
					} // End If
				} // End for (every tuple group)
			} // End If (requeriment have match)

		}// End for (every tuple t2 requirement in divisor T2[Bi])
		rowId++;
	}// End While (for every row in Dividend)
    // The total of dividend rows is equal to the last counted
    nrDivRows = rowId;

    // ========================== VERIFY THE VALID GROUPS ===================================
    // NOTE: For next update, many of the lines below probably could be inside the dividend loop
    // Reporting the answer
    // FOR ALL VERSION
    if(isForAll){
		cout << "\nQuocient: {";
		// For every group
		for (int idGroup = 0; idGroup < MAXGROUPS; idGroup++)
		{
			// Current group begin as valid
			bool passed = true;
			// For every divisor row in last Bn
			for (currentReq = 0; currentReq < nrRequisits and passed; currentReq++)
			{
				// If there exists a requirement not matched, then the group does not belong to quotient
				if(!requirementsValidator[idGroup].test(currentReq))
				{
					passed = false;
				}
			}
			// Just showing a group who has all requirements matched
			if(passed)
			{
				cout << idGroup << ", ";
			}
		}
		// Ending FTS division
		cout << "}" << endl << endl;
    }
    // FOR ANY VERSION
    else{
    	cout << "\nQuocient: {";
		// For every group
		for (int idGroup = 0; idGroup < MAXGROUPS; idGroup++)
		{
			// Current group begin as valid
			bool passed = false;
			// For every divisor row in last Bn
			for (currentReq = 0; currentReq < nrRequisits and !passed; currentReq++)
			{
				// If there exists a requirement not matched, then the group does not belong to quotient
				if(requirementsValidator[idGroup].test(currentReq))
				{
					passed = true;
				}
			}
			// Just showing a group who has all requirements matched
			if(passed)
			{
				cout << idGroup << ", ";
			}
		}
		// Ending FTS division
		cout << "}" << endl << endl;
    }
}


Attribute TApp::ReadAttributeDetails(string fileName, string attName)
{
	DEBUG_MSG("Reading Attribute Details from "<<fileName<<"("<<attName<<")"<<endl);
	/* Let's have an entry example
	 * ATT1(string,10);ATT2(float,2);
	 * a;0.1,1.0;
	 * a;0.15,15.0;
	 * a;10.5,5.10;
	 * For the example, let's assume that attName = ATT2
	 * */
	// Just a file parser
    simple_file_parser sfp;
    // Open File Validation
    if (sfp.open(fileName))
    {
    	// Set the tokens separators
        sfp.set_single_char_tokens(";(),");

        // Just reading the header
        sfp.get_next_line();
        // Search for the attribute [attName] in the header
        int i = 0;
        while (sfp.get_token(i).compare(attName) != 0)
        {
            i += 7;
        }
        // Construct an attribute, for the examen it will be ("ATT2","float",2,1)
        Attribute att(attName,sfp.get_token(i+2),sfp.get_token_uint(i+4),i/7);
        // Just closing the file parsed
        sfp.close();
        // return the attribue we created
        DEBUG_MSG("Details Successfully Read"<<endl);
        return att;
    }
    else MYERR("Cannot read file: '" << fileName << "'");
}



void TApp::Done()
{

}//end TApp::Done
