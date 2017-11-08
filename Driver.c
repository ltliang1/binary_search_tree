/*******************************************************************************

                                                        Lee Liang (cs12xkw)
                                                        Marlon Gamez (cs12xbq)
                                                        CSE 12, Spring 2017
                                                        June 6, 2017
                                Assignment 9

File Name:      Driver.c
Description:    This is the main driver that runs the progrram, prompting the
                user for input and calling the subsequent functions in Tree.c
                based on those inputs

*******************************************************************************/

#include <iostream>
#include <cstdio>
#include <string>
#include <getopt.h>
#include "Driver.h"
#include "SymTab.h"

#include <fstream>

using namespace std;

#ifdef NULL
#undef NULL
#define NULL 0
#endif

/*------------------------------------------------------------------------------
Function name:      operator <<
Purpose:            This function exists to return values of the UCSDStudent
                    object passed in to the stream passed in
Description:        This function returns the stream with the name and number of
                    the UCSDStudent appended
Input:              stream: reference to the stream to return value to
                    stu: reference to the UCSDStudent object to get name and
                    number value from
------------------------------------------------------------------------------*/
ostream & operator << (ostream & stream, const UCSDStudent & stu) {
        return stream << "name:  " << stu.name
                << " with studentnum:  " << stu.studentnum;
}

/*------------------------------------------------------------------------------
Function name:      main
Purpose:            This function exists to drive the program to test the
                    functionalities of the binary search tree
Description:        This function asks for input from the user, and the inserts,
                    looksup, remove, or writes out each element in the Tree
Input:              argc: the number of arguments in the command line
                    arvg: the char array of arguments in the command line
------------------------------------------------------------------------------*/
int main (int argc, char * const * argv) {
    char buffer[BUFSIZ];
    char command;           // char field for the command entered by the user
    long number;            // student number
    char option;            // flag entered by user

    istream * input = &cin;
    ostream * output = &cout;
    
    SymTab<UCSDStudent>::Set_Debug_Off ();

    while ((option = getopt (argc, argv, "x")) != EOF) {

    switch (option) {
        case 'x': SymTab<UCSDStudent>::Set_Debug_On ();
            break;
        }       
    }
    
    //THIS LINE EDITED FROM HW8
    SymTab<UCSDStudent> ST ("Driver.datafile");
    ST.Write (*output << "Initial Symbol Table:\n" );

    while (cin) {
            command = NULL;         // reset command each time in loop
            *output << "Please enter a command ((f)ile, (i)nsert, "
                    << "(l)ookup, (r)emove, (w)rite):  ";
            *input >> command;

        switch (command) {

            case 'f': {
                *output << "Please enter file name for commands:  ";
                *input >> buffer; // formatted input

                // delete istream and ostream for no memory leaks
                if (*input != cin) {
                    delete input;
                    delete output;
                }

                input = new ifstream (buffer);
                output = new ofstream ("/dev/null");
                break;
            }

            case 'i': {
                *output << "Please enter UCSD student name to insert:  ";
                *input >> buffer;  // formatted input

                *output << "Please enter UCSD student number:  ";
                *input >> number;

                UCSDStudent stu (buffer, number);

                // create student and place in symbol table
                ST.Insert (stu);
                break;
            }
            case 'l': { 
                unsigned long found;    // whether found or not

                *output << "Please enter UCSD student name to lookup:  ";
                *input >> buffer;  // formatted input

                UCSDStudent stu (buffer, 0);
                found = ST.Lookup (stu);
                
                if (found)
                        cout << "Student found!!!\n" << stu << "\n";
                else
                        cout << "student " << buffer << " not there!\n";
                break;
            }
            case 'r': { 
                unsigned long removed;

                *output << "Please enter UCSD student name to remove:  ";
                *input >> buffer;  // formatted input

                UCSDStudent stu (buffer, 0);
                removed = ST.Remove(stu);

                if (removed)
                        *output << "Student removed!!!\n" << stu << "\n";
                else
                        *output << "student " << buffer << " not there!\n";
                break;
            }
            case 'w': {
                ST.Write (cout << "The Symbol Table contains:\n");
                break;
            }
            default: {  // default call to reassign istream and ostream
                string line;

                if (*input != cin && !getline (*input, line)) {
                    delete input;
                    delete output;

                    input = &cin;
                    output = &cout;
                }
            }
        }
    }


    ST.Write (cout << "\nFinal Symbol Table:\n");

    if (ST.GetOperation() != 0) {
        cout << "\nCost of operations:    ";
        cout << ST.GetCost();
        cout << " tree accesses";

        cout << "\nNumber of operations:  ";
        cout << ST.GetOperation();

        cout << "\nAverage cost:          ";
        cout << (((float)(ST.GetCost()))/(ST.GetOperation()));
        cout << " tree accesses/operation\n";
    }
    else
        cout << "\nNo cost information available.\n";
}
