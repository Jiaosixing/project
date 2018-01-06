/****************************************************************************

                                                        Sixing Jiao
                                                        CSE 12, Winter 2016
                                                        Nov 21rd, 2016
                                                        cs12xhz
                                Assignment Eight

File Name:      Driver.c
Description:    This file includes UCSDStudents class.
		it includes the consturtor,getter,setter and other
		operator methods and main driver.
****************************************************************************/
#ifndef DRIVER_H
#define DRIVER_H

#include <string.h>
#include <iostream>
#include <cstdlib>
using namespace std;

class UCSDStudent {
        friend ostream & operator << (ostream &, const UCSDStudent &);
        char name[16];
        long studentnum;

public:

	UCSDStudent () :studentnum (0){
		memset(name, '\0', sizeof(name)); 
	}
	
 /*-------------------------------------------------------------------------- 
  Function Name:          constructor of UCSDStudent
  Purpose:                When called, this function will initialize a new 
			  UCSDStudent object
  Description:            This function initlizes all member variables to 
			  parameter value
  Input:                  name : name of student
			  val : number of student.
  Result:                 constructor has no return value.
--------------------------------------------------------------------------*/ 
       	UCSDStudent (char * nm, long val = 0) :studentnum (val) {
		memset(name, '\0', sizeof(name)); 
		strcpy(name,nm); 
	}


/*-------------------------------------------------------------------------- 
  Operator Name:          const char*
  Purpose:                When used, this operator will return name of the
			  user
  Description:            This function directly returns the name of user
  Input:                  none
  Result:                 the name of user
--------------------------------------------------------------------------*/ 
	operator const char * (void) const {
		return name;
	}
/*-------------------------------------------------------------------------- 
  Operator Name:          ==
  Purpose:                When used, this operator will determine if users is 
			  equal to the the base that user is comparing to
  Description:            This operator copmpares the letters of names
  Input:                  &base: the pointer of base that user wants to compare
  Result:                 true if they are same and false if they are not
--------------------------------------------------------------------------*/ 
	long operator == (const UCSDStudent & stu) const {
		return ! strcmp (name, stu.name);
	}

/*-------------------------------------------------------------------------- 
  Operator Name:          <
  Purpose:                When used, this operator will determine if users is 
			  less than the the base that user is comparing to
  Description:            This operator copmpares the first letter of names
  Input:                  &base: the pointer of base that user wants to compare
  Result:                 true if user is smaller and false if it isn't
--------------------------------------------------------------------------*/ 
	long operator < (const UCSDStudent  & stu) const {
		return (strcmp (name, stu.name) < 0) ? 1 : 0;
	}
};

#endif
