/*******************************************************************************

                                                        Lee Liang (cs12xkw)
                                                        Marlon Gamez (cs12xbq)
                                                        CSE 12, Spring 2017
                                                        June 7, 2017
                                Assignment 9

File Name:      Tree.c
Description:    This program represents the binary search tree in C++, including
                the insert, lookup, remove, and write functionalities, while
                also writing data to the disk in a Driver.datafile file

--------------------------------------------------------------------------------

struct TNode

Description: Implements the node of the binary search tree data structure.
             Each node contains two children, the left child which contains
             data "less than" the data of the current node, and the right child
             which contains data "greater than" the data of the current node.

Data Fields:
    data (Whatever)         - the UCSDStudent object that is stored in
                              the TNode
    height (long)           - the height of the current TNode
    balance (long)          - the balance of the current TNode
    left (offset)           - the pointer to the left child
    right (offset)          - the pointer to the right child
    this_position (offset)  - current position in the tree

Public Functions:
    Insert              - inserts a new TNode recursively
    Lookup              - looks up a TNode recursively
    Remove              - removes a TNode recursively
    ReplaceAndRemoveMax - replaces the root data to the max value in the left
                          side of the tree and reinserts that data value
    SetHeightAndBalance - sets the height and balance of the TNodes
    Write               - update node to the disk
    Write_AllTNodes     - display the current TNode and it's children

--------------------------------------------------------------------------------

class Tree

Description: Implements the binary search tree generic container. Each Tree
             contains a root that serves as the entrance into the binary tree.

Data Fields:
    debug_on (int)      - flag for printing debug messages
    cost (long)         - cost of operations performed (reading/writing)
    operation (long)    - number of operations performed
    fstream (fio *)     - the datafile to write to
    occupancy (long)    - the number of elements of the Tree
    root (offset)       - the position of the root of the Tree
    tree_count (long)   - the count of the current Tree

Public Functions:
    Set_Debug_On        - sets debugging on
    Set_Debug_Off       - sets debugging off
    Insert              - inserts the root if it doesn't exist, otherwise
                          delegates it to TNode's insert
    Lookup              - looks for a TNode that contains the UCSDStudent with a
                          certain name
    Remove              - delegates removing of a TNode to TNode's Remove,
                          making a new TNode to pass in as a parameter
    Write               - writes out the contents of the Tree into the stream
    GetCost             - returns the cost of operations
    GetOperation        - returns the number of operations
    IncrementCost       - increments the cost field
    IncrementOperation  - increments the operation field

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "Tree.h"

// debug messages
static const char ALLOCATE[] = " - Allocating]\n";
static const char COST_READ[] = "[Cost Increment (Disk Access): Reading ";
static const char COST_WRITE[] = "[Cost Increment (Disk Access): Writing ";
static const char DEALLOCATE[] = " - Deallocating]\n";
static const char TREE[] = "[Tree ";

template <class Whatever>
int Tree<Whatever>::debug_on = 0;       // flag for debug messages

template <class Whatever>
long Tree<Whatever>::cost = 0;          // cost of operations (reading/writing)

template <class Whatever>
long Tree<Whatever>::operation = 0;     // number of operations performed

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define THRESHOLD 2

// overloaded << operator
template <class Whatever>
ostream & operator << (ostream &, const TNode<Whatever> &);

template <class Whatever>
struct  TNode {
// friends:

// data fields:
	Whatever data;          // UCSDStudent object
	long height;            // height of current TNode
	long balance;           // balance of tree
	offset left;            // left child node
	offset right;           // right child node
	offset this_position;	// current position

// function fields:
	TNode () : height (0), balance (0), left (0), right (0), 
		this_position (0) {}

	// to declare the working TNode in Tree's Remove
	TNode (Whatever & element) : data (element), height (0), balance (0),
		left (0), right (0), this_position (0) {}
	
	TNode (Whatever &, fstream *, long &);	// to add new node to disk
	TNode (const offset &, fstream *);	// to read node from disk
	
	unsigned long Insert (Whatever &, fstream *, long &, offset &);

	// optional recursive Lookup declaration would go here
    unsigned long Lookup (Whatever & element, fstream * const fio, 
    offset const& PositionInParent) const;

	void Read (const offset &, fstream *);	// read node from disk
	unsigned long Remove (TNode<Whatever> &, fstream *, long &, offset &,
		long fromSHB = FALSE);
	void ReplaceAndRemoveMax (TNode<Whatever> &, fstream *, offset &);
	void SetHeightAndBalance (fstream *, offset &);
	void Write (fstream *) const;		// update node to disk

	ostream & Write_AllTNodes (ostream &, fstream *) const;
};

/* Set Debug On
 * toggles the debug mode on, adding extra print statements
 */
