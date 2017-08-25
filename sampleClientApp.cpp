#include "zeromq/zhelpers.hpp"
#include "client.h"

int main(int argc, char** argv) {
    Client client;
    bool isValid = client.connectAndGetAuthToken();

    if (isValid) {
        std::cout << "Brilliant! I am able to use the service!" << std::endl;
    }
    else { 
        std::cout << "Oh why?" << std::endl;
    }

    return 0;
}
