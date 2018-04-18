#include "myLogIO.h"
#include <iostream>
#include <Windows.h>


CRITICAL_SECTION m_cs;


bool myLogIO::PushMessage(char* msg)
{
	if (msg == nullptr)
		return false;
	msgQ.push(msg);

	return true;
}

bool myLogIO::UnfulfilledLog()
{
	if (msgQ.empty())
		return false;


	return true;
}

bool myLogIO::WriteLog()
{
	while (true)
	{
		if (!msgQ.empty())
		{
			cout << currentDateTime() << endl;
			char* value;
			msgQ.pop(value);
			Write(value);
		}
		Sleep(100);
	}

	return false;
}

bool myLogIO::Write(char* msg)
{
	ofstream file(m_fname, ios::app);
	if (!file.is_open())
		return false;
	else
	{
		std::string log = msg;
		cout << "Write Log" << endl;
		file << "- Time : " << currentDateTime() << " - Error Content: " << log << std::endl;
		if (msgQ.empty())
		{
			file.close();
		}
		return true;
	}

}

