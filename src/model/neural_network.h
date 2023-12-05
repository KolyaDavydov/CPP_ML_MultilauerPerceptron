#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>
#include <cmath>

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
  // load from file
  bool load(const char* file);

  // save to file
  bool save(const char* file);

  // data forward propagation
  void forward(RowVector& input);
  double activation(double x);

  // backward propagation of errors
  void backward(RowVector& output);
  double activationDerivative(double x);

  // train the neural network given an input
  void train(RowVector& input, RowVector& output);

  // test the neural network given an input
  void test(RowVector& input, RowVector& output);

  void resetConfusion();
  void evaluate(RowVector& output);
  void confusionMatrix(RowVector*& precision, RowVector*& recall);

  // get max output index
  int vote(double& value);
  int vote(RowVector& v, double& value);

  // get last layer output
  double output(int col);

  // get output layer mean squere error
  double mse();
};
#endif
