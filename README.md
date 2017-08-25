# Licensing Server

A basic licensing server.

## Mechanism
A client ID is hardcoded in the client library. The client application, which
is to be built using the client library, will send the client ID to the server
when it is invoked. The server will validate the authorization request based on
the type of the license the client has, the client's IP, and/or other factors.

The communication between the client and the server is secured by Diffie-Hellman
key exchange and AES encryption.

Please feel free to modify it to suit your needs. You may want to improve the
implementation in the following ways:

+ Use passwords and/or public/private keys
+ Use encrypted resources in the client application


## Requirements

The following libraries are required:

+ 0mq
+ sqlite3
+ Crypto++

## Usage

### Database and user account management
1. Create the database using
   ```
   scripts/create-db.sh
   ```
2. Insert user accounts. Please refer to:
   ```
   scripts/insert-testing-data.sh
   ```

### Starting the server
   ```
   ./licensingServer
   ```

### Building new client library
Every client application should be given a unique client ID. That means the
client library has to be built uniquely for each client. The following line in
<i>CMakeLists.txt</i> should be modified to set the client ID:
 <pre>
 -DCLIENT\_ID="TESTING\_CLIENT"
 </pre>

## Donation
If you find this useful I'd appreciate a donation. My XMR address:
<p>
<pre>
4JUdGzvrMFDWrUUwY3toJATSeNwjn54LkCnKBPRzDuhzi5vSepHfUckJNxRL2gjkNrSqtCoRUrEDAgRwsQvVCjZbRwGcc71zBonCK7M2cd
</pre>
