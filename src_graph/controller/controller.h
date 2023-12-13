#ifndef CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_
#define CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_

#include "../model/model.h"

namespace s21 {
class MlpController {
 public:
  MlpController(MlpModel *m) : model_(m){};

  void SetModelType(int model_type) { model_->SetModelType(model_type); }

  bool OpenModel(std::string filename) {
    model_->OpenModel(filename);
    return model_->GetModelValid();
  }

  bool OpenDataset(std::string filename) {
    model_->OpenDataset(filename);
    return model_->GetDatasetLoaded();
  }

  bool OpenTestDataset(std::string filename) {
    model_->OpenTestDataset(filename);
    return model_->GetTestDatasetLoaded();
  }

  bool SaveModel(std::string filename) { return model_->SaveModel(filename); }

  void TrainModel(int epoch, int hiden_layers) {
    model_->TrainModel(epoch, hiden_layers);
  }

  char RecognizeImage(std::string letter) {
    model_->RecognizeImage(letter);
    return model_->GetRecognized();
  }

  bool GetModelValid() { return model_->GetModelValid(); }

  testResults TestModel(int test_part) {
    model_->TestModel(test_part);
    return model_->GetTestResults();
  }

  std::vector<double> GetTrainErrors() { return model_->GetTrainErrors(); };

  std::vector<testResults> CrossValidation(int k_value, int epoch,
                                           int hiden_layers) {
    return model_->CrossValidation(k_value, epoch, hiden_layers);
  }

 private:
  MlpModel *model_;
};
}  // namespace s21
#endif  // CPP7_MLP_1_SRC_CONTROLLER_CONTROLLER_H_
