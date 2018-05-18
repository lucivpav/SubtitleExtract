#include "Detector.h"

Detector::Detector(int dilationIterations, double minFraction, double positionFraction)
	:	dilationIterations(dilationIterations),
		minFraction(minFraction),
		positionFraction(positionFraction)
{
}

Detector::~Detector()
{
}

DetectionResult Detector::Detect(const cv::Mat & image)
{
	DetectionResult result;
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

	RemoveUnlikelyRectangles(result);

	return result;
}

void Detector::RemoveUnlikelyRectangles(DetectionResult & detection)
{
	RemoveRectanglesOfUnlikelySize(detection);
	RemoveRectanglesOfUnlikelyPosition(detection);
}

void Detector::RemoveRectanglesOfUnlikelySize(DetectionResult & detection)
{
	auto imageArea = detection.image.rows * detection.image.cols;
	auto & rectangles = detection.rectangles;
	auto newEnd = std::remove_if(rectangles.begin(), rectangles.end(), [=](auto & rect) {
		return rect.area() < imageArea * minFraction;
	});
	rectangles.erase(newEnd, rectangles.end());
}

void Detector::RemoveRectanglesOfUnlikelyPosition(DetectionResult & detection) // TODO: extend by supporting subtitles on top too
{
	auto & rectangles = detection.rectangles;
	auto newEnd = std::remove_if(rectangles.begin(), rectangles.end(), [=](auto & rect) {
		return rect.y < positionFraction;
	});
	rectangles.erase(newEnd, rectangles.end());
}
