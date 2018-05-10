#include "Extractor.h"

#include <opencv2/opencv.hpp>

Extractor::Extractor(const std::string & videoFilePath, double from, double to)
{
	cv::VideoCapture video(videoFilePath);
	if ( from != -1 )
		video.set(CV_CAP_PROP_POS_MSEC, from);
	cv::Mat image;
	if (!video.read(image))
		throw 666;
	if (!cv::imwrite("test.png", image))
		throw 666;
}


Extractor::~Extractor()
{
}
