#ifndef CPP7_MLP_1_SRC_MODEL_GRAPH_PERCEPTRON_H_
#define CPP7_MLP_1_SRC_MODEL_GRAPH_PERCEPTRON_H_

#include <locale.h>
#include "Eigen/Eigen"
typedef Eigen::RowVectorXd RowVector;
typedef Eigen::MatrixXd Matrix;
using namespace std;

#include <cmath>
#include <vector>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#define CMD_RESET "\x1b[0m"
#define CMD_RED "\x1b[31;1m"
#define CMD_GREEN "\x1b[32;1m"
#define LETTERS 26
#define LEARNING_RATE 0.01

// #include "common.h"
#include "graph_layer.h"
using namespace std;

namespace s21 {

// class GraphPerceptron : public Common {
class GraphPerceptron {



 public:
  // network learning rate
  double mLearningRate;
  // network layers architecture (input-hidden-output)
  vector<int> mArchitecture;
    // confusion matrix
  Matrix* mConfusion;


  GraphPerceptron();
  ~GraphPerceptron();
  GraphPerceptron(vector<int> architecture, double learningRate = LEARNING_RATE);
  void init(vector<int> architecture, double learningRate = LEARNING_RATE);

  bool load(const char* filename);

    // save to file
  bool save(const char* file);

    // train the neural network given an input
  void train(RowVector& input, RowVector& output);

  // test the neural network given an input
  void test(RowVector& input, RowVector& output);

    // data forward propagation
  void forward(RowVector& input);
  double activation(double x);

  // backward propagation of errors
  void backward(RowVector& output);
   double activationDerivative(double x);

  void confusionMatrix(RowVector*& precision, RowVector*& recall);

  void evaluate(RowVector& output);

  // get max output index
  // int vote(double& value);
  int vote(RowVector& v, double& value);

  void WeightsCalculation(int l, int n, double delta_weight);

    // get max output index
  int FindMaximum();

  double mse();

  // //___________Create_And_Init_GraphPerceptron___________//
  // GraphPerceptron();
  // GraphPerceptron(int layer_hidden_count);
  // void InitGraphPerceptron();
  // void GenerateWeightNeuron();

  // //_____________________Parse_File______________________//
  // void LoadValuesTrain(std::string const &filename_train);
  // void LoadValuesTest(std::string const &filename_test);
  // // void SetStartValues(const char* filename_train, const char* filename_test);

  // //________________________Train________________________//
  // void EpochTrain(int epoch, std::vector<double> *report_graph);
  // //  void CrossValidation(const char* filename_train, int k_validation);
  // void TrainForthBack(int d) override;

  // //_________________ForwardPropagation__________________//
  // void ForwardPropagation();

  // //_________________BackwardPropagation_________________//
  // void BackwardPropagation();
  // void WeightsCalculation(int l, int n, double delta_weight);

  // //__________________Save_Load_Weights__________________//
  // void SaveWeights(std::string save_new_weights);
  // void LoadWeights(std::string const &filename_weights);

  // //_______________________Predict_______________________//
  // std::vector<int> Predict(std::string name_image);
  // int FindMaximum();
  // std::vector<int> FindMaximumPredict();

  // //________________________Test_________________________//
  // void Test(int test_sample);
  // void TestForthBack(int d) override;
  // double CalculatePercent(int k_group) override;
  // void CountingSuccessfulLetters();
  // void Testing(char *filename_test);

  // //________________PrintGraphPerceptron_________________//
  // void PrintPerceptron();

  // //__________________ResizePerceptron___________________//
  // void Clear();
  // void ResizePerceptron(int count_hidden_layers);

 private:
  //  size_t successful_find_letter_{};
  size_t count_hidden_layers_{};
  size_t count_layers_{};
  size_t number_out_layer_{};
  std::vector<Layer> layers_;

};

}  // namespace s21
#endif  // CPP7_MLP_1_SRC_MODEL_GRAPH_PERCEPTRON_H_
