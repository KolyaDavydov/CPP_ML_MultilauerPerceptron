#include "view.h"
#include "ui_view.h"

namespace s21 {

s21::MlpModel model;
s21::MlpController controller(&model);

MlpView::MlpView(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  SetupCharts();
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
  connect(ui_->pushButton_cross_validation, SIGNAL(clicked()), this,
          SLOT(CrossValidation()));
  connect(ui_->radioButton_graph_model, SIGNAL(clicked()), this,
          SLOT(SetModelType()));
  connect(ui_->radioButton_matrix_model, SIGNAL(clicked()), this,
          SLOT(SetModelType()));
}

void MlpView::SetupCharts() {
  scene = new paintScene();
  ui_->graphicsView->setScene(scene);
  ui_->graphicsView->setSceneRect(scene->sceneRect());
  scene->setSceneRect(0, 0, ui_->graphicsView->width(),
                      ui_->graphicsView->height());
  QChart *chart = new QChart();
  chart->setAnimationOptions(QChart::AnimationOption::AllAnimations);
  chart->legend()->hide();
  chart->createDefaultAxes();
  chart->setBackgroundBrush(QBrush(QColor("black")));
  // chart->setTitle("Test errors by epoch");
  ui_->chartView->setChart(chart);
}

void MlpView::PlotChart(std::vector<double> series_vec) {
  QLineSeries* series = new QLineSeries();
  int i = 0;
  for (auto var : series_vec) {
    series->append(i, var);
    i++;
  }
  QChart *chart = new QChart();
  chart->setAnimationOptions(QChart::AnimationOption::AllAnimations);
  chart->legend()->hide();
  series->setColor(QColor(255, 255, 255));
  chart->addSeries(series);
  chart->createDefaultAxes();
  chart->setBackgroundBrush(QBrush(QColor("black")));
  ui_->chartView->setChart(chart);
  ui_->chartView->setRenderHint(QPainter::Antialiasing);
  ui_->chartView->show();
}

void MlpView::SetModelType() {
  int model_type;
  if (ui_->radioButton_matrix_model->isChecked()) {
    model_type = MATRIX_MODEL;
  } else {
    model_type = GRAPH_MODEL;
  }
  controller.SetModelType(model_type);
}

void MlpView::OpenModel() {
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Open model"), QDir::homePath(), "TXT-files (*.txt)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();

    if (!controller.OpenModel(file_name_)) {
      QMessageBox msgBox;
      msgBox.setText("Cannot open model file");
      msgBox.exec();
    };
  }
}

void MlpView::SaveModel() {
  QString file_name = QFileDialog::getSaveFileName(
      this, tr("Save model"), QDir::homePath(), "TXT-files (*.txt)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();
    controller.SaveModel(file_name_);
  }
}

void MlpView::OpenDataset() {
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Open dataset"), QDir::homePath(), "Datasets (*.csv)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();
    controller.OpenDataset(file_name_);
  }
}

void MlpView::OpenTestDataset() {
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Open test dataset"), QDir::homePath(), "Datasets (*.csv)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();
    controller.OpenTestDataset(file_name_);
  }
}

void MlpView::OpenBmp() {
  QImage image;
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Open BMP-file"), QDir::homePath(), "BMP-files (*.bmp)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();
    image.load(file_name_.c_str());
    if (image.height() < 28 || image.width() < 28 || image.height() > 512 ||
        image.width() > 512) {
      QMessageBox msgBox;
      msgBox.setText("Too small or too large image");
      msgBox.exec();
    } else {
      QImage grayscale = image.convertToFormat(QImage::Format_Grayscale8);
      QImage small = grayscale.scaled(28, 28, Qt::KeepAspectRatio);
      stringstream letter;
      letter << "0";
      for (int x = 0; x < 28; x++) {
        for (int y = 0; y < 28; y++) {
          QColor pixColor = small.pixelColor(x, y);
          int red = pixColor.red();
          letter << "," << red;
        }
      }
      char recognizedletter = controller.RecognizeImage(letter.str());
      error_msg_ = controller.GetErrorMsg();
      if (error_msg_ != "") {
        QMessageBox msgBox;
        msgBox.setText(error_msg_);
        msgBox.exec();
      } else {
        ui_->label_recognized_letter->setText(
            QString("%1").arg(recognizedletter));
        ui_->label_recognized_letter->repaint();
        scene->clear();
        scene->addPixmap(QPixmap::fromImage(grayscale));
        ui_->graphicsView->setScene(scene);
      }
    }
  }
}

