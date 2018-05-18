#include <iostream>

#include "CommandLineParser.h"
#include "Extractor.h"

void usage() {
	std::cout << "usage: SubtitleExtract --file <video_file> [--verbose|--from|--to|--threads]\n" 
			  << "                       --help\n";
}

int main(int argc, const char ** argv) {
	auto parser = CommandLineParser(argc, argv);
	auto file = parser.GetCommandLineParameterValue("file");
	auto help = parser.IsCommandLineParameterSet("help");
	if (help || file.empty())
	{
		usage();
		std::cin.get();
		return 1;
	}
	auto from = parser.GetCommandLineParameterValue("from");
	auto to = parser.GetCommandLineParameterValue("to");

	Extractor extractor(file, 200000);//from, to);

	return 0;
}