template <class Whatever>
void Tree<Whatever> :: Set_Debug_On ()
/*******************************************************************************
% Routine Name : Tree :: Set_Debug_On (public)
% File :         Tree.c
% 
% Description : This function sets the debug flag on
*******************************************************************************/
{
    //turn off debug toggle
    Tree<Whatever>::debug_on = 1;
}

/* Set Debug Off
 * toggles the debug mode off, removing extra print statements
 */
template <class Whatever>
void Tree<Whatever> :: Set_Debug_Off ()
/*******************************************************************************
% Routine Name : Tree :: Set_Debug_Off (public)
% File :         Tree.c
% 
% Description : This function sets the debug flag off
*******************************************************************************/
{
    //turn on debug toggle
    Tree<Whatever>::debug_on = 0;
}

template <class Whatever>
unsigned long Tree<Whatever> :: Insert (Whatever & element)
/*******************************************************************************
% Routine Name : Tree :: Insert (public)
% File :         Tree.c
% 
% Description : This function inserts a new TNode with the data element into the
%               corresponding places based on the current TNodes in the Tree
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% element            a reference to the UCSDStudent object to insert
% <return>           TRUE is insertion succeeds, FALSE if not
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in Tree's Insert\n";

	// check for first item insertion
    if (occupancy == 0) {

        //insert at root using Write constuctor
        TNode<Whatever> newRoot (element, fio, occupancy);

        root = newRoot.this_position;

    // delegate to TNode's Insert
    } else {

    	//read root TNode data from file
    	TNode<Whatever> readRootNode (root, fio);

    	//delegate to TNode's insert method
    	readRootNode.Insert (element, fio, occupancy, root);
    }

    IncrementOperation ();

    return TRUE;

}

template <class Whatever>
void TNode<Whatever> :: ReplaceAndRemoveMax (TNode<Whatever> & targetTNode, 
	fstream * fio, offset & PositionInParent)
/*******************************************************************************
% Routine Name : TNode :: ReplaceAndRemoveMax (public)
% File :         Tree.c
% 
% Description : This function is called when a TNode has a balance over the
%               threshold, replaces the data of the root node to the largest
%               value to the left of the root, and reinserts the original node
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% targetTNode        a reference to the TNode to replace
% fio                a pointer to the fstream to write to
% PositionInParent   a reference to the offset of the current TNode
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's ReplaceAndRemoveMax\n";

    // keep traversing tree until max is found
    if (right != 0) {

    	TNode<Whatever> readRightNode (right, fio);

        readRightNode.ReplaceAndRemoveMax (targetTNode, fio, right);

        SetHeightAndBalance (fio, PositionInParent);
    }

    // replace data of target TNode
    else {

        targetTNode.data = data;

        PositionInParent = (left != 0) ? left : right;
    }
    
}

template <class Whatever>
unsigned long TNode<Whatever> :(TNode<Whatever> & elementTNode,
	fstream * fio, long & occupancy, offset & PositionInParent,
	long fromSHB)
/*******************************************************************************
% Routine Name : TNode :: Remove (public)
% File :         Tree.c
% 
% Description : This function removes a TNode from the Tree and traverses the
%               Tree using recursion
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% elementTNode       a reference to the TNode to remove
% fio                a reference to the fstream to write to
% occupancy          a reference to the occupancy of the Tree
% PositionInParent   a reference to the offset of the current TNode
% <return>           TRUE if remove successful, FALSE if not
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's Remove\n";

	long returnVal; // return value

    // check for matching data
    if (elementTNode.data == data) {

        returnVal = TRUE;

        occupancy--;

        // set return data to data
        elementTNode.data = data;

        // TNode if no children
        if ((left == 0) && (right == 0)) {
            PositionInParent = 0;
        }
        // reattach TNode descendents to parent TNode if one child
        else if ((left != 0) ^ (right != 0)) {
            PositionInParent = (left != 0) ? left : right;
        }
        // for two children
        else {

        	//read in left node from datafile
        	TNode<Whatever> readLeftNode (left, fio);

            readLeftNode.ReplaceAndRemoveMax (*this, fio, left);

            long fakeOcc = -1;

            // reset height and balance when not called from SHB
            if (!fromSHB)
                SetHeightAndBalance (fio, PositionInParent);

            // reinsert node if from SHB
            else
                Insert (data, fio, fakeOcc, PositionInParent);
        }

    // check to go right
    } else if (elementTNode.data > data) {
        
        if (right == 0)
            returnVal = FALSE;
        // go right
        else {

        	//read in right node from datafile
        	TNode<Whatever> readRightNode (right, fio);

            returnVal = readRightNode.Remove (elementTNode, fio, occupancy,
                                                                    right, 0);

            // recalculate height and balance if call isn't from
            // SetHeightAndBalance
            if (!fromSHB)
                SetHeightAndBalance (fio, PositionInParent);
        }
    // go left
    } else {

        if (left == 0)
            returnVal = FALSE;
        else {

        	TNode<Whatever> readLeftNode (left, fio);

            returnVal = readLeftNode.Remove (elementTNode, fio, occupancy,
                                                                    left, 0);

            // recalculate height and balance if call isn't from
            // SetHeightAndBalance
            if (!fromSHB)
                SetHeightAndBalance (fio, PositionInParent);
        }
    }

    return returnVal;

}
	
template <class Whatever>
unsigned long Tree<Whatever> :: Remove (Whatever & element)
/*******************************************************************************
% Routine Name : Tree :: Insert (public)
% File :         Tree.c
% 
% Description : This function removes a TNode with the data that has the same
%               UCSDStudent name as the element passed in
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% element            a reference to the UCSDStudent object to remove
% <return>           TRUE is remove succeeds, FALSE if not
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in Tree's Remove\n";
	
	long removeSuccess = 0; // return value for successful remove

    // check if root exists
    if (occupancy == 0)
        removeSuccess = FALSE;

    // delegate to TNode's Remove
    else {

        // remove TNode with corresponding data value
        TNode<Whatever> readRootNode (root, fio);

        // temporary TNode to save data value
        TNode<Whatever> toRemove (element);

        removeSuccess = readRootNode.Remove (toRemove, fio, occupancy, root, 0);

        element = toRemove.data;

        // reset root if all nodes are removed
        if (occupancy == 0)
        	ResetRoot ();
    }

    IncrementOperation ();

    return removeSuccess;
}

template <class Whatever>
void TNode<Whatever> :: SetHeightAndBalance (fstream * fio,
	offset & PositionInParent)
/*******************************************************************************
% Routine Name : TNode :: SetHeightAndBalance (public)
% File :         Tree.c
% 
% Description : This function resets the height and balance of the TNodes
%               that precede the TNode passed in
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% fio                a pointer to the fstream to write to
% PositionInParent   a reference to the offset of the current TNode
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's SetHeightAndBalance\n";

	// variables to hold height of left and right children
    long leftHeight = -1;
    long rightHeight = -1;

    if (left != 0) {
        TNode<Whatever> readLeftNode (left, fio);
        leftHeight = readLeftNode.height;
    }
    if (right != 0) {
        TNode<Whatever> readRightNode (right, fio);
        rightHeight = readRightNode.height;
    }

    // set working balance
    balance = leftHeight - rightHeight;

    // set working height
    height = (leftHeight > rightHeight) ? leftHeight + 1 : rightHeight + 1;

    // restructure tree if balance is above the threshold
    if (abs (balance) > THRESHOLD) {
        
        // fake occupancy variable used to pass to remove and insert
        long fakeOcc = 0;

        // temporary node to save data value to reinsert
        TNode<Whatever> tempNode (data);

        Remove (*this, fio, fakeOcc, PositionInParent, TRUE);

        // read TNode to insert data back in
        TNode<Whatever> removeTNode (PositionInParent, fio);

        removeTNode.Insert (tempNode.data, fio, fakeOcc, PositionInParent);
    }
    else
        Write (fio);
}

template <class Whatever>
long Tree <Whatever> :: GetCost ()
/*******************************************************************************
% Routine Name : Tree :: GetCost (public)
% File :         Tree.c
% 
% Description : This function returns the cost field of Tree
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% <return>           Tree's cost data field
*******************************************************************************/
{
	
	//return value of cost variable
	return Tree<Whatever> :: cost;
}

