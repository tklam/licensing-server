#pragma once

#include "../src/sqliter.h"

class csqlitertestdataaccess
{
public:
	csqlitertestdataaccess(void);
	~csqlitertestdataaccess(void);

	void setbasebath(std::string &basepath);

	int testtypesreturned();
	int testvaluesasnull();

	int testresponsenumargserror();
	int testresponseargtypeserror();

private:
	std::string testpath;
	std::string fullpath;

};