void MlpView::UpdateLabel() {
  std::vector<double> train_errors = controller.GetTrainErrors();
  ui_->label_test_epoch_val->setText(QString("%1").arg(train_errors.size()));
  ui_->label_test_error_val->setText(QString("%1").arg(
      QString::number(train_errors[train_errors.size() - 1], 'f', 1)));
  ui_->label_test_error_val->repaint();
}

void MlpView::TrainModel() {
  int epoch = ui_->epoch_number->value();
  int hiden_layers = ui_->hiden_layers_number->value();
  controller.TrainModel(epoch, hiden_layers);
  error_msg_ = controller.GetErrorMsg();
  if (error_msg_ != "") {
    QMessageBox msgBox;
    msgBox.setText(error_msg_);
    msgBox.exec();
  } else {
    if (controller.GetModelValid()) {
      UpdateLabel();
      std::vector<double> train_errors = controller.GetTrainErrors();
      PlotChart(train_errors);
    }
  }
  // int count_epoch = 1;
  // while (count_epoch != epoch) {
  //   std::vector<double> train_errors = controller.getTrainErrors();
  //   ui_->label_test_epoch_val->setText(QString("%1").arg(count_epoch));
  //   ui_->label_test_error_val->setText(
  //       QString("%1").arg(train_errors[train_errors.size()]));
  //   ui_->label_test_error_val->repaint();
  //   count_epoch = train_errors.size();
  // }
};

void MlpView::TestModel() {
  int test_part = ui_->test_part->value();
  testResults testRes = controller.TestModel(test_part);
  error_msg_ = controller.GetErrorMsg();
  if (error_msg_ != "") {
    QMessageBox msgBox;
    msgBox.setText(error_msg_);
    msgBox.exec();
  } else {
    QString res =
        "Test results\nAccuracy:\t" +
        QString::number(testRes.accuracy, 'f', 1) + " %\nPrecision:\t" +
        QString::number(testRes.precision * 100, 'f', 1) + " %\nRecall:\t" +
        QString::number(testRes.recall * 100, 'f', 1) + " %\nF-measure:\t" +
        QString::number(testRes.fmeasure * 100, 'f', 1) + " %\nTest time:\t" +
        QString::number(testRes.runtime / 1000, 'f', 1) + " s";
    QMessageBox msgBox;
    msgBox.setText(res);
    msgBox.exec();
  }
};

void MlpView::RecognizeImage() {
  QPixmap pixMap =
      ui_->graphicsView->grab(ui_->graphicsView->sceneRect().toRect());
  QImage image = pixMap.scaled(28, 28).toImage();
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
  char recognizedletter = controller.RecognizeImage(letter.str());
  error_msg_ = controller.GetErrorMsg();
  if (error_msg_ != "") {
    QMessageBox msgBox;
    msgBox.setText(error_msg_);
    msgBox.exec();
  } else {
    ui_->label_recognized_letter->setText(QString("%1").arg(recognizedletter));
    ui_->label_recognized_letter->repaint();
  }
}

void MlpView::ClearPaint() {
  scene->clear();
  ui_->graphicsView->setSceneRect(scene->sceneRect());
};

void MlpView::CrossValidation() {
  int k_value = ui_->k_value->value();
  int epoch = ui_->epoch_number->value();
  int hiden_layers = ui_->hiden_layers_number->value();
  // вектор куда сохраняются значения тестовых частей для k-групп
  std::vector<testResults> crossResultstest =
      controller.CrossValidation(k_value, epoch, hiden_layers);
  QString res = "";
  for (int i = 0; i < k_value; ++i) {
    res += "\n       Number of k group: " + QString::number(i + 1) + "\n"
      "Test results\nAccuracy:\t" + QString::number(crossResultstest[i].accuracy, 'f', 1) +
      " %\nPrecision:\t" + QString::number(crossResultstest[i].precision * 100, 'f', 1) +
      " %\nRecall:\t" + QString::number(crossResultstest[i].recall * 100, 'f', 1) +
      " %\nF-measure:\t" + QString::number(crossResultstest[i].fmeasure * 100, 'f', 1);   
  }
  // testResults testRes = controller.testModel(test_part);
  // QString res =
  //     "Test results\nAccuracy:\t" + QString::number(testRes.accuracy, 'f', 1) +
  //     " %\nPrecision:\t" + QString::number(testRes.precision * 100, 'f', 1) +
  //     " %\nRecall:\t" + QString::number(testRes.recall * 100, 'f', 1) +
  //     " %\nF-measure:\t" + QString::number(testRes.fmeasure * 100, 'f', 1) +
  //     " %\nTest time:\t" + QString::number(testRes.runtime / 1000, 'f', 1) +
  //     " s";
  QMessageBox msgBox;
  msgBox.setText(res);
  msgBox.exec();
};

}  // namespace s21