template <class Whatever>
long Tree <Whatever> :: GetOperation ()
/*******************************************************************************
% Routine Name : Tree :: GetOperation (public)
% File :         Tree.c
% 
% Description : This function returns the operation field of Tree
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% <return>           Tree's operation data field
*******************************************************************************/
{
	
	//return value of operation variable
	return Tree<Whatever> :: operation;
}

template <class Whatever>
void Tree <Whatever> :: IncrementCost ()
/*******************************************************************************
% Routine Name : Tree :: IncrementCost (public)
% File :         Tree.c
% 
% Description : This function increments the cost field of Tree
*******************************************************************************/
{

	//calls increment operator on cost
	Tree<Whatever> :: cost++;
}

template <class Whatever>
void Tree <Whatever> :: IncrementOperation ()
/*******************************************************************************
% Routine Name : Tree :: IncrementOperation (public)
% File :         Tree.c
% 
% Description : This function increments the operation field of Tree
*******************************************************************************/
{
	
	//calls increment operator on operation
	Tree<Whatever> :: operation++;
}

template <class Whatever>
void Tree <Whatever> :: ResetRoot ()
/*******************************************************************************
% Routine Name : Tree :: ResetRoot (public)
% File :         Tree.c
% 
% Description : This function resets the root of the Tree by setting the put
%               pointer of the fstream to the end and setting root to the point
*******************************************************************************/
{
    if (!*fio)
        cerr << "fio is currupted in Tree's ResetRoot\n";

    fio->seekp (0, ios :: end);    
    root = fio->tellp ();
}

