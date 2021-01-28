#include "ISAT.h"

#include <QDebug>

ImageMask::ImageMask() {}

ImageMask::ImageMask(QSize s) 
{
    id = QImage(s, QImage::Format_RGB888);
    color = QImage(s, QImage::Format_RGB888);
    id.fill(QColor(0, 0, 0));
    color.fill(QColor(0, 0, 0));
}

void ImageMask::drawFillCircle(int x, int y, int pen_size, ColorMask cm, QImage& image) 
{
    QPen pen(QBrush(cm.id), 1.0);
    QPainter painter_id(&id);
    painter_id.setRenderHint(QPainter::Antialiasing, false);
    painter_id.setPen(pen);
    painter_id.setBrush(QBrush(cm.id));
    painter_id.drawEllipse(x, y, pen_size, pen_size);
    painter_id.end();

    QPen pen_color(QBrush(cm.color), 1.0);
    QPainter painter_color(&color);
    painter_color.setRenderHint(QPainter::Antialiasing, false);
    painter_color.setPen(pen_color);
    painter_color.setBrush(QBrush(cm.color));
    painter_color.drawEllipse(x, y, pen_size, pen_size);
    painter_color.end();

    QPen pen_image(QBrush(cm.color), 1.0);
    QPainter painter_image(&image);
    painter_image.setRenderHint(QPainter::Antialiasing, false);
    painter_image.setPen(pen_image);
    painter_image.setBrush(QBrush(cm.color));
    painter_image.drawEllipse(x, y, pen_size, pen_size);
    painter_image.end();
}

void ImageMask::drawPixel(int x, int y, ColorMask cm, QImage& image)
{
    id.setPixelColor(x, y, cm.id);
    color.setPixelColor(x, y, cm.color);
    image.setPixelColor(x, y, cm.color);
}

ISAT::ISAT(QWidget *parent)
    :QLabel(parent)
{
    _imgFile = QString();
    _imgList = QStringList();
    _imgIndex = 0;
    _classList = QStringList();

    _pen_size = 30;
    
    _inputImg = QImage();
    _inputImg_size = cv::Size();

    _mouse_is_pressed = false;
    _mouse_pos = QPoint();

    _color = ColorMask();
    _mask = ImageMask();
    _watershed = ImageMask();
    _is_watershed = false;
    
    _init_watershed = ImageMask();

    _inputImg_display = QImage();
    _display_manual_mask = true;
    _display_watershed_mask = true;
    _draw_manual_mask = false;

    _effective_id.clear();
    _id_storage.clear();
}

void ISAT::readImage()
{
    _undo_mask.clear();
    _undo_watershed.clear();
    _undo_idx = 0;

    openImage(_imgFile, _inputImg_size, _inputImg);
    
    _inputImg_display = QImage(_inputImg.size(), QImage::Format_RGB888);
    QPainter painter(&_inputImg_display);
    painter.drawImage(QPoint(0, 0), _inputImg);
    painter.end();

    _mask = ImageMask(_inputImg.size());
    _watershed = ImageMask(_inputImg.size());

    read();

    _undo_mask.emplace_back(_mask);
    _undo_watershed.emplace_back(_watershed);
}

bool ISAT::checkDuplication(int id, QColor color)
{
    for (auto _id : _id_storage)
    {
        if (_id.id.red() == id)
            return false;
        if (_id.color == color)
            return false;
    }

    return true;
}

void ISAT::createID(int& id, QColor& color, QString class_name)
{
    id = 1;
    color = QColor();
    
    if (class_name == "Background")
    {
		color = QColor(0, 0, 0);
		id = 255;
    }
    else
    {
        while (1)
        {
            getRandomColor(color);

            if (checkDuplication(id, color))
                break;

            id++;
        }
    }
    
    ColorMask new_id;
    new_id.id = QColor(id, id, id);
    new_id.color = color;
    new_id.class_name = class_name;

    _id_storage.emplace_back(new_id);
}

