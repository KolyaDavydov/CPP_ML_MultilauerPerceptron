/********************************************************************************
** Form generated from reading UI file 'view.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_H
#define UI_VIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *pushButton_open_dataset;
    QPushButton *pushButton_open_model;
    QRadioButton *matrix_model;
    QRadioButton *graph_model;
    QLabel *label_model;
    QPushButton *pushButton_open_bmp;
    QLabel *slider_dataset_part;
    QSlider *test_part;
    QPushButton *pushButton_save_model;
    QWidget *layoutWidget;
    QHBoxLayout *layers_layout;
    QLabel *label_hiden_layers;
    QSpinBox *hiden_layers_number;
    QWidget *layoutWidget1;
    QHBoxLayout *epoch_layout;
    QLabel *label_epoch;
    QSpinBox *epoch_number;
    QGraphicsView *graphicsView;
    QPushButton *pushButton_train_model;
    QPushButton *pushButton_test_model;
    QPushButton *pushButton_recognize_symbol;
    QPushButton *pushButton_clear_paint;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1020, 800);
        MainWindow->setStyleSheet(QString::fromUtf8("background-color: rgb(80, 80, 80);"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        pushButton_open_dataset = new QPushButton(centralwidget);
        pushButton_open_dataset->setObjectName(QString::fromUtf8("pushButton_open_dataset"));
        pushButton_open_dataset->setGeometry(QRect(850, 100, 101, 30));
        pushButton_open_dataset->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  color: white;\n"
"  background-color: rgb(200, 150, 50);\n"
"  border: 1px solid black;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: rgb(150, 100, 0);\n"
"}"));
        pushButton_open_model = new QPushButton(centralwidget);
        pushButton_open_model->setObjectName(QString::fromUtf8("pushButton_open_model"));
        pushButton_open_model->setGeometry(QRect(850, 20, 101, 30));
        pushButton_open_model->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  color: white;\n"
"  background-color: rgb(200, 150, 50);\n"
"  border: 1px solid black;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: rgb(150, 100, 0);\n"
"}"));
        matrix_model = new QRadioButton(centralwidget);
        matrix_model->setObjectName(QString::fromUtf8("matrix_model"));
        matrix_model->setGeometry(QRect(850, 280, 99, 20));
        graph_model = new QRadioButton(centralwidget);
        graph_model->setObjectName(QString::fromUtf8("graph_model"));
        graph_model->setGeometry(QRect(850, 310, 99, 20));
        label_model = new QLabel(centralwidget);
        label_model->setObjectName(QString::fromUtf8("label_model"));
        label_model->setGeometry(QRect(870, 250, 60, 30));
        label_model->setStyleSheet(QString::fromUtf8("color: white;"));
        pushButton_open_bmp = new QPushButton(centralwidget);
        pushButton_open_bmp->setObjectName(QString::fromUtf8("pushButton_open_bmp"));
        pushButton_open_bmp->setGeometry(QRect(850, 140, 101, 30));
        pushButton_open_bmp->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  color: white;\n"
"  background-color: rgb(200, 150, 50);\n"
"  border: 1px solid black;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: rgb(150, 100, 0);\n"
"}"));
        slider_dataset_part = new QLabel(centralwidget);
        slider_dataset_part->setObjectName(QString::fromUtf8("slider_dataset_part"));
        slider_dataset_part->setGeometry(QRect(850, 180, 91, 30));
        slider_dataset_part->setStyleSheet(QString::fromUtf8("color: white;"));
        test_part = new QSlider(centralwidget);
        test_part->setObjectName(QString::fromUtf8("test_part"));
        test_part->setGeometry(QRect(839, 220, 131, 25));
        test_part->setMaximum(100);
        test_part->setValue(100);
        test_part->setSliderPosition(100);
        test_part->setOrientation(Qt::Horizontal);
        pushButton_save_model = new QPushButton(centralwidget);
        pushButton_save_model->setObjectName(QString::fromUtf8("pushButton_save_model"));
        pushButton_save_model->setGeometry(QRect(850, 60, 101, 30));
        pushButton_save_model->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  color: white;\n"
"  background-color: rgb(200, 150, 50);\n"
"  border: 1px solid black;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: rgb(150, 100, 0);\n"
"}"));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(840, 340, 121, 22));
        layers_layout = new QHBoxLayout(layoutWidget);
        layers_layout->setObjectName(QString::fromUtf8("layers_layout"));
        layers_layout->setContentsMargins(0, 0, 0, 0);
        label_hiden_layers = new QLabel(layoutWidget);
        label_hiden_layers->setObjectName(QString::fromUtf8("label_hiden_layers"));
        label_hiden_layers->setStyleSheet(QString::fromUtf8("color: white;"));

        layers_layout->addWidget(label_hiden_layers);

        hiden_layers_number = new QSpinBox(layoutWidget);
        hiden_layers_number->setObjectName(QString::fromUtf8("hiden_layers_number"));
        hiden_layers_number->setMinimum(2);
        hiden_layers_number->setMaximum(5);

        layers_layout->addWidget(hiden_layers_number, 0, Qt::AlignRight);

        layoutWidget1 = new QWidget(centralwidget);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(840, 380, 121, 22));
        epoch_layout = new QHBoxLayout(layoutWidget1);
        epoch_layout->setObjectName(QString::fromUtf8("epoch_layout"));
        epoch_layout->setContentsMargins(0, 0, 0, 0);
        label_epoch = new QLabel(layoutWidget1);
        label_epoch->setObjectName(QString::fromUtf8("label_epoch"));
        label_epoch->setStyleSheet(QString::fromUtf8("color: white;"));

        epoch_layout->addWidget(label_epoch);

        epoch_number = new QSpinBox(layoutWidget1);
        epoch_number->setObjectName(QString::fromUtf8("epoch_number"));
        epoch_number->setMinimum(2);
        epoch_number->setMaximum(5);

        epoch_layout->addWidget(epoch_number, 0, Qt::AlignRight);

        graphicsView = new QGraphicsView(centralwidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setEnabled(true);
        graphicsView->setGeometry(QRect(160, 70, 512, 512));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(graphicsView->sizePolicy().hasHeightForWidth());
        graphicsView->setSizePolicy(sizePolicy);
        graphicsView->setMaximumSize(QSize(512, 512));
        graphicsView->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pushButton_train_model = new QPushButton(centralwidget);
        pushButton_train_model->setObjectName(QString::fromUtf8("pushButton_train_model"));
        pushButton_train_model->setGeometry(QRect(850, 430, 101, 30));
        pushButton_train_model->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  color: white;\n"
"  background-color: rgb(200, 150, 50);\n"
"  border: 1px solid black;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: rgb(150, 100, 0);\n"
"}"));
        pushButton_test_model = new QPushButton(centralwidget);
        pushButton_test_model->setObjectName(QString::fromUtf8("pushButton_test_model"));
        pushButton_test_model->setGeometry(QRect(850, 480, 101, 30));
        pushButton_test_model->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  color: white;\n"
"  background-color: rgb(200, 150, 50);\n"
"  border: 1px solid black;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: rgb(150, 100, 0);\n"
"}"));
        pushButton_recognize_symbol = new QPushButton(centralwidget);
        pushButton_recognize_symbol->setObjectName(QString::fromUtf8("pushButton_recognize_symbol"));
        pushButton_recognize_symbol->setGeometry(QRect(850, 530, 101, 30));
        pushButton_recognize_symbol->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  color: white;\n"
"  background-color: rgb(200, 150, 50);\n"
"  border: 1px solid black;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: rgb(150, 100, 0);\n"
"}"));
        pushButton_clear_paint = new QPushButton(centralwidget);
        pushButton_clear_paint->setObjectName(QString::fromUtf8("pushButton_clear_paint"));
        pushButton_clear_paint->setGeometry(QRect(850, 570, 101, 30));
        pushButton_clear_paint->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  color: white;\n"
"  background-color: rgb(200, 150, 50);\n"
"  border: 1px solid black;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: rgb(150, 100, 0);\n"
"}"));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Multilayer Perceptron", nullptr));
        pushButton_open_dataset->setText(QApplication::translate("MainWindow", "Open dataset", nullptr));
        pushButton_open_model->setText(QApplication::translate("MainWindow", "Open model", nullptr));
        matrix_model->setText(QApplication::translate("MainWindow", "matrix", nullptr));
        graph_model->setText(QApplication::translate("MainWindow", "graph", nullptr));
        label_model->setText(QApplication::translate("MainWindow", "Model:", nullptr));
        pushButton_open_bmp->setText(QApplication::translate("MainWindow", "Open BMP", nullptr));
        slider_dataset_part->setText(QApplication::translate("MainWindow", "Part of dataset:", nullptr));
        pushButton_save_model->setText(QApplication::translate("MainWindow", "Save model", nullptr));
        label_hiden_layers->setText(QApplication::translate("MainWindow", "Hiden layers:", nullptr));
        label_epoch->setText(QApplication::translate("MainWindow", "Epoch:", nullptr));
        pushButton_train_model->setText(QApplication::translate("MainWindow", "Train model", nullptr));
        pushButton_test_model->setText(QApplication::translate("MainWindow", "Test model", nullptr));
        pushButton_recognize_symbol->setText(QApplication::translate("MainWindow", "Recognize", nullptr));
        pushButton_clear_paint->setText(QApplication::translate("MainWindow", "Clear paint", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_H
