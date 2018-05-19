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
	auto & threshImage = detection.image;

	for (auto rect : detection.rectangles)
	{
		cv::rectangle(image, rect, cv::Scalar(255,0,255));
		auto textImage = cv::Mat(threshImage, rect);
		auto text = recognizer.Recognize(textImage);
		std::cout << "Recognized: " << text << std::endl;
	}

	if (!cv::imwrite("test_rects.png", image))
		throw 666;

	if (!cv::imwrite("test_thresh.png", threshImage))
		throw 666;
}

Extractor::~Extractor()
{
}
