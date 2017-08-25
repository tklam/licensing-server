#pragma once

#include <string>
#include "../src/sqliter.h"

class csqlitertestfileoperations
{
public:
	csqlitertestfileoperations(void);
	~csqlitertestfileoperations(void);

	void setbasebath(std::string &basepath);

	int testdbcreation();
	int testdbopen();
	int testdbremove();

private:
	std::string testpath;
	std::string fullpath;

};

