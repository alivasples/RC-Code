//
// Created by gonzaga on 7/12/15.
//
#pragma hdrstop

#include <iostream>
#include <iostream>
#include <app.h>

#pragma argsused

int main(int argc, char* argv[])
{
	// Validation of number of arguments for execution
    if(argc != 4 and argc != 5)
    {
        cout <<  "./simDiv QUERY [filename] TG [filename] QUERYTYPE[index | fts] [isDebugMode]" << endl;
        return 0;
    }
    // Definition of object
    TApp app;

    // In case we want to enter in debug mode
    if(argc == 5 and argv[4][0] == '1') {app.setDebugMode(true);}

    // Init application.
    app.Init(argv[1],argv[2]);

    // Run it.
    app.Run(argv[3]);

    // Release resources.
    app.Done();



    return 0;
}//end main
