#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include "mainwindow.h"
#include <QString>
#include <QFile>

void MainWindow::set_rec_opened_butts()
{
    QFile file;
    file.setFileName("D:\\University\\cs\\sem3\\cursach\\photored\\recently_opened.json");
    file.open(QIODevice::ReadOnly);
    QString s;
    s = file.readAll();
    file.close();
    QJsonDocument d;
    d = QJsonDocument::fromJson(s.toUtf8());
    QJsonArray pathes = d.array();
    QJsonValue path;
    int n = pathes.size();
    for (int i = 0; i < n; i++)
    {
        path = pathes[i];
        s = path.toString();
        QPixmap im(s);
        QIcon but_im(im);
        switch (i)
        {
        case 0:
            pushButton_Rec_open_1->set_image_path(s);
            pushButton_Rec_open_1->setIcon(but_im);
            pushButton_Rec_open_1->show();
            break;
        case 1:
            pushButton_Rec_open_2->set_image_path(s);
            pushButton_Rec_open_2->setIcon(but_im);
            pushButton_Rec_open_2->show();
            break;
        case 2:
            pushButton_Rec_open_3->set_image_path(s);
            pushButton_Rec_open_3->setIcon(but_im);
            pushButton_Rec_open_3->show();
            break;
        case 3:
            pushButton_Rec_open_4->set_image_path(s);
            pushButton_Rec_open_4->setIcon(but_im);
            pushButton_Rec_open_4->show();
            break;
        case 4:
            pushButton_Rec_open_5->set_image_path(s);
            pushButton_Rec_open_5->setIcon(but_im);
            pushButton_Rec_open_5->show();
            break;
        }
    }
}

void MainWindow::set_curr_proc(PROCESSES n)
{
    current_process = n;
}

PROCESSES MainWindow::get_curr_proc()
{
    return current_process;
}

void MainWindow::set_slider_limits()
{
    PROCESSES n = get_curr_proc();
    regulation->setMinimum(-100);
    regulation->setMaximum(100);
    switch (n)
    {
    case PROCESSES::BRIGHTNESS:
    {
        regulation->setValue(image_info.brightness);
        out_amount->setNum(image_info.brightness);
        break;
    }
    case PROCESSES::CONTRAST:
    {
        regulation->setValue(image_info.contrast);
        out_amount->setNum(image_info.contrast);
        break;
    }
    case PROCESSES::SATURATUIN:
    {
        regulation->setValue(image_info.saturation);
        out_amount->setNum(image_info.saturation);
        break;
    }
    case PROCESSES::CLARITY:
    {
        regulation->setValue(image_info.clarity);
        out_amount->setNum(image_info.clarity);
        break;
    }
    case PROCESSES::TEMPERATURE:
    {
        regulation->setValue(image_info.temperture);
        out_amount->setNum(image_info.temperture);
        break;
    }
    }
}

void MainWindow::prepare_image()
{
    QPixmap image = *(image_info.start_image);
    if (current_process != PROCESSES::BRIGHTNESS)
    {
        Oper_brightness oper(image_info.brightness, QtOcv::image2Mat(image.toImage()));
        image = QPixmap::fromImage(QtOcv::mat2Image(oper.exec()));
    }
    if (current_process != PROCESSES::CONTRAST)
    {
        Oper_contrast oper(image_info.contrast, QtOcv::image2Mat(image.toImage()));
        image = QPixmap::fromImage(QtOcv::mat2Image(oper.exec()));
    }
    if (current_process != PROCESSES::SATURATUIN)
    {
        Oper_saturation oper(image_info.saturation, QtOcv::image2Mat(image.toImage()));
        image = QPixmap::fromImage(QtOcv::mat2Image(oper.exec()));
    }
    if (current_process != PROCESSES::CLARITY)
    {
        Oper_clarity oper(image_info.clarity, QtOcv::image2Mat(image.toImage()));
        image = QPixmap::fromImage(QtOcv::mat2Image(oper.exec()));
    }
    if (current_process != PROCESSES::TEMPERATURE)
    {
        Oper_temperature oper(image_info.temperture, QtOcv::image2Mat(image.toImage()));
        image = QPixmap::fromImage(QtOcv::mat2Image(oper.exec()));
    }
    if (image_info.filter == FILTER::INVERSE)
    {
        Inverse filt(QtOcv::image2Mat(image.toImage()));
        image = QPixmap::fromImage(QtOcv::mat2Image(filt.apply()));
    }
    else if (image_info.filter == FILTER::GRAY)
    {
        Gray filt(QtOcv::image2Mat(image.toImage()));
        image = QPixmap::fromImage(QtOcv::mat2Image(filt.apply()));
    }
    (*image_info.image_in_proc) = image;
}

