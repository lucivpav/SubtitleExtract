#pragma once

#include <string>
#include "Detector.h"
#include "Recognizer.h"

class Extractor
{
public:
	Extractor();
	~Extractor();

	void Extract(const std::string & videoFilePath, const std::string & subtitlePath, double from = -1, double to = -1, double step = 1000);
	std::string ExtractFromImage(const cv::Mat & image, const std::string & id = "");
private:
	Detector detector;
	Recognizer recognizer;

	const cv::Rect & MostLikelyRectangle(const std::vector<cv::Rect> & candidates, const cv::Mat & image) const;
};

