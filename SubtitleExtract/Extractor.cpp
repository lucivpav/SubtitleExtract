#include "Extractor.h"

#include <opencv2/opencv.hpp>

Extractor::Extractor()
	:detector()
{
}

Extractor::~Extractor()
{
}

void Extractor::Extract(const std::string & videoFilePath, const std::string & subtitlePath, double from, double to, double step)
{
	cv::VideoCapture video(videoFilePath);

	if (from == -1)
		from = 0;
	if (to == -1)
	{
		auto frameCount = video.get(cv::CAP_PROP_FRAME_COUNT);
		auto fps = video.get(cv::CAP_PROP_FPS);
		to = (frameCount / fps) * 1000;
	}

	std::ofstream subtitleFile(subtitlePath);
	int subtitleId = 1;
	std::string lastSubtitle;
	double timeBegin;

	for (; from < to; from += step)
	{
		std::cout << "Position: " << from << " [ms]\n";
		video.set(cv::CAP_PROP_POS_MSEC, from);
		cv::Mat image;
		if (!video.read(image))
			throw 666;

		auto text = ExtractFromImage(image, std::to_string(from));
		if (text.empty())
			std::cout << "<nothing extracted>" << std::endl;
		else
			std::cout << "Extracted: " << text << "\n\n\n";

		if (lastSubtitle.empty())
		{
			lastSubtitle = text;
			timeBegin = from;
		}
		else
		{
			if (lastSubtitle != text)
			{
				auto timeEnd = from - step;
				subtitleFile << subtitleId++ << std::endl;
				subtitleFile << TimeCode(timeBegin) << " --> " << TimeCode(timeEnd) << std::endl;
				subtitleFile << lastSubtitle;
				lastSubtitle = text;
				timeBegin = from;
			}
		}
		// TODO: final subtitle
	}
}

std::string Extractor::TimeCode(int milliseconds)
{
	auto sec2ms = 1000;
	auto min2ms = sec2ms * 60;
	auto hr2ms = min2ms * 60;
	auto hours = milliseconds / hr2ms;
	milliseconds -= hours * hr2ms;
	auto minutes = milliseconds / min2ms;
	milliseconds -= minutes * min2ms;
	auto seconds = milliseconds / sec2ms;

	std::ostringstream oss;
	oss << std::setfill('0') << std::setw(2) << hours << ":";
	oss << std::setfill('0') << std::setw(2) << minutes << ":";
	oss << std::setfill('0') << std::setw(2) << seconds << ",";
	oss << std::setfill('0') << std::setw(3) << milliseconds / 10;
	return oss.str();
}

std::string Extractor::ExtractFromImage(const cv::Mat & image, const std::string & id)
{
	const std::string dir = "extractions/";
	auto detection = detector.Detect(image, id);
	auto & threshImage = detection.image;
	const auto & rectangles = detection.rectangles;
	auto rectangleCount = rectangles.size();

	if (rectangleCount == 0)
	{
		if (!cv::imwrite(dir + id + "_thresh.png", threshImage))
			throw 666;
		return "";
	}

	const auto & rect = rectangles[detection.mostLikelyRectangle];
	auto textImage = cv::Mat(threshImage, rect);

	cv::rectangle(threshImage, rect, cv::Scalar(255, 0, 255));
	if (!cv::imwrite(dir + id + "_thresh.png", threshImage))
		throw 666;

	return recognizer.Recognize(textImage);
}