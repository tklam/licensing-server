CREATE TABLE UserValidIP (
    clientID TEXT PRIMARY KEY, 
    ip TEXT
);

create index UserValidIP_ip_index on UserValidIP (ip);
