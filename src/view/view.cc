#include "view.h"

#include <QMessageBox>
#include <iostream>

#include "ui_view.h"

namespace s21 {

void delay(int millisecondsToWait) {
  QTime dieTime = QTime::currentTime().addMSecs(millisecondsToWait);
  while (QTime::currentTime() < dieTime) {
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
  }
}

s21::MlpModel model;
s21::MlpController controller(&model);

MlpView::MlpView(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  scene = new paintScene();  // Инициализируем графическую сцену
  ui_->graphicsView->setScene(scene);  // Устанавливаем графическую сцену
  ui_->graphicsView->setSceneRect(scene->sceneRect());
  scene->setSceneRect(0, 0, ui_->graphicsView->width(),
                      ui_->graphicsView->height());
  SetupButtons();
}

MlpView::~MlpView() { delete ui_; }

void MlpView::SetupButtons() {
  connect(ui_->pushButton_open_model, SIGNAL(clicked()), this,
          SLOT(OpenModel()));
  connect(ui_->pushButton_save_model, SIGNAL(clicked()), this,
          SLOT(SaveModel()));
  connect(ui_->pushButton_open_dataset, SIGNAL(clicked()), this,
          SLOT(OpenDataset()));
  connect(ui_->pushButton_open_test_dataset, SIGNAL(clicked()), this,
          SLOT(OpenTestDataset()));
  connect(ui_->pushButton_open_bmp, SIGNAL(clicked()), this, SLOT(OpenBmp()));
  connect(ui_->pushButton_train_model, SIGNAL(clicked()), this,
          SLOT(TrainModel()));
  connect(ui_->pushButton_recognize_image, SIGNAL(clicked()), this,
          SLOT(RecognizeImage()));
  connect(ui_->pushButton_test_model, SIGNAL(clicked()), this,
          SLOT(TestModel()));
  connect(ui_->pushButton_clear_paint, SIGNAL(clicked()), this,
          SLOT(ClearPaint()));
}

void MlpView::OpenModel() {
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Открыть модель"), QDir::homePath(), "TXT-files (*.txt)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();

    controller.openModel(file_name_);
  }
}

void MlpView::SaveModel() {
  QString file_name = QFileDialog::getSaveFileName(
      this, tr("Сохранить модель"), QDir::homePath(), "TXT-files (*.txt)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();

    controller.saveModel(file_name_);
  }
}
void MlpView::OpenDataset() {
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Открыть датасет"), QDir::homePath(), "Datasets (*.csv)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();

    controller.openDataset(file_name_);
  }
}

void MlpView::OpenTestDataset() {
  QString file_name =
      QFileDialog::getOpenFileName(this, tr("Открыть тестовый датасет"),
                                   QDir::homePath(), "Datasets (*.csv)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();

    controller.openTestDataset(file_name_);
  }
}

void MlpView::OpenBmp() {
  QImage image;
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Открыть датасет"), QDir::homePath(), "BMP-files (*.bmp)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();
    image.load(file_name_.c_str());
    if (image.height() < 28 || image.width() < 28 || image.height() > 512 ||
        image.width() > 512)
      std::cout << "Too small or too large image" << std::endl;
    else {
      QImage grayscale = image.convertToFormat(QImage::Format_Grayscale8);
      QImage small = grayscale.scaled(28, 28, Qt::KeepAspectRatio);
      stringstream letter;
      letter << "0";
      for (int x = 0; x < 28; x++) {
        for (int y = 0; y < 28; y++) {
          QColor pixColor = grayscale.pixelColor(x, y);
          int red = pixColor.red();
          letter << "," << red;
        }
      }
      char recognizedletter = controller.recognizeImage(letter.str());
      ui_->label_recognized_letter->setText(
          QString("%1").arg(recognizedletter));
      ui_->label_recognized_letter->repaint();
      scene->clear();
      scene->addPixmap(QPixmap::fromImage(grayscale));
      ui_->graphicsView->setScene(scene);
    }
  }
}

void MlpView::TrainModel() {
  int epoch = ui_->epoch_number->value();
  int hiden_layers = ui_->hiden_layers_number->value();
  controller.trainModel(epoch, hiden_layers);
};

void MlpView::TestModel() {
  int test_part = ui_->test_part->value();
  testResults testRes = controller.testModel(test_part);
  QString res =
      "Test results\nAccuracy: " + QString::number(testRes.accuracy, 'f', 1) +
      " %\nPrecision: " + QString::number(testRes.precision * 100, 'f', 1) +
      " %\nRecall: " + QString::number(testRes.recall * 100, 'f', 1) +
      " %\nF-measure: " + QString::number(testRes.fmeasure * 100, 'f', 1) +
      " %\nRunning time: " + QString::number(testRes.runtime / 1000, 'f', 1) +
      " s";
  QMessageBox msgBox;
  msgBox.setText(res);
  msgBox.exec();
};

void MlpView::RecognizeImage() {
  QPixmap pixMap =
      ui_->graphicsView->grab(ui_->graphicsView->sceneRect().toRect());

  // pixMap.scaled(28, 28).save("/Users/nohoteth/mlp/file_pix.bmp");
  QImage image = pixMap.scaled(28, 28).toImage();
  // QImage grayscale = image.convertToFormat(QImage::Format_Grayscale8);
  stringstream letter;
  letter << "0";
  for (int x = 0; x < 28; x++) {
    for (int y = 0; y < 28; y++) {
      QColor pixColor = image.pixelColor(x, y);
      int red = pixColor.red();  // пока работаем с тем что нарисовано все цвета
                                 // одинаковые, проверить при загрузке
      // int green = pixColor.green();
      // int blue = pixColor.blue();
      letter << "," << red;
    }
  }
  char recognizedletter = controller.recognizeImage(letter.str());
  ui_->label_recognized_letter->setText(QString("%1").arg(recognizedletter));
  ui_->label_recognized_letter->repaint();

  // ui_->label_recognized_letter->setText(recognizedletter);
  // std::cout << letter.str() << endl;
}

void MlpView::ClearPaint() {
  scene->clear();
  ui_->graphicsView->setSceneRect(scene->sceneRect());
};

}  // namespace s21
