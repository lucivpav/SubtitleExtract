#include "Detector.h"

Detector::Detector(int dilationIterations)
	:dilationIterations(dilationIterations)
{
}

Detector::~Detector()
{
}

DetectResult Detector::Detect(const cv::Mat & image)
{
	DetectResult result;
	cv::Mat grayImage, preThreshImage, threshImage;
	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
	cv::threshold(grayImage, preThreshImage, 180, 255, cv::THRESH_BINARY);
	cv::bitwise_and(grayImage, grayImage, threshImage, preThreshImage);

	auto kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
	cv::dilate(threshImage, result.image, kernel, cv::Point(-1, -1), dilationIterations);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(result.image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	for (auto contour : contours)
		result.rectangles.push_back(cv::boundingRect(contour));

	return result;
}
