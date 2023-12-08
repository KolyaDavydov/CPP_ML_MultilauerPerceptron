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

 private:
  Ui::MainWindow *ui_;
  // MlpModel *model_;
  void SetupButtons();
  void updateLabel();
  paintScene *scene;  // Объявляем кастомную графическую сцену
};
}  // namespace s21

#endif  // VIEW_H
