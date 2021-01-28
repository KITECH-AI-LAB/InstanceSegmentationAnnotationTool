#include "utils.h"

bool FileExist(const std::string& filename)
{
    std::ifstream read;

    read.open(filename);

    if (!read)
        return false;
    else
        return true;
}

std::string getFileExt(const std::string& filename)
{
    size_t s_pos = filename.find_last_of(".") + 1;
    size_t e_pos = filename.length() - s_pos;
    std::string ext = filename.substr(s_pos, e_pos);
    
    return ext;
};

std::string str2Lower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    
    return str;
}

int getFilelistRecursive(std::string path, std::vector<std::string>& filelist)
{
    filelist.clear();
    for (auto& p : std::experimental::filesystem::recursive_directory_iterator(path))
        filelist.emplace_back(p.path().string());
		
    return 0;
}

int getImgFilelist(std::vector<std::string>& totalfilelist, std::vector<std::string>& imgfilelist)
{
    imgfilelist.clear();
    std::vector<std::string> img_ext_list = {"jpg","jpeg","png","bmp"};
	
    for (auto file : totalfilelist)
    {
        std::string ext = getFileExt(file);
        std::vector<std::string>::iterator it = std::find(img_ext_list.begin(), img_ext_list.end(), str2Lower(ext));

        if (it == img_ext_list.end())
	        continue;

        imgfilelist.emplace_back(file);
    }
	
    return 0;
}

std::vector<std::string> readNamesFile(std::string filename)
{
    std::ifstream file(filename);
    std::vector<std::string> file_lines;
    if (!file.is_open()) return file_lines;
    for (std::string line; file >> line;) file_lines.push_back(line);
    
    return file_lines;
}

int openImage(QString img_path, cv::Size& img_size, QImage& image)
{
    std::string path = img_path.toLocal8Bit();
    cv::Mat cv_img = cv::imread(path);
    
    img_size = cv_img.size();

    cv_img = resizeImage(cv_img);

    image = mat2QImage(cv_img);

    return 0;
}

cv::Mat resizeImage(cv::Mat const& src)
{
    int max_width = 1400;
    int max_height = 900;

    int current_width = src.cols;
    int current_height = src.rows;

    int width = std::min(max_width, current_width);
    int height = std::min(max_height, current_height);

    cv::Mat dst;
    cv::resize(src, dst, cv::Size(width, height), 0, 0, cv::INTER_NEAREST);

    return dst;
}

QImage mat2QImage(cv::Mat const& src)
{
    cv::Mat temp;
    cv::cvtColor(src, temp, cv::COLOR_BGR2RGB);
    QImage dest((const uchar *)temp.data, temp.cols, temp.rows, int(temp.step), QImage::Format_RGB888);
    dest.bits(); // enforce deep copy, see documentation 
                 // of QImage::QImage ( const uchar * data, int width, int height, Format format )
    return dest;
}

cv::Mat qImage2Mat(QImage const& src)
{
    cv::Mat tmp(src.height(), src.width(), CV_8UC3, (uchar*)src.bits(), src.bytesPerLine());
    cv::Mat result;
    cv::cvtColor(tmp, result, cv::COLOR_BGR2RGB);

    return result;
}

cv::Mat convertMat32StoRGBC3(const cv::Mat& mat)
{
    cv::Mat dst(mat.size(), CV_8UC3);
    uchar *pix;
    uchar label;
    for (int r = 0; r < dst.rows; ++r)
    {
        const int* ptr = mat.ptr<int>(r);
        uchar* ptr_dst = dst.ptr<uchar>(r);
        for (int c = 0; c < dst.cols; ++c)
        {
            label = ptr[c];
            pix = &ptr_dst[c * 3];
            if (label > 0 && label < 255)
            {
                pix[0] = label;
                pix[1] = label;
                pix[2] = label;
            }
            else {
                pix[0] = 0;
                pix[1] = 0;
                pix[2] = 0;
            }
        }
    }
    return dst;
}

int getRandomColor(QColor& rgb)
{
    std::random_device rng;

	std::mt19937_64 rnd(rng());

	std::uniform_int_distribution<int> range(0, 128);

	int r = 127 + range(rnd);
	int g = 127 + range(rnd);
	int b = 127 + range(rnd);

    rgb = QColor(r, g, b);

    return 0;
}

QImage watershed(const QImage& qimage, const QImage& qmarkers_mask)
{
    cv::Mat image = qImage2Mat(qimage);
    cv::Mat markers_mask = qImage2Mat(qmarkers_mask);
    cv::Mat markers = cv::Mat::zeros(markers_mask.size(), CV_32S);
    for (int y = 0; y < markers_mask.rows; y++)
    {
        int* mark = markers.ptr<int>(y);
        cv::Vec3b* mask = markers_mask.ptr<cv::Vec3b>(y);
        for (int x = 0; x < markers_mask.cols; x++)
            mark[x] = mask[x][0];
    }
    cv::watershed(image, markers);
    cv::Mat new_mask = convertMat32StoRGBC3(markers);

    return mat2QImage(new_mask);
}