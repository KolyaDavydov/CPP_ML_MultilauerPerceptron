#ifndef CPP7_MLP_1_SRC_MODEL_GRAPH_NEURON_H_
#define CPP7_MLP_1_SRC_MODEL_GRAPH_NEURON_H_

#include <cmath>
#include <iostream>
#include <vector>

namespace s21 {
class Neuron {
 public:
  Neuron();
  ~Neuron() {}
  void GenerateWeight();
  void PrintNeuron();
  void SetCountWeight(int count_weight);
  void SetValue(double value);
  void SetError(double error);
  void SetWeightNeuron(int weight_index, double weight);
  double GetValue();
  double GetError();
  double GetWeightVector(int index_weight);
  void ClearFunction();
  double value_{};
  int count_weight_;
  std::vector<double> array_weight_;
  double error_;
};

}  // namespace s21
#endif  // CPP7_MLP_1_SRC_MODEL_GRAPH_NEURON_H_
