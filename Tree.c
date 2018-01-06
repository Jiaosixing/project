/* Name: Jinsong Cao ---cs12xgj
 *    && Sixing Jiao---cs12xhz
 * Date: 2,12,2016
 * 
 *
 * file header: this file implement a binary tree with
 * similar algorithm with hw8, and also, the tree will be
 * implemented on dist.
 * */

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
int Tree<Whatever>::debug_on = 0;

template <class Whatever>
long Tree<Whatever>::cost = 0;

template <class Whatever>
long Tree<Whatever>::operation = 0;


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define THRESHOLD 2

template <class Whatever>
ostream & operator << (ostream &, const TNode<Whatever> &);

template <class Whatever>
struct  TNode {
// friends:

// data fields:
	Whatever data;
	long height;
	long balance;
	offset left;
	offset right;
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
	void Read (const offset &, fstream *);	// read node from disk
	unsigned long Remove (TNode<Whatever> &, fstream *, long &, offset &,
		long fromSHB = FALSE);
	void ReplaceAndRemoveMin (TNode<Whatever> &, fstream *, offset &);
	void SetHeightAndBalance (fstream *, offset &);
	void Write (fstream *) const;		// update node to disk

	ostream & Write_AllTNodes (ostream &, fstream *) const;
};

template <class Whatever>
unsigned long Tree<Whatever> :: Insert (Whatever & element) {
	/*if not inserting the root node*/
	if (occupancy != 0){
		TNode<Whatever> readRootNode (root, fio);
		readRootNode.Insert (element, fio, occupancy, root); 
	}
	else {
		/*else inserting at root,just replace root with a new TNode
		 *to hold the data*/
		TNode<Whatever> rootNode (element,fio,occupancy);
		root = rootNode.this_position;	
	}
	Tree <Whatever> :: IncrementOperation ();
	return TRUE;
}

/* Name: ReplaceAndRemoveMin
 * purpose: deal with the situation with 2 children during remove
 * description: replace the tnode with the largest child in the 
 *              right tree
 * parameter: TNode<Whateer> & targetTNode, fstream *fio
 *            offset&PositionInParent
 * result:    return void
 * */
template <class Whatever>
void TNode<Whatever> :: ReplaceAndRemoveMin (TNode<Whatever> & targetTNode, 
	fstream * fio, offset & PositionInParent) {
		/*The successor node is the one along the line without left 
		 * child*/
		if (left == 0){
			/*change the targetTNode's data with that of its 
			 * successor*/
			targetTNode.data = this->data;
			/*update the pointer in parent*/
			PositionInParent = right;
			/*delete the successor after replacement*/
		}
		/*else we have not found the successor yet*/
		else{
			/*recursively calls itself with its left child until 
			 * successor is found*/
			TNode<Whatever> leftNode (left, fio);
			leftNode.ReplaceAndRemoveMin(targetTNode,fio,left);
			/*reset Height and balance of nodes along the path in 
			 *the end*/
			SetHeightAndBalance(fio,PositionInParent);
		}
}

/* Name: Remove
 * purpose: remove a tnode from the tree
 * description: remove the tnode from the tree
 * parameter: TNode<Whatever> & elementTNode
 *            fstream * fio
 *            long & occupancy
 *            offset & positionInParent
 *            long from SHB
 * result:    remove the tnode and return a long
 * */
