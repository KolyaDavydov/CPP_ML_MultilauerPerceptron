#ifndef CPP7_MLP_1_SRC_MODEL_GRAPH_PERCEPTRON_H_
#define CPP7_MLP_1_SRC_MODEL_GRAPH_PERCEPTRON_H_

#include <locale.h>

#include "Eigen/Eigen"
typedef Eigen::RowVectorXd RowVector;
typedef Eigen::MatrixXd Matrix;
using namespace std;

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#define CMD_RESET "\x1b[0m"
#define CMD_RED "\x1b[31;1m"
#define CMD_GREEN "\x1b[32;1m"
#define LETTERS 26
#define LEARNING_RATE 0.01

#include "graph_layer.h"
using namespace std;

namespace s21 {

class GraphPerceptron {
 public:
  double mLearningRate;
  vector<int> mArchitecture;
  Matrix* mConfusion;

  GraphPerceptron();
  ~GraphPerceptron();
  GraphPerceptron(vector<int> architecture,
                  double learningRate = LEARNING_RATE);
  void init(vector<int> architecture, double learningRate = LEARNING_RATE);
  bool Load(const char* filename);
  bool Save(const char* file);
  void Train(RowVector& input, RowVector& output);
  void Test(RowVector& input, RowVector& output);
  void Forward(RowVector& input);
  double Activation(double x);
  void Backward(RowVector& output);
  double ActivationDerivative(double x);
  void ConfusionMatrix(RowVector*& precision, RowVector*& recall);
  void Evaluate(RowVector& output);
  int Vote(RowVector& v, double& value);
  void WeightsCalculation(int l, int n, double delta_weight);
  int FindMaximum();
  double Mse();

 private:
  size_t count_hidden_layers_{};
  size_t count_layers_{};
  size_t number_out_layer_{};
  std::vector<Layer> layers_;
};

}  // namespace s21
#endif  // CPP7_MLP_1_SRC_MODEL_GRAPH_PERCEPTRON_H_
