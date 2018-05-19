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
	Detector(int dilationIterations = 9, double minFraction = 0.05, double positionFraction = 0.3); // TODO: maxFraction
	~Detector();

	DetectionResult Detect(const cv::Mat & image) const;
private:
	int dilationIterations;
	double minFraction;
	double positionFraction;

	void RemoveUnlikelyRectangles(DetectionResult & detection) const;
	void RemoveRectanglesOfUnlikelySize(DetectionResult & detection) const;
	void RemoveRectanglesOfUnlikelyPosition(DetectionResult & detection) const;
};

