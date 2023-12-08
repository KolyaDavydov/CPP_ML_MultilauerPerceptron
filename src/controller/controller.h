#ifndef CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_
#define CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_

#include "../model/model.h"

namespace s21 {
class MlpController {
 public:
  MlpController(MlpModel *m) : model_(m){};

  bool openModel(std::string filename) {
    model_->openModel(filename);
    return model_->getModelValid();
  }

  bool openDataset(std::string filename) {
    model_->openDataset(filename);
    return model_->getDatasetLoaded();
  }

  bool openTestDataset(std::string filename) {
    model_->openTestDataset(filename);
    return model_->getTestDatasetLoaded();
  }

  bool saveModel(std::string filename) { return model_->saveModel(filename); }

  void trainModel(int epoch, int hiden_layers) {
    model_->trainModel(epoch, hiden_layers);
  }

  char recognizeImage(std::string letter) {
    model_->recognizeImage(letter);
    return model_->getRecognized();
  }

  testResults testModel(int test_part) {
    model_->testModel(test_part);
    return model_->getTestResults();
  }

  std::vector<double> getTrainErrors() { return model_->getTrainErrors(); };

 private:
  MlpModel *model_;
};
}  // namespace s21
#endif  // CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_