template <class Whatever>
unsigned long TNode<Whatever> :: Remove (TNode<Whatever> & elementTNode,
	fstream * fio, long & occupancy, offset & PositionInParent,
	long fromSHB) {
		long result = 0;
		/*keep searching left if data of parameter TNode is smaller than
		 *that of the calling TNode*/
		if(elementTNode.data < data){
			/*remove failed if reached left end*/
			if (left == 0){
				result = FALSE;
				return FALSE;
			}
			/*searching down by recursively calling itself with left
			 *child */
			TNode<Whatever> leftNode (right, fio);
			result = leftNode.Remove
			(elementTNode,fio,occupancy,left,0);
		}

		/*keep searching right if data of parameter TNode is smaller 
		 * than that of the calling TNode*/
		else if(data < elementTNode.data){
			/*remove failed if reached right end*/
			if (right == 0){
				result = FALSE;
				return FALSE;
			}
			/*searching down by recursively calling itself with 
			 * right child */
			TNode<Whatever> rightNode (right, fio);
			result = rightNode.Remove
			(elementTNode,fio,occupancy,right,0);
		}

		/*When the data in parameter TNode is found*/
		else if(data == elementTNode.data){
			/*if the TNode we want to delete doesn't have a child*/
			if (left == 0 && right == 0){
				PositionInParent = 0;
				elementTNode.data = data;
				/*calls setheightandbalance if the function call
				 *is not from SHB itself*/
				result = TRUE;
			}
			
			/*if the TNode we want to delete have two children*/
			else if (left != 0 && right != 0){
				/*reassign elementTNode's data to data for 
				 *display*/
				elementTNode.data = data;
				/*uses its right child to call 
				 * replaceandremovemin*/
				TNode<Whatever> rightNodeRARM (right, fio);
				rightNodeRARM.ReplaceAndRemoveMin
				(*this,fio,right);
				result = TRUE;
			}
			
			/*else the TNode we want to delete has one child*/
			else {
				/*find which side the child is on and assign 
				 *pip*/
				if(left != 0){
					PositionInParent = left;	
				}	
				else{
					PositionInParent = right;	
				}
				/*reassign elementTNode's data to data for 
				 *display*/
				elementTNode.data = data;
				/*detele the current node*/
				result = TRUE;
			}
				if (fromSHB == FALSE && result == TRUE){
					occupancy --;	
				}	
		}
		/*calls setheightandbalance if the function call
		*is not from SHB itself*/
		if (fromSHB == FALSE){
			SetHeightAndBalance(fio,PositionInParent);
		}
		Write(fio);
		return result;
}

/* Name: Tree::remove
 * purpose: remove the tree
 * parameter: whatever & element
 * return: unsighed long
 * */
template <class Whatever>
unsigned long Tree<Whatever> :: Remove (Whatever & element) {
		/*a long to indicate if remove is successful*/
	unsigned long result = 0;
	/*makes a tnode to hold the parameter element*/
	TNode<Whatever> targetTNode = TNode<Whatever>(element);
	/*call the TNode's remove if it is not the root node*/
	if (occupancy == 0){
		return FALSE;
	}
	/*else just remove the root node*/
	else{
		TNode<Whatever> rootNode (root,fio);
		result = rootNode.Remove(targetTNode,fio,occupancy,root,0);
	}
		element = targetTNode.data;
		Tree <Whatever> :: IncrementOperation (); 
		if (occupancy == 0){
			Tree <Whatever> :: ResetRoot ();
		}
		return result;
	}

/***************************************************************************
  % method Name    Set_Debug_On (void)
  % File :         Tree.c
  % 
  % Description :  This function sets debug mode on 
  %
  % Parameters descriptions :
  % 
  % name               description
  % ------------------ ------------------------------------------------------
  % none
  %***************************************************************************/
template <class Whatever>
void Tree<Whatever>:: Set_Debug_On (void) {
	Tree<Whatever>::debug_on = TRUE;
}

/***************************************************************************
  % method Name    Set_Debug_Off (void)
  % File :         Tree.c
  % 
  % Description :  This function sets debug mode off
  %
  % Parameters descriptions :
  % 
  % name               description
  % ------------------ ------------------------------------------------------
  % none
  %***************************************************************************/
template <class Whatever>
void Tree<Whatever>:: Set_Debug_Off (void) {
	Tree<Whatever>::debug_on = FALSE;
}

/* Name: setHeightAndBalance
 * parameter: fio, positionInparameter
 * purpose: set height and balance of tnode
 * return void
 * */

template <class Whatever>
void TNode<Whatever> :: SetHeightAndBalance (fstream * fio,
	offset & PositionInParent) {
		long fakeOccupancy = 0;
		/*height of left child*/
		long leftheight = 0;
		/*height of right child*/
		long rightheight = 0;
		/*balance of left side*/
		long leftbalance = 0;
		/*balance of right side*/
		long rightbalance = 0; 
		/*creates a copy to hold the data of TNode we want to 
		 *re insert if threshold is met*/
		/*when the current node does not have a left node*/
		if (left == 0){
			/*set height of left to 0 and balance to -1*/
			leftheight = -1;
			leftbalance = -1;
		}
		/*when the current node does have a left node*/
		else{
			/*set height and balance of left same to those 
			 * of its left child*/
		
		TNode <Whatever> left = TNode<Whatever>(this->left,fio);
			leftheight = left.height;
			leftbalance = left.height;
		}

		/*when the current node does not have a right node*/
		if (right == 0){
			/*set height of right to 0 and balance to -1*/
			rightheight = -1;
			rightbalance = -1;
		}
		/*when the current node does have a right node*/
		else{
			/*set height and balance of right same to those 
			 * of its right child*/
			TNode <Whatever> right = 
			TNode<Whatever>(this->right,fio);
			rightheight = right.height;	
			rightbalance = right.height;
		}

		/*height of current node is 1 plus the height of its 
		 * tallest child, so compare height of two sides first
		 * and set current height as the higher one plus one*/
		if(leftheight > rightheight){
			height = leftheight +1;	
		}
		else{
			height = rightheight +1;	
		}
		/*balance of current node is the left balaance - 
		 * right balance*/
		balance = leftbalance - rightbalance;
		/*When the threshold is met*/
		Write(fio);
		if (abs(balance) > THRESHOLD){
			TNode<Whatever> deleteNode(data);
			/*remove the current node*/
			Remove
			(deleteNode,fio,fakeOccupancy,PositionInParent,TRUE);
			/*reinsert current node into the tree*/
			TNode<Whatever> replaceNode (PositionInParent,fio);
			replaceNode.Insert
			(deleteNode.data,fio,fakeOccupancy,PositionInParent);
		}  
}

