#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  setlocale(LC_NUMERIC, "C");
  s21::MlpModel model;
  s21::MlpController controller(&model);
  s21::MlpView w;
  w.show();
  return a.exec();
}