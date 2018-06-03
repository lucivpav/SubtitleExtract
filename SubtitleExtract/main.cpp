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
	double from = 240000;
	double to = 350000;
	bool debug = true;

	if ( !debug )
		extractor.Extract(file, "subtitles.srt", from, to);
	else
	{
		std::cout << "opening video " << file << std::endl;
		cv::VideoCapture video(file);
		video.set(cv::CAP_PROP_POS_MSEC, 240000);
		cv::Mat image;
		if (!video.read(image))
		{
			std::cerr << "cannot read image from video.\n";
			throw 666;
		}
		std::cout << "extracted: " << extractor.ExtractFromImage(image, "bla") << std::endl;
	}

	return 0;
}