void ISAT::createID(int id, QString class_name)
{
    QColor color = QColor();
	if (class_name == "Background") 
		color = QColor(0, 0, 0);
    else
    {
        while (1)
        {
            getRandomColor(color);

            if (checkDuplication(-1, color))
                break;
        }
    }

    ColorMask new_id;
    new_id.id = QColor(id, id, id);
    new_id.color = color;
    new_id.class_name = class_name;
    
    _id_storage.emplace_back(new_id);
}

bool ISAT::changeID(int id)
{
    for (auto _id : _id_storage)
    {
        if (_id.id.red() == id)
        {
            _color = _id;
            return true;
        }
    }
	return false;
}

void ISAT::changeBackgroundID()
{
	for (auto _id : _id_storage)
	{
		if (_id.class_name == "Background")
		{
			_color = _id;
			break;
		}
	}
}

void ISAT::checkID()
{
    _effective_id.clear();

    cv::Mat inputImg_display = qImage2Mat(_inputImg);
    cv::Mat id = qImage2Mat(_mask.id);

    int width = inputImg_display.cols;
    int height = inputImg_display.rows;

    for (int col = 0; col < width; col++)
    {
        for (int row = 0; row < height; row++)
        {
            int idx = id.at<cv::Vec3b>(row, col)[0];

            if (idx == 0)
                continue;

            std::vector<int>::iterator it = std::find(_effective_id.begin(), _effective_id.end(), idx);

            if (it == _effective_id.end())
                _effective_id.emplace_back(idx);
        }
    }
}

void ISAT::draw(QMouseEvent *e, QRect size)
{
    if (_color.class_name == "Background")
    {
        if (_draw_manual_mask)
            _color.id = QColor(0, 0, 0);
        else
            _color.id = QColor(255, 255, 255);
    }

    int mouse_pos_x = std::max(0, _mouse_pos.x() - size.x() - (size.width() - _inputImg.width()) / 2);
    int mouse_pos_y = std::max(0, _mouse_pos.y() - size.y() - (size.height() - _inputImg.height()) / 2 - 13);

    if (!_draw_manual_mask)
    {
        if (_pen_size > 0)
        {
            int x = mouse_pos_x - _pen_size / 2;
            int y = mouse_pos_y - _pen_size / 2;
            _mask.drawFillCircle(x, y, _pen_size, _color, _inputImg_display);
        }
        else
        {
            int x = (mouse_pos_y + 0.5);
            int y = (mouse_pos_y + 0.5);
            _mask.drawPixel(x, y, _color, _inputImg_display);
        }
    }
    else
    {
        if (_pen_size > 0)
        {
            int x = mouse_pos_x - _pen_size / 2;
            int y = mouse_pos_y - _pen_size / 2;
            _watershed.drawFillCircle(x, y, _pen_size, _color, _inputImg_display);
        }
        else
        {
            int x = (mouse_pos_y + 0.5);
            int y = (mouse_pos_y + 0.5);
            _watershed.drawPixel(x, y, _color, _inputImg_display);
        }
        update_mask();
    }
}

