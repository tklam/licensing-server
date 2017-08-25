#include "config.h"
#include "client.h"
#include "zeromq/zhelpers.hpp"
#include "crypto.h"

LoginRequest::LoginRequest(const std::string & clientIdentity_, 
                 const std::string & clientID_, 
                 const std::string & ip_):
    clientIdentity(clientIdentity_),
    clientID(clientID_),
    ip(ip_)
{
}

Client::Client():
    _id(CLIENT_ID)
{
}

Client::~Client() {
}

bool Client::connectAndGetAuthToken() {
    Crypto crypto;
    crypto.initKeyExchangeParams();

    zmq::context_t context(1);
    zmq::socket_t client(context, ZMQ_DEALER);
    client.connect(LICENSING_SERVER_ADDRESS);

    s_sendmore (client, GREETING);
    
    //key exchange
    //1. send public keys to another party
    s_sendmore(client, crypto.spubHexStr()); //signing public key
    s_send(client, crypto.epubHexStr()); //ephemeral(temp) public key
    //2. receive publics keys from another party
    const std::string spub = s_recv(client);
    const std::string epub = s_recv(client);
    //3. generate XPORT (encrypted CEK + digest)
    crypto.genXport(spub, epub);
    //4. receiv XPORT from another party
    const std::string xport = s_recv(client);
    crypto.decryptXport(xport);

    //5. send client ID
    s_send(client, _id);

    // The communication channel can now be encrypted with CEK
    std::string ack = s_recv(client);
    client.disconnect(LICENSING_SERVER_ADDRESS);    

    ack = crypto.decrypt(ack);
    if (ack == SUCCESSFUL_LOGIN_REPLY) {
        return true;
    }
    else if (ack == FAILED_LOGIN_REPLY) {
        return false;
    }

    return false;
}
