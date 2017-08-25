// SQLiter2Test.cpp : Defines the entry point for the console application.
//

#include <iostream>

#include "../src/sqliter.h"

#include "sqlitertestfileoperations.h"
#include "sqlitertestdataaccess.h"
#include "sqlitertestbind.h"

#define TEST_PATH "./"

//int _tmain(int argc, _TCHAR* argv[])
int main()
{
	csqliter db;
	dbresulttype r;
	bool bval;

	if (db.runinternaltests()) {
		return(-1);
	}

	csqlitertestfileoperations testfileops;
	std::string path;
	int rval;
	
	path = TEST_PATH;
	testfileops.setbasebath(path);

	rval = testfileops.testdbcreation();
	if (rval) {
		return(1);
	}

	rval = testfileops.testdbopen();
	if (rval) {
		return(2);
	}

	rval = testfileops.testdbremove();
	if (rval) {
		return(3);
	}

	csqlitertestdataaccess testaccess;
	
	path = TEST_PATH;
	testaccess.setbasebath(path);

	rval = testaccess.testtypesreturned();
	if (rval) {
		return(4);
	}

	rval = testaccess.testvaluesasnull();
	if (rval) {
		return(5);
	}

	rval = testaccess.testresponsenumargserror();
	if (rval) {
		return(6);
	}

	rval = testaccess.testresponseargtypeserror();
	if (rval) {
		return(7);
	}

	csqlitertestbind testbind;
	path = TEST_PATH;
	rval = testbind.testeachtypeofbind();
	if (rval) {
		return(8);
	}

	std::cout << "tests successful\r\n";

	return(0);

}

