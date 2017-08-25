#include <iostream>

#include "authorization.h"

bool Authorization::checkLoginRequest(const LoginRequest & req) {
    std::string licenseType = db.getUserLicenseType(req.clientID);
    std::cout << "License type: " << licenseType << std:: endl;

    if (licenseType == GROUP_LICENSE) {
        const std::vector<std::string> validIPs = db.getUserValidIPs(req.clientID);
        bool isValid = false;
        for (std::vector<std::string>::const_iterator it=validIPs.cbegin(); it!=validIPs.cend(); ++it) {
            if (req.ip == *it) {
                isValid = true;
                break;
            }
        }

        if (isValid) {
            return true;
        }
    }
    else if (licenseType == INDIVIDUAL_LICENSE) {
        std::cerr << "Individual license is mot supported yet!" << std::endl;
        throw std::exception();
    }
    else {
        return false;
    }

    return true;
}
