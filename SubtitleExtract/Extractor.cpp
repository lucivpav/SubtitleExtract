#include "Extractor.h"

#include <opencv2/opencv.hpp>

Extractor::Extractor(const std::string & videoFilePath, double from, double to)
	:detector()
{
	cv::VideoCapture video(videoFilePath);
	if ( from != -1 )
		video.set(cv::CAP_PROP_POS_MSEC, from);
	cv::Mat image;
	if (!video.read(image))
		throw 666;

	auto detection = detector.Detect(image);

	for (auto rect : detection.rectangles)
	{
		cv::rectangle(image, rect, cv::Scalar(255,0,255));
	}

	if (!cv::imwrite("test_rects.png", image))
		throw 666;
}

Extractor::~Extractor()
{
}