template <class Whatever>
unsigned long TNode<Whatever> :: Insert (Whatever & element, fstream * fio,
	long & occupancy, offset & PositionInParent)
/*******************************************************************************
% Routine Name : TNode :: Insert (public)
% File :         Tree.c
% 
% Description : This function recursively inserts a new TNode with the data
%               element into the corresponding places based on the current
%               TNodes in the Tree
%
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% element            a reference to the UCSDStudent object to insert
% fio                a pointer to the fstream to write to
% occupancy          a reference to the occupancy of the Tree
% PositionInParent   a reference to the offset of the current TNode
% <return>           TRUE is insertion succeeds, FALSE if not
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's Insert\n";
	
	// check for duplicates
    if (element == data) {

        // replace working data with new data
        data = element;

        Write (fio);
    }
    // check to go right
    else {

        if (element > data) {
            
            // go right
            if (right == 0) {

            	//write new TNode to datafile
                TNode<Whatever> rightNode (element, fio, occupancy);

                //assign new TNode's offset to current TNode's right field
                right = rightNode.this_position;

            }
            // insert somewhere down the right
            else {

            	//read existing TNode in from datafile
            	TNode<Whatever> readRightNode (right, fio);

            	//call insert method on created TNode to insert further down
                readRightNode.Insert (element, fio, occupancy, right);
            }
        // go left
        } else {

            if (left == 0) {

            	//write new TNode to datafile
            	TNode<Whatever> leftNode (element, fio, occupancy);

            	//assign new TNode's offset to current TNode's left field
            	left = leftNode.this_position;

            }
            // insert somewhere down the left
            else {
                
                //read existing TNode in from datafile
                TNode<Whatever> readLeftNode (left, fio);

                //call insert method on created TNode to insert further down
                readLeftNode.Insert (element, fio, occupancy, left);
            }
        }

        // reset height and balance if call is not from Remove
        if (occupancy > -1)
            SetHeightAndBalance (fio, PositionInParent);
    }

    return TRUE;
}

template <class Whatever>
unsigned long TNode<Whatever> :: Lookup (Whatever & element,
    fstream * const fio, offset const & PositionInParent) const
/*******************************************************************************
% Routine Name : TNode :: Lookup (public)
% File :         Tree.c
% 
% Description : This function recursively locates a desired UCSDStudent object
%
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% element            a reference to the UCSDStudent object to lookup
% fio                a pointer to the fstream to write to
% PositionInParent   a reference to the offset of the current TNode
% <return>           TRUE if object is found, FALSE if not
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's Lookup\n";

    long returnVal; // return value

    // check for matching data
    if (element == data) {

        element = data;
        returnVal = TRUE;

    } 
    // check to go right
    else if (element > data) {
        
        if (right == 0)
            returnVal = FALSE;

        // go right
        else {

            TNode<Whatever> readRightNode (right, fio);

            returnVal = readRightNode.Lookup (element, fio, right);
        }

    } 
    // go left
    else {

        if (left == 0)
            returnVal = FALSE;

        else {
            
            TNode<Whatever> readLeftNode (left, fio);

            returnVal = readLeftNode.Lookup (element, fio, left);
        }
    }

    return returnVal;
}

template <class Whatever>
unsigned long Tree<Whatever> :: Lookup (Whatever & element) const
/*******************************************************************************
% Routine Name : Tree :: Lookup (public)
% File :         Tree.c
% 
% Description : This function looks for a TNode with the data that has the same
%               UCSDStudent name as the element passed in
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% element            a reference to the UCSDStudent object to lookup
% <return>           TRUE if object is found, FALSE if not
*******************************************************************************/
{

    long status;

    if (!*fio)
        cerr << "fio is currupted in Tree's Lookup\n";

    IncrementOperation ();

	// empty Tree
    if (occupancy == 0)
        status = FALSE;

    else {
        // read in root
        TNode<Whatever> readRootNode (root, fio);

        // set status to the call to TNode's recusive lookup
        status = readRootNode.Lookup (element, fio, root);
    }

    return status;
}

