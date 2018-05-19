#pragma once

#include <string>
#include "Detector.h"
#include "Recognizer.h"

class Extractor
{
public:
	Extractor(const std::string & videoFilePath, double from = -1, double to = -1);
	~Extractor();
private:
	Detector detector;
	Recognizer recognizer;
};

