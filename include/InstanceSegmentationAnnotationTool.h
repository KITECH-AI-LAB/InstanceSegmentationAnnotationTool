#pragma once

#include <QMainWindow>
#include <QDebug>
#include <QtCore>
#include <QtWidgets>
#include <QShortcut>
#include <QKeyEvent>
#include "ui_InstanceSegmentationAnnotationTool.h"

#include "utils.h"
#include "ISAT.h"

struct init_param {
    bool is_set_dir;
    bool is_set_class;
    bool is_init;
};

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void btn_prev_clicked();
    void btn_next_clicked();
    void slider_value_changed();
    void btn_up_clicked();
    void btn_down_clicked();
    void btn_watershed_clicked();
    void btn_save_clicked();

    void btn_img_dir_clicked();
    void btn_set_class_clicked();
    void set_pen_size();
    void change_manual();
    void change_watershed();
    void change_visualize_mode();
    void btn_plus_clicked();
    void btn_minus_clicked();

    void undo();
    void redo();

private:
    void init();
    void check_is_init();
    void set_class_tree();
    void update_id();
    void move_img();
    void update_img();
    
    Ui::MainWindow  *ui;
    init_param      init_parameter;
    ISAT            canvas;
    
protected:
    void show_msgBox(QMessageBox::Icon icon, QString title, QString content);
    void wheelEvent(QWheelEvent *ev);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
};
