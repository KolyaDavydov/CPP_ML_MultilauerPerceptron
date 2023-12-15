#include <gtest/gtest.h>

#include <cmath>
#include <iostream>

#include "../controller/controller.h"

TEST(Controller, noFile) {
  s21::Controller c = s21::Controller.openModel("asd");
  EXPECT_EQ(c.IsValid(), false);
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}