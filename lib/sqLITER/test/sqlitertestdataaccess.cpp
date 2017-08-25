#include "sqlitertestdataaccess.h"


#define TEST_FILENAME "test.db"
csqlitertestdataaccess::csqlitertestdataaccess(void)
{
}


csqlitertestdataaccess::~csqlitertestdataaccess(void)
{
}

void csqlitertestdataaccess::setbasebath(std::string &basepath) {
	testpath = basepath;

	fullpath = testpath;
	fullpath.append(TEST_FILENAME);
}

int csqlitertestdataaccess::testtypesreturned() {
//basic checks of return value types
//create a table with a column of each data type. Insert  row and make sure we can get it back.
csqliter db;
unsignedcharvector blob;

	//setup
	db.removedatabase(fullpath);

	//actual test
	if (db.createdb(fullpath) != successdb) {
		return(1);
	}

	db.setsql("CREATE TABLE testtypes ("
		"test_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"testint INTEGER, "
		"testfloat FLOAT, "
		"teststr TEXT, "
		"testblob BLOB);");

	if (db.runsinglestepstatement() != successdb) {
		return(2);
	}

	db.setsql("INSERT INTO testtypes ("
		"testint, testfloat, teststr, testblob) "
		"VALUES (:int, :float, :str, :blob);");

	blob.push_back(1);
	blob.push_back(2);
	blob.push_back(4);
	blob.push_back(5);

	db.bindint("int",5);
	db.bindfloat("float",3.14);
	db.bindstr("str", "Hello World!");
	db.bindblob("blob", blob);

	if (db.runsinglestepstatement() != successdb) {
		return(3);
	}

	//now see if we get back what we put in...
	db.setsql("SELECT o.testint, o.testfloat, o.teststr, o.testblob FROM testtypes o;");
	db.pushvaltypesout(intdbval, floatdbval, strdbval, blobdbval);

	if (db.runsinglestepstatement() != successdb) {
		return(4);
	}

	//and here's the meat of the test: see what we got back.
	if (db.rowdata[0].valtype != intdbval) {
		return(5);
	}
	if (db.rowdata[0].ival != 5) {
		return(6);
	}

	if (db.rowdata[1].valtype != floatdbval) {
		return(7);
	}
	if (db.rowdata[1].fval != 3.14) {
		return(8);
	}

	if (db.rowdata[2].valtype != strdbval) {
		return(9);
	}
	if (db.rowdata[2].sval != "Hello World!") {
		return(10);
	}

	blob.resize(0);
	if (db.rowdata[3].valtype != blobdbval) {
		return(11);
	}
	blob = db.rowdata[3].blob;
	if (blob.size() != 4) {
		return(12);
	}
	if (blob[0] != 1) {
		return(12);
	}
	if (blob[1] != 2) {
		return(12);
	}
	if (blob[2] != 4) {
		return(12);
	}
	if (blob[3] != 5) {
		return(12);
	}

	db.closedb();

	return(0);
}

int csqlitertestdataaccess::testvaluesasnull() {
csqliter db;
//prove that nulls can be set and retrieved from any nominal data type (int, float, string, blob)

	//cleanup
	db.removedatabase(fullpath);

	if (db.createdb(fullpath) != successdb) {
		return(1);
	}

	db.setsql("CREATE TABLE testdat ("
		"testdat_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"testint INTEGER, "
		"testfloat FLOAT, "
		"teststr TEXT, "
		"testblob BLOB);");

	if (db.runsinglestepstatement() != successdb) {
		return(2);
	}

	db.setsql("INSERT INTO testdat (testint, testfloat, teststr, testblob) VALUES ("
		":int, :float, :str, :blob);");
	db.bindnull("int");
	db.bindnull("float");
	db.bindnull("str");
	db.bindnull("blob");

	if (db.runsinglestepstatement() != successdb) {
		return(3);
	}

	db.setsql("SELECT o.testint, o.testfloat, o.teststr, o.testblob FROM testdat o;");
	db.pushvaltypesout(intdbval, floatdbval, strdbval, blobdbval);

	if (db.runsinglestepstatement() != successdb) {
		return(4);
	}

	if (db.rowdata.size() != 4) {
		return(5);
	}
	if (db.rowdata[0].valtype != nulldbval) {
		return(5);
	}
	if (db.rowdata[1].valtype != nulldbval) {
		return(5);
	}
	if (db.rowdata[2].valtype != nulldbval) {
		return(5);
	}
	if (db.rowdata[3].valtype != nulldbval) {
		return(5);
	}

	db.closedb();

	return(0);

}

int csqlitertestdataaccess::testresponsenumargserror() {
csqliter db;

	//clean up
	db.removedatabase(fullpath);

	//now run the test
	if (db.createdb(fullpath) != successdb) {
		return(1);
	}

	db.setsql("CREATE TABLE testdat ("
		"testdat_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"testint INTEGER, "
		"teststr TEXT);");

	if (db.runsinglestepstatement() != successdb) {
		return(2);
	}

	db.setsql("INSERT INTO testdat (testint, teststr) VALUES ("
		":int, :str);");
	db.bindint("int",3);
	db.bindstr("str","abcdefg");

	if (db.runsinglestepstatement() != successdb) {
		return(3);	
	}

	//this will return two values per row but we are telling it to expect only one
	db.setsql("SELECT o.testint, o.teststr FROM testdat o;");
	db.pushvaltypesout(intdbval);

	if (db.runsinglestepstatement() != resultcountmismatchdberror) {
		return(4);
	}

	//now ask it to expect too many
	db.setsql("SELECT o.testint, o.teststr FROM testdat o;");
	db.pushvaltypesout(intdbval, strdbval, intdbval);

	if (db.runsinglestepstatement() != resultcountmismatchdberror) {
		return(4);
	}

	//now make sure that just right actually works
	//now ask it to expect too many
	db.setsql("SELECT o.testint, o.teststr FROM testdat o;");
	db.pushvaltypesout(intdbval, strdbval);

	if (db.runsinglestepstatement() != successdb) {
		return(5);
	}

	if (db.rowdata.size() != 2) {
		return(5);
	}

	if (db.rowdata[0].valtype != intdbval) {
		return(5);
	}
	if (db.rowdata[0].ival != 3) {
		return(5);
	}

	if (db.rowdata[1].valtype != strdbval) {
		return(5);
	}
	if (db.rowdata[1].sval != "abcdefg") {
		return(5);
	}

	db.closedb();

	return(0);
}

int csqlitertestdataaccess::testresponseargtypeserror() {
csqliter db;

	//clean up
	db.removedatabase(fullpath);

	//now run the test
	if (db.createdb(fullpath) != successdb) {
		return(1);
	}

	db.setsql("CREATE TABLE testdat ("
		"testdat_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"testint INTEGER, "
		"teststr TEXT);");

	if (db.runsinglestepstatement() != successdb) {
		return(2);
	}

	db.setsql("INSERT INTO testdat (testint, teststr) VALUES ("
		":int, :str);");
	db.bindint("int",3);
	db.bindstr("str","abcdefg");

	if (db.runsinglestepstatement() != successdb) {
		return(3);	
	}

	db.setsql("SELECT o.testint, o.teststr FROM testdat o;");
	db.pushvaltypesout(intdbval, intdbval);

	if (db.runsinglestepstatement() != resulttypemismatchdberror) {
		return(4);
	}

	db.setsql("SELECT o.testint, o.teststr FROM testdat o;");
	db.pushvaltypesout(intdbval, strdbval);

	if (db.runsinglestepstatement() != successdb) {
		return(4);
	}

	db.closedb();

	return(0);
}

