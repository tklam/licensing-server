#include "sqlitertestfileoperations.h"

#define TEST_FILENAME "test.db"

csqlitertestfileoperations::csqlitertestfileoperations(void)
{
}


csqlitertestfileoperations::~csqlitertestfileoperations(void)
{
}

void csqlitertestfileoperations::setbasebath(std::string &basepath) {
	testpath = basepath;

	fullpath = testpath;
	fullpath.append(TEST_FILENAME);

}

int csqlitertestfileoperations::testdbcreation() {
csqliter db;

	//prep step... the results aren't important, just cleaning up if there was already a file there
	//from a previous test
	std::remove(fullpath.c_str());

	//the actual test- return values matter here
	if (db.createdb(fullpath) != successdb) {
		return(1);
	}

	//we checked the call itself, but put some framework in place for the next test call (testdbopen) to verify that
	//	it actually created a real database handle that worked.

	//create a sample table.  we'll use it when we test opening a db (testdbopen)
	db.setsql("CREATE TABLE pt ("
		"pt_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"x INTEGER, "
		"y INTEGER);");
	if (db.runsinglestepstatement() != successdb) {
		return(2);
	}

	db.setsql("INSERT INTO pt (x, y) "
		"VALUES ("
		"11, 12);");
	if (db.runsinglestepstatement() != successdb) {
		return(3);
	}

	db.closedb();

	return(0);

}

int csqlitertestfileoperations::testdbopen() {
csqliter db;

	if (db.openexistingdb(fullpath) != successdb) {
		return(1);
	}
	
	//so it claims to have opened one... let's see if it's actually a valid db by running a basic query
	//	on the data we think we put in during the testdbcreate test
	db.setsql("SELECT o.x from pt o;");
	db.pushvaltypeout(intdbval);
	if (db.runstep() != rowresultdb) {
		return(2);
	}
	if (db.rowdata.size() != 1) {
		return(3);
	}
	if (db.rowdata[0].valtype != intdbval) {
		return(4);
	}
	if (db.rowdata[0].ival != 11) {
		return(5);
	}

	//looks like we opened a database and managed to retrieve data from it.
	//must have been a valid open
	db.closedb();
	return(0);

}

int csqlitertestfileoperations::testdbremove() {
//the remove call is a thin wrapper around a system call to delete a file.
//so we're not trying to test the system call, only make sure that the logic
//of the csqliter class works properly
csqliter db;
csqliter db2;

	//should be callsed after the create and open tests, so we know the file is there when this is called

	//two test cases: one when the db already exists and one when it doesn't.
	// the first should delete successfully.  the second should fail
	//we're not trying to do exhaustive testing like what std::remove does 
	//  when someone else has the file open, or when permissions prevent it, etc.
//int rval;
	//rval = std::remove("c:\\delme\\test.db");

	if (db.removedatabase(fullpath) != successdb) {
		return(1);
	}

	//ok, now it should be gone.

	//shouldn't succeed again though, and try it with both a clean and a dirty csqliter db to be sure
	if (db.removedatabase(fullpath) == successdb) {
		return(2);
	}

	if (db2.removedatabase(fullpath) == successdb) {
		return(3);
	}
	

	return(0);

}

