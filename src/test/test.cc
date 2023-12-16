#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

#include "../controller/controller.h"

TEST(Controller, noFile) {
  s21::MlpModel model;
  s21::MlpController controller(&model);
  controller.OpenModel("asd");
  EXPECT_EQ(controller.GetModelValid(), false);
}

TEST(Controller, matrixModel) {
  s21::MlpModel model;
  s21::MlpController controller(&model);
  controller.SetModelType(1);
  bool is_open = controller.OpenDataset("test/test_test_A.csv");
  EXPECT_EQ(is_open, true);
  controller.TrainModel(2, 2);
  bool is_save = controller.SaveModel("test/model_A.txt");
  EXPECT_EQ(is_save, true);
  bool is_open_model = controller.OpenModel("test/model_A.txt");
  EXPECT_EQ(is_open_model, true);
  char letter = controller.RecognizeImage("test/file_image.bmp");
  EXPECT_EQ(letter, 'A');
}

TEST(Controller, graphModel) {
  s21::MlpModel model;
  s21::MlpController controller(&model);
  controller.SetModelType(2);
  bool is_open = controller.OpenDataset("test/test_test_A.csv");
  EXPECT_EQ(is_open, true);
  controller.TrainModel(2, 2);
  bool is_save = controller.SaveModel("test/model_A.txt");
  EXPECT_EQ(is_save, true);
  bool is_open_model = controller.OpenModel("test/model_A.txt");
  EXPECT_EQ(is_open_model, true);
  char letter = controller.RecognizeImage("test/file_image.bmp");
  EXPECT_EQ(letter, 'A');
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}