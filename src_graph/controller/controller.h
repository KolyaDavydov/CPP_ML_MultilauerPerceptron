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

  bool trainModel(int model_type, int epoch, int hiden_layers) {
    return model_->trainModel(model_type, epoch, hiden_layers);
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

  std::vector<testResults> crossValidation(int k_value, int model_type, int epoch, int hiden_layers) {
    return model_->crossValidation(k_value, model_type, epoch, hiden_layers);
  }

 private:
  MlpModel *model_;
};
}  // namespace s21
#endif  // CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_
