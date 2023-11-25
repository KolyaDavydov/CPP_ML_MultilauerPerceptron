#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QColor>
#include <QColorDialog>
#include <QFileDialog>
#include <QMainWindow>
#include <QThread>
#include <QTime>

#include "../controller/controller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
namespace s21 {
class MlpView : public QMainWindow {
  Q_OBJECT

 public:
  MlpView(QWidget *parent = nullptr);
  ~MlpView();

 protected:
 private slots:
  void OpenModel();
  void SaveModel();
  void OpenDataset();
  //  void OpenBmp();
  void TrainModel();

 private:
  Ui::MainWindow *ui_;
  // MlpModel *model_;
  void SetupButtons();
};
}  // namespace s21

#endif  // MAINWINDOW_H
