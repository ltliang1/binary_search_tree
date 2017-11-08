/*******************************************************************************

                                                    Lee Liang (cs12xkw)
                                                    Marlon Gamez (cs12xbq)
                                                    CSE 12, Spring 2017
                                                    June 6, 2017
                              Assignment 9

File Name:      Driver.h
Description:    This file defines the UCSDStudent class, which serves as one of
                the objects that will be used to test the functionality of the
                Binary search tree data structure defined in Tree.c

*******************************************************************************/

#ifndef DRIVER_H
#define DRIVER_H

#include <string.h>
#include <iostream>
#include <cstdlib>
using namespace std;

//==============================================================================
// class UCSDStudent
//
// Description: Implements the UCSDStudent class, which is used as on the
//              objects used to test the binary search tree data structure
//
// Data Fields:
//     name (char[]) - name of the student
//     studentnum (long) - the number assigned to the student
//
// Public functions:
//     operator const char * - gives the name of the UCSDStudent
//     operator == - checks for name equality between two UCSDStudents
//     operator > - compares the names of two UCSDStudents
//==============================================================================
class UCSDStudent {
    friend ostream & operator << (ostream &, const UCSDStudent &);
    char name[16];
    long studentnum;

public:
    

    UCSDStudent () : studentnum (0) {}

    /* UCSDStudent constructor
     * constructor for the UCSDStudent class which creates an object given a
     * name and val to assign to studentnum
     *
     * @param  nm: name to assign to the object
     * @param val: value to assign to the studentnum
     */
    UCSDStudent (char * nm, long val) : studentnum (val) {

        for (unsigned int i = 0; i < sizeof (name); i++)
            name[i] = 0;
        

        strcpy (name, nm); 
    }

    /* const char * operator
     * used to convert a UCSDStudent object to a const char*
     *
     * @return the name field of the object
     */
    operator const char * (void) const {
        return name;
    }

    /* == operator
     * used to check for two UCSDStudent's having the same name
     *
     * @return the name field of the object
     */
    long operator == (const UCSDStudent & bbb) const {
        return ! strcmp (name, bbb);
    }

    /* > operator
     * used to compare two different UCSDStudents based on order of their names
     *
     * @return 1 (TRUE): if current objects name comes after bbb's
     *         0 (FALSE): otherwise
     */
    long operator > (const UCSDStudent & bbb) const {
        return (strcmp (name, bbb) > 0) ? 1 : 0;
    }

};

#endif
