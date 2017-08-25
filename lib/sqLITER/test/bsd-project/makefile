test: ../sqlitertest.cpp sqliter.o sqlite3.o sqlitertestdataaccess.o sqlitertestfileoperations.o sqlitertestbind.o
	g++ -pthread ../sqlitertest.cpp sqliter.o sqlite3.o sqlitertestdataaccess.o sqlitertestfileoperations.o sqlitertestbind.o -o test

sqlitertestfileoperations.o: ../sqlitertestfileoperations.cpp ../sqlitertestfileoperations.h
	g++ -c ../sqlitertestfileoperations.cpp

sqlitertestdataaccess.o: sqliter.o ../sqlitertestdataaccess.h ../sqlitertestdataaccess.cpp
	g++ -c ../sqlitertestdataaccess.cpp

sqlitertestbind.o: sqliter.o ../sqlitertestbind.h ../sqlitertestbind.cpp
	g++ -c ../sqlitertestbind.cpp

sqliter.o: ../../src/sqliter.cpp ../../src/sqliter.h
	g++ -c ../../src/sqliter.cpp

sqlite3.o: ../../src/sqlite3.c ../../src/sqlite3.h ../../src/sqlite3ext.h
	gcc -c ../../src/sqlite3.c
