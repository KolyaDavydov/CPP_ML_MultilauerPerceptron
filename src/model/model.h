#ifndef CPP7_MLP_1_SRC_MODEL_MODEL_H_
#define CPP7_MLP_1_SRC_MODEL_MODEL_H_

#include <fstream>
#include <string>

#include "neural_network.h"

namespace s21 {
class MlpModel {
 public:
  MlpModel() = default;
  bool openModel(std::string filename);
  void openDataset(std::string filepath);
  bool saveModel(std::string filename);

  bool getValid();
  bool getDatasetLoaded();
  bool train(NeuralNetwork &net, std::string line, int serial, bool testing);
  void train(NeuralNetwork &net, int epoch);
  void test(NeuralNetwork &net);
  bool trainModel(int epoch, int hiden_layers);
  void evaluate(NeuralNetwork &net);
  size_t getDatasetSize(std::string filepath);

 private:
  std::ifstream file_{};
  size_t dataset_size_;
  NeuralNetwork net_{};
  bool is_valid_;
  bool is_dataset_loaded_;
  std::vector<std::string> dataset_;
  void readLetter(const std::string line, int &desired, RowVector *&data);
  void Close() {
    if (file_.is_open()) file_.close();
  };
};
}  // namespace s21
#endif  // CPP7_MLP_1_SRC_MODEL_MODEL_H_
