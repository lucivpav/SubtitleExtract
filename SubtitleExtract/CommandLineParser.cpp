#include "CommandLineParser.h"

CommandLineParser::CommandLineParser(int argc, const char ** argv)
	:argc(argc), argv(argv)
{
}

bool CommandLineParser::IsCommandLineParameterSet(const std::string & param) {
	auto end = argv + argc;
	return std::find(argv, end, "--" + param) != end;
}

std::string CommandLineParser::GetCommandLineParameterValue(const std::string & param) {
	auto end = argv + argc;
	auto name = std::find(argv, end, "--" + param);
	if (name == end)
		return "";
	auto nameStr = *name;
	auto value = ++name;
	if (value == end)
		return "";
	return *value;
}