/* Name: getCost
 * parameter: no
 * purpose: getCost
 * return cost
 * */
template <class Whatever>
long Tree <Whatever> :: GetCost () {
	return cost;
}

/* Name: getOperation
 * parameter: no
 * purpse: get operation
 * return: operation
 * */
template <class Whatever>
long Tree <Whatever> :: GetOperation () {
	return operation;
}

/* Name: incrementCost
 * parameter: no
 * purpose: increase the cost
 * return void
 * */
template <class Whatever>
void Tree <Whatever> :: IncrementCost () {
	cost++;
}

/* Name: incrementOperation
 * paramter: no
 * purpose: increase the cost
 * return no return
 * */
template <class Whatever>
void Tree <Whatever> :: IncrementOperation () {
	operation++;	
}

/* Name: resetRoot
 * purpose: set the root to the bottom
 * parameter: no
 * return void
 *
 * */
template <class Whatever>
void Tree <Whatever> :: ResetRoot () {
        root = fio->seekp(0,ios::end).tellp();
}


/* Name: Tnode insert
 * purpose:insert tnode
 * parameter: whatever&element
 *            fstream * fio
 *            long occupancy
 *            offset & positionInparent
 * return long
 * */
template <class Whatever>
unsigned long TNode<Whatever> :: Insert (Whatever & element, fstream * fio,
	long & occupancy, offset & PositionInParent) {
	/*keep searching left if data of parameter TNode is smaller than
	 *that of the calling TNode*/
	if(element < data){
		/*reached the end if left is null*/
		if (left == 0){
			/*create new TNode on the left to hold the parameter 
			 *data*/
			TNode<Whatever> newNode(element,fio,occupancy);
			/*reset the height and balance*/
			this->left = newNode.this_position;
			SetHeightAndBalance(fio,PositionInParent);
			return TRUE;
		}
		/*if not down to the end yet, keep recursively calling itself 
		 *with left child*/
		TNode<Whatever> leftNode (this->left, fio);
		leftNode.Insert(element,fio,occupancy,left);
	}

	/*keep searching right if data of parameter TNode is bigger than
	 *that of the calling TNode*/
	else if(data < element){
		/*reached the end if right is null*/
		if (right == 0){
			/*create new TNode on the right to hold the parameter 
			 *data*/
			TNode<Whatever> newNode (element,fio,occupancy);
			/*reset the height and balance*/
			this->right = newNode.this_position;
			SetHeightAndBalance(fio,PositionInParent);
			return TRUE;
		}
		/*if not down to the end yet, keep recursively calling itself 
		 *with right child*/
		TNode<Whatever> rightNode (this->right, fio);
		rightNode.Insert(element,fio,occupancy,right);
	}

	/*if inserting the samething again*/
	else if(data == element){
		/*update the existing node*/
		data = element;
		return TRUE;
	}
	/*reset the height and balance*/
	SetHeightAndBalance(fio,PositionInParent);
	return TRUE;
}


/* Name: lookup
 * purpose: look up an element
 * parameter: whatever & element
 * return long
 * */
