#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

struct DetectionResult {
	std::vector<cv::Rect> rectangles;
	size_t mostLikelyRectangle;
	cv::Mat image;
};

class Detector
{
public:
	Detector(int dilationIterations = 9, double minFraction = 0.005, double positionFraction = 0.3); // TODO: maxFraction
	~Detector();

	DetectionResult Detect(const cv::Mat & image, const std::string & id = "") const;
private:
	int dilationIterations;
	double minFraction;
	cv::Point2d optimalSubtitleCenter;

	void RemoveUnlikelyRectangles(DetectionResult & detection, const cv::Mat & image) const;
	void RemoveRectanglesOfUnlikelySize(DetectionResult & detection) const;
	void RemoveRectanglesOfUnlikelyPosition(DetectionResult & detection, const cv::Mat & image) const;
	size_t MostLikelyRectangle(const std::vector<cv::Rect> & candidates, const cv::Mat & image) const;
	cv::Point2d GetExpectedSubtitleLocation(const cv::Mat & image) const;
};

