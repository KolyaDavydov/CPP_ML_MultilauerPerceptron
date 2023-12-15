#ifndef CPP7_MLP_1_SRC_MODEL_MODEL_H_
#define CPP7_MLP_1_SRC_MODEL_MODEL_H_

#include <chrono>
#include <fstream>
#include <random>
#include <string>

#include "graph_perceptron.h"
#include "neural_network.h"

#define MATRIX_MODEL 1
#define GRAPH_MODEL 2

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
  void OpenModel(std::string filename);
  void OpenDataset(std::string filepath);
  void OpenTestDataset(std::string filepath);
  bool SaveModel(std::string filename);

  bool GetModelValid();
  std::string GetErrorMsg();
  bool GetDatasetLoaded();
  bool GetTestDatasetLoaded();
  void SetModelType(int model_type);
  std::vector<double> GetTrainErrors() { return train_errors_; };
  testResults GetTestResults();

  bool Train(GraphPerceptron &net, std::string line, int serial, bool testing);
  void Train(GraphPerceptron &net, int epoch);
  void Test(GraphPerceptron &net, int test_part);
  bool TestModel(int test_part);
  void TrainModel(int epoch, int hiden_layers);
  std::vector<testResults> CrossValidation(int k_value, int epoch,
                                           int hiden_layers);
  void Evaluate(GraphPerceptron &net);

  bool Train(NeuralNetwork &net, std::string line, int serial, bool testing);
  void Train(NeuralNetwork &net, int epoch);
  void Test(NeuralNetwork &net, int test_part);
  void Evaluate(NeuralNetwork &net);

  void RecognizeImage(std::string letter);
  char GetRecognized() {
    if (recognizedLetter_) {
      return recognizedLetter_;
    } else {
      return '-';
    }
  };
  size_t GetDatasetSize(std::string filepath);

 private:
  std::ifstream file_{};
  size_t dataset_size_;
  size_t test_dataset_size_;
  int model_type_ = 1;
  std::string error_msg_ = "";
  NeuralNetwork matrix_net_{};
  GraphPerceptron graph_net_{};
  bool is_matrix_model_valid_ = false;
  bool is_graph_model_valid_ = false;
  bool is_dataset_loaded_ = false;
  std::vector<std::string> dataset_;
  bool is_test_dataset_loaded_ = false;
  std::vector<std::string> test_dataset_;
  std::vector<double> train_errors_{};
  testResults test_results_{};
  char recognizedLetter_ = 0;
  void ReadLetter(const std::string line, int *desired, RowVector *&data);
  void Close() {
    if (file_.is_open()) file_.close();
  };
};
}  // namespace s21
#endif  // CPP7_MLP_1_SRC_MODEL_MODEL_H_
