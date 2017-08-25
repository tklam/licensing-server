basic: ../basic.cpp sqliter.o sqlite3.o
	g++ -pthread ../basic.cpp sqliter.o sqlite3.o -o basic

sqliter.o: ../../../src/sqliter.cpp ../../../src/sqliter.h
	g++ -c ../../src/sqliter.cpp

sqlite3.o: ../../../src/sqlite3.c ../../../src/sqlite3.h ../../../src/sqlite3ext.h
	gcc -c ../../../src/sqlite3.c