void MainWindow::set_filter_number(int n)
{
    filter_number = n;
}

void MainWindow::set_filters()
{
    pushButton_deleteF->setEnabled(false);
    FILTER temp = image_info.filter;
    image_info.filter = FILTER::CUSTOM;
    prepare_image();
    image_info.filter = temp;
    filters.clean();
    Mat image = QtOcv::image2Mat(image_info.image_in_proc->toImage());
    Mat start_image = QtOcv::image2Mat(image_info.start_image->toImage());
    filters.push(new Inverse(image));
    filters.push(new Original(start_image.clone()));
    filters.push(new Gray(image));
    QFile file;
    file.setFileName("D:\\University\\cs\\sem3\\cursach\\photored\\filters_inform.json");
    file.open(QIODevice::ReadOnly);
    QString s;
    s = file.readAll();
    file.close();
    QJsonDocument d;
    d = QJsonDocument::fromJson(s.toUtf8());
    QJsonArray js_filters = d.array();
    QJsonObject js_filter;
    std::string filter_name;
    int br, co, st, cl, tmp;
    for (int i = 0; i < js_filters.size(); i++)
    {
        js_filter = js_filters[i].toObject();
        filter_name = js_filter["name"].toString().toStdString();
        br = js_filter["brightness"].toInt();
        co = js_filter["contrast"].toInt();
        st = js_filter["saturation"].toInt();
        cl = js_filter["clarity"].toInt();
        tmp = js_filter["temperature"].toInt();
        filters.push(new CustomFilter(filter_name, start_image.clone(), br, co, st, cl, tmp));
    }
}

void MainWindow::save_filters()
{
    QFile file;
    file.setFileName("D:\\University\\cs\\sem3\\cursach\\photored\\filters_inform.json");
    file.open(QIODevice::WriteOnly);
    file.resize(0);
    QJsonObject js_filter;
    QJsonArray js_filters;
    QJsonDocument d;
    for (int i = 0; i < filters.size(); i++)
    {
        std::string filter_name = filters[i]->get_filter_name();
        if (filter_name != "Inverse" && filter_name != "Original" && filter_name != "Gray")
        {
            js_filter["name"] = QString::fromStdString(filter_name);
            js_filter["brightness"] = filters[i]->get_brightness();
            js_filter["contrast"] = filters[i]->get_contrast();
            js_filter["saturation"] = filters[i]->get_saturation();
            js_filter["clarity"] = filters[i]->get_clarity();
            js_filter["temperature"] = filters[i]->get_temperature();
            js_filters.push_back(js_filter);
        }
    }
    d.setArray(js_filters);
    QString s = d.toJson();
    file.write(s.toUtf8());
    file.close();
}

