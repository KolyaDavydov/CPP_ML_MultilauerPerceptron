#include "view.h"

#include "ui_view.h"

namespace s21 {

s21::MlpModel model;
s21::MlpController controller(&model);

MlpView::MlpView(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  scene = new paintScene();  // Инициализируем графическую сцену
  ui_->graphicsView->setScene(scene);  // Устанавливаем графическую сцену
  ui_->graphicsView->setSceneRect(scene->sceneRect());
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
  //  connect(ui_->pushButton_open_bmp, SIGNAL(clicked()), this,
  //          SLOT(OpenBmp()));
  connect(ui_->pushButton_train_model, SIGNAL(clicked()), this,
          SLOT(TrainModel()));
  connect(ui_->pushButton_test_model, SIGNAL(clicked()), this,
          SLOT(TestModel()));
  connect(ui_->pushButton_clear_paint, SIGNAL(clicked()), this,
          SLOT(ClearPaint()));
}

void MlpView::OpenModel() {
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Открыть модель"), QDir::homePath(), "М (*.txt)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();

    controller.openModel(file_name_);
  }
}

void MlpView::SaveModel() {
  QString file_name = QFileDialog::getSaveFileName(
      this, tr("Сохранить модель"), QDir::homePath(), "М (*.txt)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();

    controller.saveModel(file_name_);
  }
}
void MlpView::OpenDataset() {
  QString file_name = QFileDialog::getOpenFileName(
      this, tr("Открыть датасет"), QDir::homePath(), "М (*.csv)");
  if (!file_name.isEmpty() && !file_name.isNull()) {
    std::string file_name_ = file_name.toStdString();

    controller.openDataset(file_name_);
  }
}

void MlpView::TrainModel() {
  int epoch = ui_->epoch_number->value();
  int hiden_layers = ui_->hiden_layers_number->value();

  controller.trainModel(epoch, hiden_layers);
};

void MlpView::TestModel() {
  int test_part = ui_->test_part->value();
  controller.testModel(test_part);
};

void MlpView::ClearPaint() {
  scene->clear();
  ui_->graphicsView->setSceneRect(scene->sceneRect());
};

}  // namespace s21
