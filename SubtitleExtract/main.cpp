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
	auto fromStr = parser.GetCommandLineParameterValue("from");
	auto toStr = parser.GetCommandLineParameterValue("to"); // TODO

	Extractor extractor;
	double from = 200000;
	double to = 220000;
	bool debug = false;

	if ( !debug )
		extractor.Extract(file, "subtitles.srt", from, to);
	else
	{
		cv::VideoCapture video(file);
		video.set(cv::CAP_PROP_POS_MSEC, 209000);
		cv::Mat image;
		if (!video.read(image))
			throw 666;
		std::cout << "extracted: " << extractor.ExtractFromImage(image, "bla") << std::endl;
	}

	std::cin.get();
	return 0;
}