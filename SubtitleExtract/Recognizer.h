#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

class Recognizer
{
public:
	Recognizer();
	~Recognizer();

	std::string Recognize(cv::Mat & image) const;
private:
	std::unique_ptr<tesseract::TessBaseAPI> api;
};

