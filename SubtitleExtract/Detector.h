#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

struct DetectResult {
	std::vector<cv::Rect> rectangles;
	cv::Mat image;
};

class Detector
{
public:
	Detector(int dilationIterations = 9);
	~Detector();

	DetectResult Detect(const cv::Mat & image);
private:
	int dilationIterations;
};

