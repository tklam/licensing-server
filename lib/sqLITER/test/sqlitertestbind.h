#pragma once

#include "../src/sqliter.h"

class csqlitertestbind
{
public:
	csqlitertestbind();
	~csqlitertestbind();

	void setbasebath(std::string &basepath);

	int testeachtypeofbind();

private:
	std::string testpath;
	std::string fullpath;

};