void MainWindow::set_filters_buttons()
{
    QIcon leftF(QPixmap::fromImage(QtOcv::mat2Image(filters[0]->apply())));
    pushButton_leftF->setIcon(leftF);
    pushButton_leftF->setIconSize(pushButton_leftF->icon().actualSize(QSize(pushButton_leftF->size().width() - 10, pushButton_leftF->size().height() - 10)));
    label_leftFname->setText(QString::fromStdString(filters[0]->get_filter_name()));
    QIcon centerF(QPixmap::fromImage(QtOcv::mat2Image(filters[1]->apply())));
    pushButton_centerF->setIcon(centerF);
    pushButton_centerF->setIconSize(pushButton_centerF->icon().actualSize(QSize(pushButton_centerF->size().width() - 10, pushButton_centerF->size().height() - 10)));
    label_centerFname->setText(QString::fromStdString(filters[1]->get_filter_name()));
    QIcon rightF(QPixmap::fromImage(QtOcv::mat2Image(filters[2]->apply())));
    pushButton_rightF->setIcon(rightF);
    pushButton_rightF->setIconSize(pushButton_rightF->icon().actualSize(QSize(pushButton_rightF->size().width() - 10, pushButton_rightF->size().height() - 10)));
    label_rightFname->setText(QString::fromStdString(filters[2]->get_filter_name()));
}

void MainWindow::next_prev_filter(int n)
{
    if (n > 0)
        filters.next_node();
    if (n < 0)
        filters.prev_node();
}

Mat MainWindow::get_filtered_image(int n)
{
    return filters[n]->apply();
}

std::string MainWindow::get_filter_name(int n)
{
    return filters[n]->get_filter_name();
}

void MainWindow::set_deleteF_enabled(std::string filter_name)
{
    if (filter_name == "Inverse" || filter_name == "Original" || filter_name == "Gray")
        pushButton_deleteF->setEnabled(false);
    else
        pushButton_deleteF->setEnabled(true);
}

void MainWindow::back_from_filters()
{
    prepare_image();
    change_image(QtOcv::image2Mat((*image_info.image_in_proc).toImage()));
    save_filters();
    filters.clean();
    pushButton_toLeft->hide();
    pushButton_leftF->hide();
    label_leftFname->hide();
    pushButton_centerF->hide();
    label_centerFname->hide();
    pushButton_rightF->hide();
    label_rightFname->hide();
    pushButton_toRight->hide();
    pushButton_addF->hide();
    pushButton_deleteF->hide();
    pushButton_back->hide();
    pushButton_applyFilter->hide();
    pushButton_brightness->show();
    pushButton_contrast->show();
    pushButton_saturation->show();
    pushButton_clarity->show();
    pushButton_temperature->show();
    pushButton_crop->show();
    pushButton_filters->show();
    pushButton_left->show();
    pushButton_right->show();
}

void MainWindow::delete_filter()
{
    switch (filter_number)
    {
    case 0:
    {
        filters.pop_head();
        break;
    }
    case 1:
    {
        filters.next_node();
        filters.pop_head();
        filters.prev_node();
        break;
    }
    case 2:
    {
        filters.next_node();
        filters.next_node();
        filters.pop_head();
        filters.prev_node();
        filters.prev_node();
        break;
    }
    }
    set_filters_buttons();
}

void MainWindow::show_pressed_button()
{
    QString pressed = "background-color: gray";
    QString not_pressed = "";
    if (current_process == PROCESSES::BRIGHTNESS)
        pushButton_brightness->setStyleSheet(pressed);
    else
        pushButton_brightness->setStyleSheet(not_pressed);
    if (current_process == PROCESSES::CONTRAST)
        pushButton_contrast->setStyleSheet(pressed);
    else
        pushButton_contrast->setStyleSheet(not_pressed);
    if (current_process == PROCESSES::SATURATUIN)
        pushButton_saturation->setStyleSheet(pressed);
    else
        pushButton_saturation->setStyleSheet(not_pressed);
    if (current_process == PROCESSES::CLARITY)
        pushButton_clarity->setStyleSheet(pressed);
    else
        pushButton_clarity->setStyleSheet(not_pressed);
    if (current_process == PROCESSES::TEMPERATURE)
        pushButton_temperature->setStyleSheet(pressed);
    else
        pushButton_temperature->setStyleSheet(not_pressed);
}
