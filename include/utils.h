#pragma once
#include <fstream>
#include <io.h>
#include <algorithm>
#include <filesystem>
#include <random>

#include <QImage>

#include <opencv2/opencv.hpp>

#define CV_VERSION_ID CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) \
  CVAUX_STR(CV_SUBMINOR_VERSION)

#ifdef _DEBUG
#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
#define otherLIB(name) name "d"
#else
#define cvLIB(name) "opencv_" name CV_VERSION_ID
#define otherLIB(name) name
#endif //_DEBUG

#pragma comment(lib, cvLIB("world"))

bool FileExist(const std::string& filename);
std::string getFileExt(const std::string& filename);
std::string str2Lower(std::string str);
int getFilelistRecursive(std::string path, std::vector<std::string>& filelist);
int getImgFilelist(std::vector<std::string>& totalfilelist, std::vector<std::string>& imgfilelist);
std::vector<std::string> readNamesFile(std::string filename);

int openImage(QString img_path, cv::Size& img_size, QImage& image);
cv::Mat resizeImage(cv::Mat const& src);
QImage mat2QImage(cv::Mat const& src);
cv::Mat qImage2Mat(QImage const& src);
cv::Mat convertMat32StoRGBC3(const cv::Mat& mat);
int getRandomColor(QColor& rgb);
QImage watershed(const QImage& qimage, const QImage& qmarkers_mask);