#include "Detector.h"

Detector::Detector(int dilationIterations, double minFraction, double positionFraction)
	:	dilationIterations(dilationIterations),
		minFraction(minFraction)
{
}

Detector::~Detector()
{
}

DetectionResult Detector::Detect(const cv::Mat & image, const std::string & id) const
{
	auto imWidth = image.size().width;
	auto imHeight = image.size().height;
	auto cropX = imWidth * 0.1;
	auto cropY = imHeight * 0.8;
	auto cropWidth = imWidth - 2 * cropX;
	auto cropHeight = imHeight * 0.15;
	auto crop = cv::Rect(cropX, cropY, cropWidth, cropHeight); // TODO
	return Detect(image, crop, id);
}

DetectionResult Detector::Detect(const cv::Mat & image, const cv::Rect & crop, const std::string & id) const
{
	DetectionResult result;
	cv::Mat cropImage, grayImage, preThreshImage, dilatedImage, threshImage;
	cropImage = cv::Mat(image.clone(), crop);
	cv::cvtColor(cropImage, grayImage, cv::COLOR_BGR2GRAY);
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
		auto rectsImage = cropImage.clone();
		for (auto rect : result.rectangles)
			cv::rectangle(rectsImage, rect, cv::Scalar(255, 0, 255));
		if (!cv::imwrite(dir + id + "_prefilter_rects.png", rectsImage))
			throw 666;
	}

	RemoveUnlikelyRectangles(result, crop);
	result.mostLikelyRectangle = MostLikelyRectangle(result.rectangles, crop);

	return result;
}

void Detector::RemoveUnlikelyRectangles(DetectionResult & detection, const cv::Rect & crop) const
{
	RemoveRectanglesOfUnlikelySize(detection);
	RemoveRectanglesOfUnlikelyPosition(detection, crop);
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

void Detector::RemoveRectanglesOfUnlikelyPosition(DetectionResult & detection, const cv::Rect & crop) const // TODO: extend by supporting subtitles on top too
{
	auto & rectangles = detection.rectangles;
	auto optimalCenter = GetExpectedSubtitleLocation(crop);
	auto maxDistance = cv::norm(cv::Point2d(crop.width, crop.height))*0.05;

	auto newEnd = std::remove_if(rectangles.begin(), rectangles.end(), [&](auto & rect) {
		cv::Point2d actualCenter(rect.x + rect.width / 2, rect.y + rect.height / 2);
		auto distance = cv::norm(optimalCenter - actualCenter);
		return distance > maxDistance;
	});
	rectangles.erase(newEnd, rectangles.end());
}

size_t Detector::MostLikelyRectangle(const std::vector<cv::Rect> & candidates, const cv::Rect & crop) const
{
	std::vector<double> distances;
	auto optimalSubtitleCenter = GetExpectedSubtitleLocation(crop);
	distances.resize(candidates.size());
	std::transform(candidates.begin(), candidates.end(), distances.begin(), [&](const cv::Rect & rect) {
		cv::Point2d actualCenter(rect.x + rect.width/2, rect.y + rect.height/2);
		return cv::norm(optimalSubtitleCenter - actualCenter);
	});
	return std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()));
}

cv::Point2d Detector::GetExpectedSubtitleLocation(const cv::Rect & crop) const
{
	return cv::Point2d(crop.width/2, crop.height/2);
}