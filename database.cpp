#include "database.h"
#include <iostream>
#include <exception>

Database::Database() {
    std::cout << "Setting up database..." << std::endl;

    dbresulttype rval = db.openexistingdb("licensing.sqlite");

	if (rval != successdb) {
        std::cerr << "Failed to open database." << std::endl;
        std::cerr << "Please create one using the following command:" << std::endl;
        std::cerr << "./create-db.sh" << std::endl;
        return;
    }

    std::cout << "DONE" << std::endl;
}

Database::~Database() {
    db.closedb();    
}

std::string Database::getUserLicenseType(const std::string & clientID) {
    //TODO use stored procedure
    db.setsql("SELECT licenseType FROM User where clientID = :clientID \
            and (expiryDateTime-strftime('%s', 'now')) > 0;");
    db.bindstr("clientID", clientID.c_str());
	db.pushvaltypeout(strdbval); 
	if (db.runstep() == rowresultdb) {
        return db.rowdata[0].sval;
	}
    return "";
}

std::vector<std::string> Database::getUserValidIPs(const std::string & clientID) {
    //TODO use stored procedure
    db.setsql("SELECT ip FROM UserValidIP where clientID = :clientID;");
    db.bindstr("clientID", clientID.c_str());
	db.pushvaltypeout(strdbval); 

    std::vector<std::string> ips;

	while (db.runstep() == rowresultdb) {
        ips.push_back(db.rowdata[0].sval);
	}
    return ips;
}
