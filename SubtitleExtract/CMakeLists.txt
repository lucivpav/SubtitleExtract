cmake_minimum_required (VERSION 3.8)

add_executable (SubtitleExtract "CommandLineParser.cpp" "CommandLineParser.h" "Detector.cpp" "Detector.h" "Extractor.cpp" "Extractor.h" "Recognizer.cpp" "Recognizer.h" "main.cpp")

find_package(Leptonica REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Tesseract REQUIRED)

target_include_directories(SubtitleExtract PRIVATE ${OpenCV_INCLUDE_DIRS} ${Tesseract_INCLUDE_DIRS})
target_link_libraries(SubtitleExtract PRIVATE opencv_world libtesseract)