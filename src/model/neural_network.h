#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <cmath>
#include <vector>

#include "Eigen/Eigen"

typedef Eigen::RowVectorXd RowVector;
typedef Eigen::MatrixXd Matrix;
using namespace std;

#define LEARNING_RATE 0.01

class NeuralNetwork {
 public:
  // network learning rate
  double mLearningRate;
  // network layers architecture (input-hidden-output)
  vector<int> mArchitecture;
  // network layers vectors
  vector<RowVector*> mNeurons;
  // neurons' output errors
  vector<RowVector*> mErrors;
  // connections' weights
  vector<Matrix*> mWeights;
  // confusion matrix
  Matrix* mConfusion;

  // constructors
  NeuralNetwork();
  ~NeuralNetwork();
  NeuralNetwork(vector<int> architecture, double learningRate = LEARNING_RATE);
  void init(vector<int> architecture, double learningRate = LEARNING_RATE);
  // Load from file
  bool Load(const char* file);

  // Save to file
  bool Save(const char* file);

  // data Forward propagation
  void Forward(RowVector& input);
  double Activation(double x);

  // Backward propagation of errors
  void Backward(RowVector& output);
  double ActivationDerivative(double x);

  // Train the neural network given an input
  void Train(RowVector& input, RowVector& output);

  // Test the neural network given an input
  void Test(RowVector& input, RowVector& output);

  void ResetConfusion();
  void Evaluate(RowVector& output);
  void ConfusionMatrix(RowVector*& precision, RowVector*& recall);

  // get max output index
  int Vote(double& value);
  int Vote(RowVector& v, double& value);

  // get last layer Output
  double Output(int col);

  // get output layer mean squere error
  double Mse();
};
#endif
