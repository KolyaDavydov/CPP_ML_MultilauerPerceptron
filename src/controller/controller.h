#ifndef CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_
#define CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_

#include "../model/model.h"

namespace s21 {
class MlpController {
 public:
  MlpController(MlpModel *m) : model_(m){};

  bool openModel(std::string filename) {
    model_->openModel(filename);
    return model_->getValid();
  }
  bool openDataset(std::string filename) {
    model_->openDataset(filename);
    return model_->getDatasetLoaded();
  }

  bool saveModel(std::string filename) { return model_->saveModel(filename); }

  bool trainModel(int epoch, int hiden_layers) {
    model_->trainModel(epoch, hiden_layers);
    return model_->getDatasetLoaded();
  }
  char recognizeImage(std::string letter) {
    model_->recognizeImage(letter);
    return model_->getRecognized();
  }
  bool testModel(int test_part) { return model_->testModel(test_part); }

 private:
  MlpModel *model_;
};
}  // namespace s21
#endif  // CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_
