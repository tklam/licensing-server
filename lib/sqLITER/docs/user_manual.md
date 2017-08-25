# sqLITER v0.6 Documentation

## Summary

This documentation consists of:

* Installation of sqLITER
* Overview of Objects and Calls
* Overview of Typical Use
* Details of API
* Example Use of API
* Feature Requests and Bug Reports

## Installation of sqLITER
 
1.	Download one of the precompiled binaries of SQLite for your machine, or compile and install SQLite from source.  http://www.sqlite.org
2.	Download the sqLiter files from http://www.sqliter.org.
3.	Unzip and install the sqLITER files in your project.
4.	Include the sqLiter header to your C++ project using the following code: #include "sqliter/src/sqliter.h"
5.	All other required files will be called by the sqLITER wrapper.

## Overview of Objects and Calls

A user of the library will use the principal class CSQLiter.  Several other helper classes are provided by that object for setting options and storing data, but none of them need be created by the library user.  A CSQLiter class instance represents a connection to a database file and it is possible to have multiple connections in the same application or within several different applications.

## Overview of Typical Use

A typical database usage will begin by opening (or creating) a database, running commands written in SQL against that database, and closing the database connection afterwards.  For each statement, results other than an error code may or may not be provided.

### Opening a Database

You can open or create a new database by invoking **opendb()** or **createdb()** on a CSQLiter instance.  

### Running SQL Statements

Once you have an open database connection: 

1. Set the SQL command to be executed, such as "SELECT first_name, last_name FROM client;"
2. Bind any required input parameters
3. Specify the number, type, and order of expected result data rows.  An example would be to expect two TEXT values for the above SQL statement.
4. Executing the statement involves either the runstep or runsinglestepstatement functions depending on whether 0, 1, or a larger number of rows could be a correct result data set of executing the statement.  The cases of 0 and 1 are provided for convenience in the case of statements known to yield no results (such as INSERT) or to handle cases such as a SELECT by unique id in which anything other than 0 or 1 result rows constitutes an error. 
5. Results, if any, from executing the statement are available immediately after executing a step of the statement.  A result row is a vector of cells which identify the type and value of data being returned.

### Closing the Database

The database is closed by invoking the **closedb()** function.

## Details of API 

The header file sqliter.h should be consulted for full details of calling parameters.  This documentation describes families of similar calls.

### Database Access and Management 

**openexistingdb( string "path/to/database/file" )** accepts the path of a database file; The return result reflects whether or not it was opened successfully.  Note that opening a database file does not guarantee that write access has been obtained.

**createdb( string "path/to/database/file" )** will create a new database.  The call will fail if a database already exists at that path.

**removedatabase( string "path/to/database/file" )** will delete the specified database.

**getdbhandle()** returns the raw database handle

**setdbhandle( dbhandle "raw_database_handle")** sets a raw database handle

**closedb()** ends a database session.  There is no return value.

### Data and Transactions

**setsql( string "sql statement" )** assigns the SQL language statement(s) to execute, such as "SELECT first_name from client;"

**bindstr()**, **bindint()**, **bindfloat()**, **bindblob()**, **bindnull()** assigns values to a SQL statement, such as :lastname in "SELECT first_name from client o WHERE o.last_name = :lastname;"

**pushvaltype()**, **pushvaltypesout()** tells the SQLite database what type of data to expect and return (number, order, and type) in each row of SELECT results.

**runstep()** executes a step in a statement which can have an arbitrary number of result rows, including the zero or one cases covered by runsinglestepstatement().

**runsinglestepstatement()** executes a statement in which the user believes any number of rows returned more than either zero or one (user's choice for each call) is an error.  Examples are INSERT's (doesn't return a row at all) and SELECTs by an id which is supposed to be unique and which therefore shouldn't return more than one row.

**closestatement()** tells sqLITER to halt running of the existing statement, but not to entirely clear it.  The SQL statement itself is retained, which saves time if the user wants to run a series of statements which share an identical setsql() call but which differ in terms of input values bound.  Another use is to release a lock when the application has received some row results but does not intend to retrieve the rest.

### Testing, Logging, and Debug

**startlog()**, **stoplog()** starts and stops more detailed logging of calls and results.

## Example Use of API

The following example C++ code creates a database of 2D points, adds the points (1,2) and (3,5) and then retrieves and displays those points.  Note that running the demonstration code twice without deleting the database file will fail because the database will already exist, causing the createdb call to fail.

```
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
```

## Feature Requests and Bug Reports

Please submit all feature requests and bug reports to nbossett@pierb.com.