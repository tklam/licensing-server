#ifndef _AUTHORIZATION_H_
#define _AUTHORIZATION_H_

#include "client.h"
#include "database.h"

struct Authorization {
    // returns: true if successful, false otherwise
    bool checkLoginRequest(const LoginRequest & loginRequest);

private:
    // database
    Database db;

};
#endif