template <class Whatever>
void TNode<Whatever> :: Read (const offset & position, fstream * fio)
/*******************************************************************************
% Routine Name : TNode :: Read (public)
% File :         Tree.c
% 
% Description : This function reads in the node data from the data file that is
%               written on the disk
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% position           a reference to the offset of the position to read from
% fio                a pointer to the fstream to read from
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's Read\n";
	
	// 5-6 lines of code according to TA
	// move the g pointer to the position
	fio->seekg (position);

	// read node data in from datafile and increment cost
	fio->read ((char *) this, sizeof (*this));
	Tree<Whatever> :: IncrementCost ();

    // if debug on, print message
	if (Tree<Whatever> :: debug_on)
		cerr << COST_READ << (const char *) data << "]\n";

}

template <class Whatever>
TNode<Whatever> :: TNode (const offset & position, fstream * fio)
/*******************************************************************************
% Routine Name : TNode :: TNode (public)
% File :         Tree.c
% 
% Description : TNode's Read constructor
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% position           a reference to the offset of the position to read from
% fio                a pointer to the fstream to read from
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's read constructor\n";
	
	//delegate reading to the Read method
	Read (position, fio);
}

template <class Whatever>
TNode<Whatever> :: TNode (Whatever & element, fstream * fio, long & occupancy): 
			data (element), height (0), balance (0), left (0), 
			right (0)
/*******************************************************************************
% Routine Name : TNode :: TNode (public)
% File :         Tree.c
% 
% Description : TNode's Write constructor
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% element            a reference to the UCSDStudent object to write
% fio                a pointer to the fstream to write to
% occupancy          a reference to the occupancy of the Tree
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's write constructor\n";

	//increment occupancy
	occupancy++;

	//seek to the end of the file with p pointer, where the new node should go
	fio->seekp (0, ios :: end);

	//update this position field via tellp ()
	this_position = fio->tellp ();

	//call write method
	Write (fio);
}

template <class Whatever>
void TNode<Whatever> :: Write (fstream * fio) const
/*******************************************************************************
% Routine Name : TNode :: Write (public)
% File :         Tree.c
% 
% Description : This function writes the current TNode's data into the datafile
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% fio                a pointer to the fstream to write to
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's Write\n";

	//seek the p pointer to this_position
	fio->seekp (this_position);

	//write TNodes data to datafile and increment cost
	fio->write ((const char *) this, sizeof (*this));
	Tree<Whatever> :: IncrementCost ();

    // if debug on, print message
	if (Tree<Whatever> :: debug_on)
		cerr << COST_WRITE << (const char *) data << "]\n";
	
}

template <class Whatever>
Tree<Whatever> :: Tree (const char * datafile) :
	fio (new fstream (datafile, ios :: out | ios :: in))
/*******************************************************************************
% Routine Name : Tree<Whatever> :: Tree  (public)
% File :         Tree.c
% 
% Description :  Tree's constructor that guarantees initialization for fio and 
%                initializes the tree_count using a static counter.
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% datafile           a pointer to the datafile to make a new fstream from
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in Tree's constructor\n";

	//guaranteed initialization for fio, in and out are static constants in ios

	//increment tree count
	static int tree_counter = 0;
	tree_count = ++tree_counter;

    // if debug on, print message
	if (debug_on) {
		cerr << TREE << tree_count << ALLOCATE;
	}

	occupancy = 0;
    root = 0;

    // check for empty file
    fio->seekp (0, ios :: beg);
    offset beginning = fio->tellp ();

    fio->seekp (0, ios :: end);
    offset ending = fio->tellp ();

    //check for an empty datafile
    if (beginning == ending) {
        //empty file: need to reserve space
        fio->seekp (0, ios :: beg);
        fio->write ((const char*) &root, sizeof (root));
        fio->write ((const char*) &occupancy, sizeof (occupancy));

        //after this, the put pointer is at 16
        root = fio->tellp ();
    }
    //read from disk for root / occupancy
    else {
        //read from disk for root / occupancy

        //we've got at least one node

        //move g pointer to the beginning of the file
    	fio->seekg (0, ios :: beg);

        //finally, read in the root and the occupancy
        fio->read ((char *) &root, sizeof (root));
        fio->read ((char *) &occupancy, sizeof (occupancy));
    }

}

template <class Whatever>
Tree<Whatever> :: ~Tree (void)
/***************************************************************************
% Routine Name : Tree :: ~Tree  (public)
% File :         Tree.c
% 
% Description :  deallocates memory associated with the Tree.  It
%                will also delete all the memory of the elements within
%                the table.
***************************************************************************/

