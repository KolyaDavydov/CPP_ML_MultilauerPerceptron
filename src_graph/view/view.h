#ifndef VIEW_H
#define VIEW_H

#include <QFileDialog>
#include <QMainWindow>

#include "../controller/controller.h"
#include "paintscene.h"

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
  void OpenTestDataset();
  void OpenBmp();
  void TrainModel();
  void TestModel();
  void RecognizeImage();
  void ClearPaint();
  void CrossValidation();
  void SetModelType();

 private:
  Ui::MainWindow *ui_;
  void SetupButtons();
  void SetupCharts();
  void PlotChart(std::vector<double> series_val);
  void UpdateLabel();
  paintScene *scene;  // Объявляем кастомную графическую сцену
};
}  // namespace s21

#endif  // VIEW_H
