#pragma once

#include <string>

class CommandLineParser {
public:
	CommandLineParser(int argc, const char ** argv);
	bool IsCommandLineParameterSet(const std::string & param);
	std::string GetCommandLineParameterValue(const std::string & param);
private:
	int argc;
	const char ** argv;
};