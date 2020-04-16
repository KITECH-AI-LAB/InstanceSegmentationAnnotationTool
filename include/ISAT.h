#pragma once
#include <QLabel>
#include <QPainter>
#include "utils.h"

struct ColorMask
{
    QColor  id;
    QColor  color;
    QString class_name;
};

struct ImageMask 
{
    QImage  id;
    QImage  color;

    ImageMask();
    ImageMask(QSize s);

    void drawFillCircle(int x, int y, int pen_size, ColorMask cm, QImage& image);
    void drawPixel(int x, int y, ColorMask cm, QImage& image);
};

class ISAT : public QLabel
{
    Q_OBJECT

public:
    ISAT(QWidget *parent = nullptr);
    
    QString                 _imgFile;
    QStringList             _imgList;
    int                     _imgIndex;
    QStringList             _classList;
    int                     _pen_size;
    bool                    _mouse_is_pressed;
    QPoint                  _mouse_pos;
    bool                    _display_manual_mask;
    bool                    _display_watershed_mask;
    bool                    _draw_manual_mask;

    QImage                  _inputImg;
    cv::Size                _inputImg_size;
    ColorMask               _color;
    ImageMask               _mask;
    ImageMask               _watershed;
    bool                    _is_watershed;
    QImage                  _inputImg_display;
    std::vector<ColorMask>  _id_storage;
    std::vector<int>        _effective_id;
    
    std::vector<ImageMask>  _undo_list;
    int                     _undo_idx;
    
    void readImage();
    bool checkDuplication(int id, QColor color);
    void createID(int& id, QColor& color, QString class_name);
    void createID(int id, QString class_name);
    void changeID(int id);
    void checkID();
    void draw(QMouseEvent *e, QRect size);
    void update_mask();
    void run_watershed();
    void idToColor(ImageMask& mask);
    void save();
    void read();
};