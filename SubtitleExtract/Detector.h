#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

struct DetectionResult {
	std::vector<cv::Rect> rectangles;
	cv::Mat image;
};

class Detector
{
public:
	Detector(int dilationIterations = 9, double minFraction = 0.007, double positionFraction = 0.3); // TODO: maxFraction
	~Detector();

	DetectionResult Detect(const cv::Mat & image, const cv::Rect & crop, const std::string & id = "") const;
	DetectionResult Detect(const cv::Mat & image, const std::string & id = "") const;
private:
	int dilationIterations;
	double minFraction;

	void RemoveUnlikelyRectangles(DetectionResult & detection, const cv::Rect & crop) const;
	void RemoveRectanglesOfUnlikelySize(DetectionResult & detection) const;
	void RemoveRectanglesOfUnlikelyPosition(DetectionResult & detection, const cv::Rect & crop) const;
	cv::Point2d GetExpectedSubtitleLocation(const cv::Rect & crop) const;
};