template <class Whatever>
unsigned long Tree<Whatever> :: Lookup (Whatever & element) const {
	if (occupancy == 0){
		return FALSE;	
	}
	/*A pointer to the current working node, which is 
	 * initially root*/
	TNode<Whatever> current (root,fio);
	/*when current is not null, which means we have not reached
	 * the bottom yet*/
	/*if item is found*/
	while (1){
		if(element == current.data){
			/*if hasBeenDeleted is false, returns the 
			 * pointer to the data*/
			element = current.data;
			return TRUE;
		}

		/*moves the current node down to the left if it is 
		 * greater than parameter element*/
		else if (element < current.data){
			if (current.left == 0){
				return FALSE;
			}
			current = TNode<Whatever>(current.left,fio);
		}

		/*moves the current node down to the right if it is 
		 * less than parameter element*/
		else if (current.data < element){
			if (current.right == 0){
				return FALSE;
			}
			current = TNode<Whatever>(current.right,fio);
		}
	}
	Tree <Whatever> :: IncrementOperation (); 
	return FALSE;    // not there
}

/* Name: read
 * purpose: read from dist
 * parameter: position fio
 * return: void
 * */
template <class Whatever>
void TNode<Whatever> :: Read (const offset & position, fstream * fio) {
	fio -> seekg(position,ios::beg);
	fio -> read((char *)this ,sizeof(*this));
	Tree <Whatever> :: IncrementCost ();

   if(Tree<Whatever>::debug_on)
      cerr<<COST_READ<<(const char*)data<<"]\n";
}

/*  Name: tnode constructor
 *  purpose: create a tnode
 *  paramater: position, fio
 *  return 
 * */
template <class Whatever>
TNode<Whatever> :: TNode (const offset & position, fstream * fio) {
	Read(position,fio);
}


/* Name: Tnode constructor
 * purpose: construct tnode 
 * parameter: element, occupancy fio
 * return 
 * */
template <class Whatever>
TNode<Whatever> :: TNode (Whatever & element, fstream * fio, long & occupancy): 
			data (element), height (0), balance (0), left (0), 
			right (0) {
   occupancy++;
   this_position = fio->seekp(0,ios::end).tellp();
   Write(fio);
}


/* Name: write
 * purpose: write to the file
 * parameter: fio
 * result:write on disk
 * */
template <class Whatever>
void TNode<Whatever> :: Write (fstream * fio) const {
	fio -> seekp (this_position,ios::beg);
	fio -> write((const char*)this,sizeof(*this));
	Tree <Whatever> :: IncrementCost ();

   if(Tree<Whatever>::debug_on)
      cerr<<COST_WRITE<<(const char*)data<<"]\n";
}

/* Name: tree
 * parameter: datafile
 * purpose: initialize data field
 * result: construct a tree
 * */
template <class Whatever>
Tree<Whatever> :: Tree (const char * datafile) :
	fio (new fstream (datafile, ios :: out | ios :: in)),
	root(0),occupancy(0),tree_count(0){
	fio->seekg(0,ios::beg);
	offset beginning = fio -> tellg();

	fio->seekg(0,ios::end);
	offset ending = fio -> tellg();

	/*file is empty*/
	if(beginning == ending){
		//reserve space
		fio -> seekp(0,ios::beg);
		fio -> write((const char*)&root,sizeof(root));
		fio -> write((const char*)&occupancy,sizeof(occupancy));
		root = fio -> tellp();
	}
   /*if not empty*/
	else{
		fio -> seekg(0,ios::beg);
		fio -> read((char *) &root,sizeof(root));
		fio -> read((char *) &occupancy,sizeof(occupancy));
	}
   /*debug message*/
   if(debug_on)
      cerr<<TREE<<tree_count<<ALLOCATE;
	tree_count ++;
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
	fio -> seekp(0,ios::beg);
	fio -> write((const char*)&root,sizeof(root));
	fio -> write((const char*)&occupancy,sizeof(occupancy));
	fio -> close();
	delete fio;
   if(debug_on)
      cerr<<TREE<<tree_count<<DEALLOCATE;
}	/* end: ~Tree */

template <class Whatever>
ostream & operator << (ostream & stream, const TNode<Whatever> & nnn) {
	stream << "at height:  :" << nnn.height << " with balance:  "
		<< nnn.balance << "  ";
	return stream << nnn.data << "\n";
}

template <class Whatever>
ostream & Tree<Whatever> :: Write (ostream & stream) const
/***************************************************************************
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
% ------------------ ------------------------------------------------------
% stream             A reference to the output stream.
% <return>           A reference to the output stream.
***************************************************************************/

{
        long old_cost = cost;

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
Write_AllTNodes (ostream & stream, fstream * fio) const {
	if (left) {
		TNode<Whatever> readLeftNode (left, fio);
		readLeftNode.Write_AllTNodes (stream, fio);
	}
	stream << *this;
	if (right) {
		TNode<Whatever> readRightNode (right, fio);
		readRightNode.Write_AllTNodes (stream, fio);
	}

	return stream;
}

