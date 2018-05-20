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

DetectionResult Detector::Detect(const cv::Mat & image, const std::string & id) const
{
	DetectionResult result;
	cv::Mat grayImage, preThreshImage, dilatedImage, threshImage;
	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
	cv::threshold(grayImage, preThreshImage, 245, 255, cv::THRESH_BINARY);
	cv::bitwise_and(grayImage, grayImage, threshImage, preThreshImage);
	cv::medianBlur(threshImage, result.image, 3);

	auto kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));
	cv::dilate(result.image, dilatedImage, kernel, cv::Point(-1, -1), dilationIterations);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(dilatedImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	for (auto contour : contours)
		result.rectangles.push_back(cv::boundingRect(contour));

	if (id != "")
	{
		const std::string dir = "extractions/";
		auto rectsImage = image.clone();
		for (auto rect : result.rectangles)
			cv::rectangle(rectsImage, rect, cv::Scalar(255, 0, 255));
		if (!cv::imwrite(dir + id + "_prefilter_rects.png", rectsImage))
			throw 666;
	}

	RemoveUnlikelyRectangles(result);

	return result;
}

void Detector::RemoveUnlikelyRectangles(DetectionResult & detection) const
{
	RemoveRectanglesOfUnlikelySize(detection);
	RemoveRectanglesOfUnlikelyPosition(detection);
}

void Detector::RemoveRectanglesOfUnlikelySize(DetectionResult & detection) const
{
	auto imageArea = detection.image.rows * detection.image.cols;
	auto & rectangles = detection.rectangles;
	auto newEnd = std::remove_if(rectangles.begin(), rectangles.end(), [=](auto & rect) {
		return rect.area() < imageArea * minFraction;
	});
	rectangles.erase(newEnd, rectangles.end());
}

void Detector::RemoveRectanglesOfUnlikelyPosition(DetectionResult & detection) const // TODO: extend by supporting subtitles on top too
{
	auto & rectangles = detection.rectangles;
	auto newEnd = std::remove_if(rectangles.begin(), rectangles.end(), [=](auto & rect) {
		return rect.y < positionFraction;
	});
	rectangles.erase(newEnd, rectangles.end());
}