void ISAT::update_mask()
{
    if (_effective_id.size() == 0)
        return;

    cv::Mat inputImg_display = qImage2Mat(_inputImg);
    cv::Mat id = qImage2Mat(_mask.id);
    cv::Mat color = qImage2Mat(_mask.color);
    cv::Mat watershed_id = qImage2Mat(_watershed.id);

    int width = inputImg_display.cols;
    int height = inputImg_display.rows;

    for (int col = 0; col < width; col++)
    {
        for (int row = 0; row < height; row++)
        {
            int idx = id.at<cv::Vec3b>(row, col)[0];
            
            if (idx == 0)
                continue;

            std::vector<int>::iterator it = std::find(_effective_id.begin(), _effective_id.end(), idx);

            if (it == _effective_id.end())
            {
                id.at<cv::Vec3b>(row, col)[0] = 0;
                id.at<cv::Vec3b>(row, col)[1] = 0;
                id.at<cv::Vec3b>(row, col)[2] = 0;
                color.at<cv::Vec3b>(row, col)[0] = 0;
                color.at<cv::Vec3b>(row, col)[1] = 0;
                color.at<cv::Vec3b>(row, col)[2] = 0;

                watershed_id.at<cv::Vec3b>(row, col)[0] = 0;
                watershed_id.at<cv::Vec3b>(row, col)[1] = 0;
                watershed_id.at<cv::Vec3b>(row, col)[2] = 0;
            }
            else
            {
                if (_display_manual_mask)
                {
                    inputImg_display.at<cv::Vec3b>(row, col)[0] = color.at<cv::Vec3b>(row, col)[0];
                    inputImg_display.at<cv::Vec3b>(row, col)[1] = color.at<cv::Vec3b>(row, col)[1];
                    inputImg_display.at<cv::Vec3b>(row, col)[2] = color.at<cv::Vec3b>(row, col)[2];
                }
            }
        }
    }
    
    _mask.id = mat2QImage(id);
    _mask.color = mat2QImage(color);
    _watershed.id = mat2QImage(watershed_id);

    if (_display_watershed_mask && _is_watershed)
    {
        for (auto id_num : _effective_id)
        {
            cv::Mat mask = (watershed_id == id_num);

            cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY);

            mask.convertTo(mask, CV_8U);
            changeID(id_num);

            if (_color.class_name == "Background")
                continue;

            QColor rgb = _color.color;

            std::vector<cv::Mat> contours;
            cv::Mat hierarchy;
            cv::findContours(mask.clone(), contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

            if (contours.size() == 0)
                continue;

            cv::Rect box;
            for (auto contour : contours)
                box = box | cv::boundingRect(contour);    
            
            cv::Mat bgr_mat;
            std::vector<cv::Mat> bgr;
            bgr.emplace_back(mask.clone() / 255 * rgb.blue()); // b
            bgr.emplace_back(mask.clone() / 255 * rgb.green()); // g
            bgr.emplace_back(mask.clone() / 255 * rgb.red()); // r
            cv::merge(bgr, bgr_mat);

            cv::Mat coloredRoi = (0.7 * bgr_mat + 0.3 * inputImg_display);

            const cv::Rect curBoxRect(cv::Point(box.x, box.y), cv::Point(box.x+box.width, box.y+box.height));
            coloredRoi = coloredRoi(curBoxRect);

            coloredRoi.copyTo(inputImg_display(curBoxRect), mask(curBoxRect));

            cv::rectangle(inputImg_display, box, cv::Scalar(rgb.blue(), rgb.green(), rgb.red()), 2);
            cv::drawContours(inputImg_display, contours, -1, cv::Scalar(rgb.blue(), rgb.green(), rgb.red()), 2, cv::LINE_8, hierarchy, 100);
        }
    }
    
    _inputImg_display = mat2QImage(inputImg_display);
}

void ISAT::run_watershed()
{
    _is_watershed = false;
    _watershed = ImageMask();

    if (_effective_id.size() <= 1)
        return;
    
    QImage iwatershed = watershed(_inputImg, _mask.id);
    _watershed.id = iwatershed;
    idToColor(_watershed);
    _is_watershed = true;
    update_mask();
}

void ISAT::idToColor(ImageMask& mask)
{
    cv::Mat img_id = qImage2Mat(mask.id);
    
    int width = img_id.cols;
    int height = img_id.rows;

    cv::Mat img_color = cv::Mat::zeros(height, width, CV_8UC3);

    for (int col = 0; col < width; col++)
    {
        for (int row = 0; row < height; row++)
        {
            if (!changeID(img_id.at<cv::Vec3b>(row, col)[0])) 
			{

				changeBackgroundID();
				img_id.at<cv::Vec3b>(row, col)[0] = 0;
				img_id.at<cv::Vec3b>(row, col)[1] = 0;
				img_id.at<cv::Vec3b>(row, col)[2] = 0;
			}
            else
            {
                QColor rgb = _color.color;

                img_color.at<cv::Vec3b>(row, col)[0] = rgb.blue();
                img_color.at<cv::Vec3b>(row, col)[1] = rgb.green();
                img_color.at<cv::Vec3b>(row, col)[2] = rgb.red();
            }
        }
    }
	mask.id = mat2QImage(img_id);
    mask.color = mat2QImage(img_color);
}

