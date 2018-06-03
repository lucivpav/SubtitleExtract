#include "Recognizer.h"

Recognizer::Recognizer()
	:api(new tesseract::TessBaseAPI())
{
	auto ret = api->Init("./tessdata", "chi_tra");
	if (ret != 0)
		throw "Could not initialize tesseract (code " + std::to_string(ret) + ").";
}

Recognizer::~Recognizer()
{
}

std::string Recognizer::Recognize(cv::Mat & image) const
{
	api->SetImage(image.data, image.size().width, image.size().height, image.channels(), (int)image.step1());
	api->SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_LINE);
	char* textPtr = (api->GetUTF8Text());
	std::string text(textPtr);
	text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
	return text;
}