#include "Detector.h"

Detector::Detector(int dilationIterations, double minFraction)
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
	auto cropY = imHeight * 0.87;
	auto cropWidth = imWidth - 2 * cropX;
	auto cropHeight = imHeight * 0.09;
	auto crop = cv::Rect(cropX, cropY, cropWidth, cropHeight); // TODO
	return Detect(image, crop, id);
}

DetectionResult Detector::Detect(const cv::Mat & image, const cv::Rect & crop, const std::string & id) const
{
	DetectionResult result;
	cv::Mat cropImage, edgeImage, morphedImage, threshImage, focusedImage, focusedDilatedImage;
	cropImage = cv::Mat(image.clone(), crop);
	cv::Canny(cropImage, edgeImage, 10, 90);

	const std::string dir = "extractions/";
	cv::imwrite(dir + id + "_edges.png", edgeImage);
	
	auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::dilate(edgeImage, morphedImage, kernel, cv::Point(-1, -1), dilationIterations);
	threshImage = ThresholdImage(cropImage);
	cv::bitwise_and(morphedImage, threshImage, focusedImage);

	// dilate again
	kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 1));
	cv::dilate(focusedImage, focusedDilatedImage, kernel, cv::Point(-1, -1), dilationIterations);

	cv::imwrite(dir + id + "_dilated.png", morphedImage);
	cv::imwrite(dir + id + "_thresh.png", threshImage);
	cv::imwrite(dir + id + "_focusedImage.png", focusedImage);
	cv::imwrite(dir + id + "_focusedDilatedImage.png", focusedDilatedImage);

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(focusedDilatedImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	for (auto contour : contours)
		result.rectangles.push_back(cv::boundingRect(contour));

	// make the rectangles little taller
	auto extraY = crop.size().height*0.05;
	for (auto & rect : result.rectangles)
	{
		auto rectY = rect.y - extraY;
		auto rectHeight = rect.height + extraY * 2;
		if (rectY >= 0 && rectY + rectHeight < crop.height)
		{
			rect.y = rectY;
			rect.height = rectHeight;
		}
	}

	if (id != "")
	{
		auto rectsImage = cropImage.clone();
		for (auto rect : result.rectangles)
			cv::rectangle(rectsImage, rect, cv::Scalar(255, 0, 255));
		if (!cv::imwrite(dir + id + "_prefilter_rects.png", rectsImage))
			throw 666;
	}

	RemoveUnlikelyRectangles(result, crop);

	result.image = ThresholdImage(cropImage);

	// make sure there are some vertical bezels. this improves tesseract's accuracy
	// TODO: remove?
	auto bezelH = 10;
	auto bezel = cv::Mat::zeros(bezelH, result.image.cols, result.image.type());
	cv::Mat bezelImage;
	std::vector<cv::Mat> arraysToMerge = {bezel, result.image, bezel};
	cv::vconcat(arraysToMerge, bezelImage);
	result.image = bezelImage;
	for (auto & rect : result.rectangles)
		rect.height += 2*bezelH;
		

	std::sort(result.rectangles.begin(), result.rectangles.end(), [](const auto & r1, const auto & r2) {
		return r1.y < r2.y;
	});

	return result;
}

cv::Mat Detector::ThresholdImage(const cv::Mat & image) const
{
	cv::Mat grayImage, preThreshImage, threshImage;
	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
	cv::threshold(grayImage, threshImage, 200, 255, cv::THRESH_BINARY);
	//cv::bitwise_and(grayImage, grayImage, threshImage, preThreshImage);
	return threshImage;
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

cv::Point2d Detector::GetExpectedSubtitleLocation(const cv::Rect & crop) const
{
	return cv::Point2d(crop.width/2, crop.height/2);
}