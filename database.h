#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "sqliter.h"
#include <string>
#include <vector>

struct Database {
    Database();
    ~Database();

    std::string getUserLicenseType(const std::string & clientID);
    std::vector<std::string> getUserValidIPs(const std::string & clientID);
private:
    csqliter db;
    dbresulttype dbfile;
};
#endif
