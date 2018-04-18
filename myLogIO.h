#pragma once
#pragma warning(disable:4996)
#include "Common.h"
#include <string>
#include <fstream>
#include <time.h>

class myLogIO
{
private:
	std::string m_fname;
	boost::lockfree::queue<char*> msgQ{ 100 };
public:
	myLogIO() : m_fname(currentDateTime('D')+".txt") {}
	myLogIO(std::string fname) : m_fname(fname) {}
	~myLogIO() {}

	bool PushMessage(char* msg);

	bool UnfulfilledLog();

	bool WriteLog();
	bool Write(char* msg);

	inline const std::string currentDateTime(char type = 'T') {
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];
		tstruct = *localtime(&now);
		switch (type)
		{
		case 'D':
			strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
			break;
		case 'T':
			strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
			break;
		}

		return buf;
	}
};