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

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}