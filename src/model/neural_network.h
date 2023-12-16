#ifndef CPP7_MLP_1_SRC_MODEL_NEURAL_NETWORK_H
#define CPP7_MLP_1_SRC_MODEL_NEURAL_NETWORK_H

#include <cmath>
#include <vector>

#include "Eigen/Eigen"

typedef Eigen::RowVectorXd RowVector;
typedef Eigen::MatrixXd Matrix;
using namespace std;

#define LEARNING_RATE 0.01

class NeuralNetwork {
 public:
  double mLearningRate;
  vector<int> mArchitecture;
  vector<RowVector*> mNeurons;
  vector<RowVector*> mErrors;
  vector<Matrix*> mWeights;
  Matrix* mConfusion;
  NeuralNetwork();
  ~NeuralNetwork();
  NeuralNetwork(vector<int> architecture, double learningRate = LEARNING_RATE);
  void init(vector<int> architecture, double learningRate = LEARNING_RATE);
  bool Load(const char* file);
  bool Save(const char* file);
  void Forward(RowVector& input);
  double Activation(double x);
  void Backward(RowVector& output);
  double ActivationDerivative(double x);
  void Train(RowVector& input, RowVector& output);
  void Test(RowVector& input, RowVector& output);
  void ResetConfusion();
  void Evaluate(RowVector& output);
  void ConfusionMatrix(RowVector*& precision, RowVector*& recall);
  int Vote(double& value);
  int Vote(RowVector& v, double& value);
  double Output(int col);
  double Mse();
};
#endif