void ISAT::save()
{
    if (_is_watershed && _init_watershed.id != _watershed.id)
    {        
        std::string save_root = _imgFile.toLocal8Bit();
        save_root = save_root.substr(0, save_root.find_last_of(".") + 1);

        cv::Mat watershed_mask = qImage2Mat(_watershed.id); // 255: edge
        if (qImage2Mat(_inputImg).size() != _inputImg_size)
            cv::resize(watershed_mask, watershed_mask, _inputImg_size, 0, 0, cv::INTER_NEAREST);
        cv::imwrite(save_root + "mask.png", watershed_mask);

        std::string old_mask_path = save_root + "mask.png";
        std::string new_mask_path = save_root + "mask";
        if (FileExist(new_mask_path))
            remove(new_mask_path.c_str());
        if (rename(old_mask_path.c_str(), new_mask_path.c_str()))
            return;
        
        cv::Mat drawing_mask = qImage2Mat(_mask.id);
        if (qImage2Mat(_inputImg).size() != _inputImg_size)
            cv::resize(drawing_mask, drawing_mask, _inputImg_size, 0, 0, cv::INTER_NEAREST);
        cv::imwrite(save_root + "dat.png", drawing_mask);

        old_mask_path = save_root + "dat.png";
        new_mask_path = save_root + "dat";
        if (FileExist(new_mask_path))
            remove(new_mask_path.c_str());
        if (rename(old_mask_path.c_str(), new_mask_path.c_str()))
            return;

        std::string label_path = save_root + "txt";
        std::ofstream ofs_results(label_path, std::ios::binary);

		sort(_effective_id.begin(), _effective_id.end());
        for (auto idx : _effective_id)
        {
            changeID(idx);
			if (_color.class_name == "Background")
				idx = 0;
            ofs_results << idx << " " << _color.class_name.toStdString() << "\n";
        }
        ofs_results.close();
    }
    else
        return;
}

void ISAT::read()
{
    std::string save_root = _imgFile.toLocal8Bit();
    save_root = save_root.substr(0, save_root.find_last_of(".") + 1);

    if (FileExist(save_root+"mask") && FileExist(save_root+"dat") && FileExist(save_root+"txt"))
    {
        _is_watershed = true;

        std::ifstream label;
        label.open(save_root + "txt");

        std::vector<std::string> lines;
        if (label.is_open())        
            for (std::string line; label >> line;) lines.push_back(line);

        for (int idx = 0; idx < lines.size() / 2; idx++)
        {
            int id = atoi(lines[2*idx].c_str());
            QString class_name = QString::fromStdString(lines[2*idx+1]);

			if (class_name == "Background")
				id = 255;
			
            createID(id, class_name);
            _effective_id.emplace_back(id);

            changeID(id);
        }

        cv::Mat input_img = qImage2Mat(_inputImg);

        cv::Mat watershed_mask = cv::imread(save_root + "mask");
        if (input_img.size() != watershed_mask.size())
            cv::resize(watershed_mask, watershed_mask, input_img.size(), 0, 0, cv::INTER_NEAREST);
        _watershed.id = mat2QImage(watershed_mask);
        idToColor(_watershed);
        _init_watershed = _watershed;

        cv::Mat drawing_mask = cv::imread(save_root + "dat");
        if (input_img.size() != drawing_mask.size())
            cv::resize(drawing_mask, drawing_mask, input_img.size(), 0, 0, cv::INTER_NEAREST);
        _mask.id = mat2QImage(drawing_mask);
        idToColor(_mask);

        update_mask();
    }
}