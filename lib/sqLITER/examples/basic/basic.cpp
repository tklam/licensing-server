
#include <string>
#include <iostream>

#include "../../src/sqliter.h"

int main() {
csqliter db;

std::string str1;
int tmpint;
dbresulttype rval;
int i;

	//create a new database
	rval = db.createdb("test.db");
	if (rval != successdb) {
		std::cout << "Failed to create database.  Perhaps it already exists from a previous run?\r\n";
		return(1);
	}
	
	//create a table within the database consisting of 2D integer points
	db.setsql("CREATE TABLE point2d "
		"(point2d_id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"x INTEGER, "
		"y INTEGER);");
	rval = db.runsinglestepstatement();
	if (rval != successdb) {
		return(2);
	}

	//put the point (1,2) into the table
	db.setsql("INSERT INTO point2d "
		"(x, y) VALUES "
		"(:x, :y);");
	db.bindint("x",1);
	db.bindint("x",2);
	rval = db.runsinglestepstatement();
	if (rval != successdb) {
		return(3);
	}

	//put the point (3,5) into the database
	db.setsql("INSERT INTO point2d "
		"(x, y) VALUES "
		"(:x, :y);");
	db.bindint("x",3);
	db.bindint("y",5);
	
	rval = db.runsinglestepstatement();
	if (rval != successdb) {
		return(3);
	}

	//retrieve and display all of the points in the table	
	db.setsql("SELECT x, y FROM point2d;");
	db.pushvaltypesout(intdbval, intdbval); //tell SQLiter to expect each row of results to consist of two integers
	while (db.runstep() == rowresultdb) {
		std::cout << "point:"
		<< db.rowdata[0].ival << "\t"
		<< db.rowdata[1].ival << "\n";
	}

	//close the database
	db.closedb();

	std::cout << "basic example run successfully\r\n";
}
