#ifndef CPP7_MLP_1_SRC_MODEL_MODEL_H_
#define CPP7_MLP_1_SRC_MODEL_MODEL_H_

#include <fstream>
#include <string>

#include "neural_network.h"

struct testResults {
  double accuracy;
  double precision;
  double recall;
  double fmeasure;
  double runtime;
};

namespace s21 {
class MlpModel {
 public:
  MlpModel() = default;
  bool openModel(std::string filename);
  void openDataset(std::string filepath);
  void openTestDataset(std::string filepath);
  bool saveModel(std::string filename);

  bool getModelValid();
  bool getDatasetLoaded();
  bool getTestDatasetLoaded();
  testResults getTestResults();
  bool train(NeuralNetwork &net, std::string line, int serial, bool testing);
  void train(NeuralNetwork &net, int epoch);
  void test(NeuralNetwork &net, int test_part);
  bool testModel(int test_part);
  bool trainModel(int epoch, int hiden_layers);
  void evaluate(NeuralNetwork &net);
  void recognizeImage(std::string letter);
  char getRecognized() {
    if (recognizedLetter_) {
      return recognizedLetter_;
    } else {
      return '-';
    }
  };
  size_t getDatasetSize(std::string filepath);

 private:
  std::ifstream file_{};
  size_t dataset_size_;
  size_t test_dataset_size_;
  NeuralNetwork net_{};
  bool is_model_valid_ = false;
  bool is_dataset_loaded_ = false;
  std::vector<std::string> dataset_;
  bool is_test_dataset_loaded_ = false;
  std::vector<std::string> test_dataset_;
  testResults test_results_{};
  char recognizedLetter_ = 0;
  void readLetter(const std::string line, int *desired, RowVector *&data);
  void Close() {
    if (file_.is_open()) file_.close();
  };
};
}  // namespace s21
#endif  // CPP7_MLP_1_SRC_MODEL_MODEL_H_
