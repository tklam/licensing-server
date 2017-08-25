#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <string>

#define GROUP_LICENSE std::string("Group")
#define INDIVIDUAL_LICENSE std::string("Individual")

#define SUCCESSFUL_LOGIN_REPLY std::string("ACK")
#define FAILED_LOGIN_REPLY std::string("BYE")

struct LoginRequest{
    std::string clientIdentity;
    std::string clientID;
    std::string ip;

    LoginRequest(const std::string & clientIdentity_, 
                 const std::string & clientId_, 
                 const std::string & ip_);
};

struct Client {
    Client();
    ~Client();
    bool connectAndGetAuthToken();

private:
    std::string _id;
};
#endif