{

    if (!*fio)
        cerr << "fio is currupted in Tree's destructor\n";

    // if debug on, print message
	if (debug_on) {
		cerr << TREE << tree_count << DEALLOCATE;
	}

	// move p to the beginning
	fio->seekp (0, ios :: beg);

	// update root and occupancy by writing to the file
	fio->write ((const char *) &root, sizeof (root));
	fio->write ((const char *) &occupancy, sizeof (occupancy));

	// delete fio (it's on the heap)
	delete fio;

}	/* end: ~Tree */

template <class Whatever>
ostream & operator << (ostream & stream, const TNode<Whatever> & nnn)
/*******************************************************************************
% Routine Name : operator << (public)
% File :         Tree.c
% 
% Description : This overloaded operator << appends information from the TNode
%               reference passed in to the ostream
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% stream             a reference to the stream to print out to
% nnn                a TNode reference to append to the stream
% <return>           the stream with the appended values
*******************************************************************************/
{
	stream << "at height:  :" << nnn.height << " with balance:  "
		<< nnn.balance << "  ";
	return stream << nnn.data << "\n";
}

template <class Whatever>
ostream & Tree<Whatever> :: Write (ostream & stream) const
/*******************************************************************************
% Routine Name : Tree :: Write (public)
% File :         Tree.c
% 
% Description : This funtion will output the contents of the Tree table
%               to the stream specificed by the caller.  The stream could be
%               cerr, cout, or any other valid stream.
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% stream             A reference to the output stream.
% <return>           A reference to the output stream.
*******************************************************************************/

{

    if (!*fio)
        cerr << "fio is currupted in Tree's Write\n";

    long old_cost = cost; // old cost of tree operations

	stream << "Tree " << tree_count << ":\n"
		<< "occupancy is " << occupancy << " elements.\n";

	fio->seekg (0, ios :: end);
	offset end = fio->tellg ();

	// check for new file
	if (root != end) {
		TNode<Whatever> readRootNode (root, fio);
		readRootNode.Write_AllTNodes (stream, fio);
	}

        // ignore cost when displaying nodes to users
        cost = old_cost;

	return stream;
}

template <class Whatever>
ostream & TNode<Whatever> ::
Write_AllTNodes (ostream & stream, fstream * fio) const
/*******************************************************************************
% Routine Name : Write_AllTNodes (public)
% File :         Tree.c
% 
% Description : This function displays the current TNode and its children to
%               the output stream referenced by the parameter.
%
% Parameters descriptions :
% 
% name               description
% ------------------ -----------------------------------------------------------
% stream             A reference to the output stream
% fio                a pointer to the fstream to read from
% <return>           A reference to the output stream
*******************************************************************************/
{

    if (!*fio)
        cerr << "fio is currupted in TNode's Write_AllTNodes\n";

    // recursively traverse left
	if (left) {
		TNode<Whatever> readLeftNode (left, fio);
		readLeftNode.Write_AllTNodes (stream, fio);
	}
	stream << *this;

    // recursively traverse right
	if (right) {
		TNode<Whatever> readRightNode (right, fio);
		readRightNode.Write_AllTNodes (stream, fio);
	}

	return stream;
}

