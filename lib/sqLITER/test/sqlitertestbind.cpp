#include "sqlitertestbind.h"

#define TEST_FILENAME "test.db"

csqlitertestbind::csqlitertestbind()
{
}


csqlitertestbind::~csqlitertestbind()
{
}

void csqlitertestbind::setbasebath(std::string &basepath) {
	testpath = basepath;

	fullpath = testpath;
	fullpath.append(TEST_FILENAME);
}

int csqlitertestbind::testeachtypeofbind() {
	//tests each type of data that can be bound
	//	by either string name or const char * name
	//note that to be thorough, we're testing NULL binding to each supported table data type
	csqliter db;
	unsignedcharvector blob;
	std::string fullpath;
	std::string name;

	fullpath = "./testbind.db";

	//setup
	db.removedatabase(fullpath);

	//actual test
	if (db.createdb(fullpath) != successdb) {
		return(1);
	}

	db.setsql("CREATE TABLE testbindtypes ("
		"test_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"testint1 INTEGER, "
		"testint2 INTEGER, "
		"testfloat1 FLOAT, "
		"testfloat2 FLOAT, "
		"teststr1 TEXT, "
		"teststr2 TEXT, "
		"testblob1 BLOB, "
		"testblob2 BLOB);");

	if (db.runsinglestepstatement() != successdb) {
		return(2);
	}

	//now for the binding.  "real" values in the first row, null in the second

	//first row insert 

	db.setsql("INSERT INTO testbindtypes ("
		"testint1, testint2, testfloat1, testfloat2, teststr1, teststr2, testblob1, testblob2) "
		"VALUES (:int1, :int2, :float1, :float2, :str1, :str2, :blob1, :blob2);");

	db.bindint("int1", 1);
	name = "int2";
	db.bindint(name, 2);

	db.bindfloat("float1", 3.14);
	name = "float2";
	db.bindfloat(name, 4.14);

	db.bindstr("str1", "abc");
	name = "str2";
	db.bindstr(name, "def");

	blob.resize(0);
	blob.push_back(1);
	blob.push_back(2);
	blob.push_back(3);
	db.bindblob("blob1", blob);
	blob.resize(0);
	blob.push_back(3);
	blob.push_back(4);
	blob.push_back(5);
	blob.push_back(6);
	name = "blob2";
	db.bindblob(name, blob);

	if (db.runsinglestepstatement() != successdb) {
		return(3);
	}

	//second row insert, NULL's for everything
	db.setsql("INSERT INTO testbindtypes ("
		"testint1, testint2, testfloat1, testfloat2, teststr1, teststr2, testblob1, testblob2) "
		"VALUES (:int1, :int2, :float1, :float2, :str1, :str2, :blob1, :blob2);");

	db.bindnull("int1");
	name = "int2";
	db.bindnull(name);

	db.bindnull("float1");
	name = "float2";
	db.bindnull(name);

	db.bindnull("str1");
	name = "str2";
	db.bindnull(name);

	db.bindnull("blob1");
	name = "blob2";
	db.bindnull(name);

	if (db.runsinglestepstatement() != successdb) {
		return(3);
	}

	//now, retrieve our values and see if they match
	db.setsql("SELECT testint1, testint2, testfloat1, testfloat2, teststr1, teststr2, testblob1, testblob2 FROM testbindtypes;");
	db.pushvaltypesout(intdbval, intdbval);
	db.pushvaltypesout(floatdbval, floatdbval);
	db.pushvaltypesout(strdbval, strdbval);
	db.pushvaltypesout(blobdbval, blobdbval);

	if (db.runstep() != rowresultdb) {
		return(4);
	}

	//types were checked implicitly
	if (db.rowdata[0].ival != 1) {
		return(5);
	}
	if (db.rowdata[1].ival != 2) {
		return(6);
	}

	//note: ok, it's a float.  But we're testing on a single platform in a single session...
	// if this doesn't work on some platforms 
	if (db.rowdata[2].fval != 3.14) {
		return(7);
	}

	if (db.rowdata[3].fval != 4.14) {
		return(8);
	}

	if (db.rowdata[4].sval != "abc") {
		return(9);
	}

	if (db.rowdata[5].sval != "def") {
		return(10);
	}

	if (db.rowdata[6].blob.size() != 3) {
		return(11);
	}
	if (db.rowdata[6].blob[0] != 1) {
		return(12);
	}
	if (db.rowdata[6].blob[1] != 2) {
		return(13);
	}
	if (db.rowdata[6].blob[2] != 3) {
		return(14);
	}

	if (db.rowdata[7].blob.size() != 4) {
		return(15);
	}
	if (db.rowdata[7].blob[0] != 3) {
		return(16);
	}
	if (db.rowdata[7].blob[1] != 4) {
		return(17);
	}
	if (db.rowdata[7].blob[2] != 5) {
		return(18);
	}
	if (db.rowdata[7].blob[3] != 6) {
		return(19);
	}

	//first row was ok, now second row
	if (db.runstep() != rowresultdb) {
		return(18);
	}
	
	if (db.rowdata[0].valtype != nulldbval) {
		return(19);
	}

	if (db.rowdata[1].valtype != nulldbval) {
		return(19);
	}

	if (db.rowdata[2].valtype != nulldbval) {
		return(19);
	}

	if (db.rowdata[3].valtype != nulldbval) {
		return(19);
	}

	if (db.rowdata[4].valtype != nulldbval) {
		return(19);
	}

	if (db.rowdata[5].valtype != nulldbval) {
		return(19);
	}

	if (db.rowdata[6].valtype != nulldbval) {
		return(19);
	}

	if (db.rowdata[7].valtype != nulldbval) {
		return(19);
	}

	if (db.runstep() != successdb) {
		return(20);
	}

	db.closedb();

	return(0);

}
