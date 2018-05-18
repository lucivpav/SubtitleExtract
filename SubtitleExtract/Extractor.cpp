#include "Extractor.h"

#include <opencv2/opencv.hpp>

Extractor::Extractor(const std::string & videoFilePath, double from, double to)
{
	cv::VideoCapture video(videoFilePath);
	if ( from != -1 )
		video.set(cv::CAP_PROP_POS_MSEC, from);
	cv::Mat image, grayImage, threshImage, finalImage, dilatedImage;
	if (!video.read(image))
		throw 666;
	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
	cv::threshold(grayImage, threshImage, 180, 255, cv::THRESH_BINARY);
	cv::bitwise_and(grayImage, grayImage, finalImage, threshImage);
	cv::imwrite("test_thresh.png", finalImage);

	auto iterations = 9;
	auto kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
	cv::dilate(finalImage, dilatedImage, kernel, cv::Point(-1, -1), iterations);

	cv::imwrite("test_thresh_dilated.png", dilatedImage);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(dilatedImage, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	for (auto contour : contours)
	{
		auto rect = cv::boundingRect(contour);
		cv::rectangle(image, rect, cv::Scalar(255,0,255));
	}

	if (!cv::imwrite("test_rects.png", image))
		throw 666;
}


Extractor::~Extractor()
{
}
