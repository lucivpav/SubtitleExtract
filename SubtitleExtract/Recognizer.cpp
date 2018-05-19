#include "Recognizer.h"

Recognizer::Recognizer()
	:api(new tesseract::TessBaseAPI())
{
	auto ret = api->Init("./tessdata", "eng");
	if (ret != 0)
		throw "Could not initialize tesseract (code " + std::to_string(ret) + ").";
}

Recognizer::~Recognizer()
{
}

std::string Recognizer::Recognize(cv::Mat & image) const
{
	api->SetImage(image.data, image.size().width, image.size().height, image.channels(), (int)image.step1());
	auto xxx = api->GetUTF8Text();
	std::unique_ptr<char> textPtr(api->GetUTF8Text());
	std::string text(textPtr.get());
	return text;
}