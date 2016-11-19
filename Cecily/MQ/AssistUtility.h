#ifndef C_Assist_Utility_H
#define C_Assist_Utility_H
#include <sstream>
#include <string>
#include <Objbase.h>
using namespace std;


class CAssistUtility
{
public:
	CAssistUtility(void);
	~CAssistUtility(void);

	static std::string GetNowTime();
	static std::string GenerateGUID();
	static void AutoCreateDirectory(const std::string &strFilePath);
};

#endif