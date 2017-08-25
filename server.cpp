#include <queue>
#include <thread>
#include <atomic>

#include "zeromq/zhelpers.hpp"
#include "config.h"
#include "client.h"
#include "authorization.h"
#include "crypto.h"

int main(int argc, char** argv) {
    // server sockets
    zmq::context_t context(1);
    zmq::socket_t server(context, ZMQ_ROUTER);
    server.bind(LICENSING_SERVER_ADDRESS);

    // queue storing pending requests
    std::atomic<bool> isAddingQueue(false);
    std::queue<LoginRequest> clientIdentityQueue;

    Authorization auth;

    Crypto crypto;

    // check and authorize
    std::thread processRequest_t(
        [&]{
            while (true) {
                while (isAddingQueue.load(std::memory_order_relaxed) == false) {continue;}
                if (clientIdentityQueue.empty()) {
                    continue;
                }
                const LoginRequest req = clientIdentityQueue.front();
                clientIdentityQueue.pop();
                isAddingQueue.store(false, std::memory_order_relaxed);  

                bool isValid = auth.checkLoginRequest(req);

                //send Authorization result to client

                std::string result;
                if (isValid) {
                    result = crypto.encrypt(SUCCESSFUL_LOGIN_REPLY);
                }
                else {
                    result = crypto.encrypt(FAILED_LOGIN_REPLY);
                }

                s_sendmore(server, req.clientIdentity);
                s_send(server, result);
            }
        }
    );

    // receive requests
    std::thread waitRequest_t(
        [&]{
            while (true) {
                //  Process all parts of the message
                std::string clientIdentity = s_recv(server);

                // 1. recv the greeting message
                // 2. recv client id
                std::string workload = s_recv(server); 
                if (workload != GREETING) {
                    continue;
                }

                //key exchange
                //1. receive publics keys from another party
                const std::string spub = s_recv(server);
                const std::string epub = s_recv(server);
                //2. generate XPORT (encrypted CEK + digest)
                crypto.initKeyExchangeParams();
                crypto.genXport(spub, epub);
                //3. send public keys to another party                
                s_sendmore(server, clientIdentity);
                s_sendmore(server, crypto.spubHexStr()); //signing public key
                s_sendmore(server, crypto.epubHexStr()); //ephemeral(temp) public key
                //4. send XPORT to another party
                s_send(server, crypto.xportHexStr());

                //5. receive client ID
                s_recv(server); //dummy identity (we got this because we're using ZMQ_ROUTER)
                zmq::message_t message;
                server.recv(&message);
                std::string clientID = std::string(static_cast<char*>(message.data()), message.size());
                
                // The communication channel can now be encrypted with CEK

                std::string ip = message.gets("Peer-Address");

                while (isAddingQueue.load(std::memory_order_relaxed) == true) {}
                clientIdentityQueue.push(LoginRequest(clientIdentity, clientID, ip));
                isAddingQueue.store(true, std::memory_order_relaxed);  

                std::cout << "[Licensing server] Receive authorization request: \"" << workload
                              << "\" zmq identity: "
                              << clientIdentity
                              << " (ip: " << ip << ")" 
                              << " with clientID: "
                              << clientID << std::endl;       
            }
        }
    );

    processRequest_t.join();
    waitRequest_t.join();

    return 0;
}
