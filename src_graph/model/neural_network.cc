#include "neural_network.h"

#include <fstream>
#include <iostream>

NeuralNetwork::NeuralNetwork() { mConfusion = nullptr; }

NeuralNetwork::~NeuralNetwork() {
  if (mConfusion) delete mConfusion;
}

NeuralNetwork::NeuralNetwork(vector<int> architecture, double learningRate) {
  init(architecture, learningRate);
}
/**
 * @brief инициализирует нейронную сеть:
 * исходя из архитектуры заполняет вектора 
 * @param architecture список количества нейронов во входном слое, скрытых слоях и выходном
 * @param learningRate скорость обучения
*/
void NeuralNetwork::init(vector<int> architecture, double learningRate) {
  mArchitecture = architecture;
  mLearningRate = learningRate;

  mNeurons.clear();
  mErrors.clear();
  mWeights.clear();

  for (unsigned int i = 0; i < architecture.size(); i++) {
    // add extra neuron to each layer as a bias (with weight = 1)
    // Добавляем дополнительный нейрон к каждому слою кроме последнего
    int size = architecture[i] + (i != architecture.size() - 1);
    mNeurons.push_back(new RowVector(size));
    mErrors.push_back(new RowVector(size));

    mNeurons.back()->setZero();
    if (i < architecture.size() - 1)
      // set bias to 1 (used in vectors dot product)
      mNeurons.back()->coeffRef(architecture[i]) = 1.0;

    // initialize weights
    if (i > 0) {
      if (i != architecture.size() - 1) {
        // для архитектуры 784, 64, 48, 26 в матрице весов будет 3 подматрицы
        // размерность первой подматрицы 785 rows * 65 cols
        // размерность второй подматрицы 65  rows * 49 cols
        // размерность второй подматрицы 65  rows * 26!!! cols
        // и все подматрицы весов заполняем случайными значениями, кроме последнего столбца (bias)
        mWeights.push_back(
            new Matrix(architecture[i - 1] + 1, architecture[i] + 1));
        mWeights.back()->setRandom();
        mWeights.back()->col(architecture[i]).setZero();
        mWeights.back()->coeffRef(architecture[i - 1], architecture[i]) = 1.0;
      } else {
        mWeights.push_back(
            new Matrix(architecture[i - 1] + 1, architecture[i]));
        mWeights.back()->setRandom();
      }
    }
  }
  // задаем размер матрицы для оценки точности 26*26 и заполняем нулями
  mConfusion = new Matrix(architecture.back(), architecture.back());
  mConfusion->setZero();
}

/**
 * @brief функция активации, сигмоидная функция
 * @param принимает значение нейрона от 0 до 1!!!
 * необходима нормализация нейронов!!!!
*/
double NeuralNetwork::activation(double x) { return 1.0 / (1.0 + exp(-x)); }

/**
 * @brief функция активации, сигмоидная функция
 * @param принимает значение нейрона от 0 до 1!!!
 * необходима нормализация нейронов!!!!
*/
double NeuralNetwork::activationDerivative(double x) { return x * (1.0 - x); }

/**
 * @brief функция прямого распространения для вычисления нейронов
 * в скрытых слоях и в otput (26 нейронов)
 * 1) умножение подматрицы весов на подматрицу нейронов = подматрица следующего слоя нейронов
 * @param ссылка на вектор входных нейронов их 28*28+1
*/
void NeuralNetwork::forward(RowVector& input) {
  // set first layer input
  // заполняет первую подматрицу нейронов фактическим нормализованными значениями
  mNeurons.front()->block(0, 0, 1, input.size()) = input;
  // propagate forward (vector multiplication)
  // заполняет подматрицы нейронов остальных слоев полсе входного
  // происходит это следующим образом:
  // по сути это умножение подматрицы весов на подматрицу предыдущего слоя нейронов
  // и потом к каждому полученному нейрону применение функции активации - сигмоида
  for (unsigned int i = 1; i < mArchitecture.size(); i++) {
    // copy values ingoring last neuron as it is a bias
    mNeurons[i]->block(0, 0, 1, mArchitecture[i]) =
        (*mNeurons[i - 1] * *mWeights[i - 1]).block(0, 0, 1, mArchitecture[i]);
    // apply activation function
    for (int col = 0; col < mArchitecture[i]; col++)
      mNeurons[i]->coeffRef(col) = activation(mNeurons[i]->coeffRef(col));
  }
}

void NeuralNetwork::test(RowVector& input, RowVector& output) {
  forward(input);
  // calculate last layer errors
  *mErrors.back() = output - *mNeurons.back();
}

void NeuralNetwork::resetConfusion() {
  if (mConfusion) mConfusion->setZero();
}

void NeuralNetwork::evaluate(RowVector& output) {
  double desired = 0, actual = 0;
  mConfusion->coeffRef(vote(output, desired), vote(*mNeurons.back(), actual))++;
}

void NeuralNetwork::confusionMatrix(RowVector*& precision, RowVector*& recall) {
  int rows = (int)mConfusion->rows();
  int cols = (int)mConfusion->cols();

  precision = new RowVector(cols);
  for (int col = 0; col < cols; col++) {
    double colSum = 0;
    for (int row = 0; row < rows; row++)
      colSum += mConfusion->coeffRef(row, col);
    precision->coeffRef(col) = mConfusion->coeffRef(col, col) / colSum;
  }

  recall = new RowVector(rows);
  for (int row = 0; row < rows; row++) {
    double rowSum = 0;
    for (int col = 0; col < cols; col++)
      rowSum += mConfusion->coeffRef(row, col);
    if (rowSum == 0) {
      recall->coeffRef(row) = 0;
    } else {
      recall->coeffRef(row) = mConfusion->coeffRef(row, row) / rowSum;
    }
    
  }

  // convert confusion to percentage
  for (int row = 0; row < rows; row++) {
    double rowSum = 0;
    for (int col = 0; col < cols; col++)
      rowSum += mConfusion->coeffRef(row, col);
    if (rowSum == 0) {
      for (int col = 0; col < cols; col++)
        mConfusion->coeffRef(row, col) = 0;
    } else {
      for (int col = 0; col < cols; col++)
        mConfusion->coeffRef(row, col) =
            mConfusion->coeffRef(row, col) * 100 / rowSum;
    }

  }
}

/**
 * @brief метод обратного распространения ошибки
 * заполняет матрицу ошибок по методу градиентного спуска
 * обновляет матрицу весов
 * @param output указатель на вектор шаблон размером 26 (который заполнен 0,
 * кроме фактического числа (номера буквы) данной строки)
*/
void NeuralNetwork::backward(RowVector& output) {
  // calculate last layer errors
  *mErrors.back() = output - *mNeurons.back();

  // calculate hidden layers' errors (vector multiplication)
  for (size_t i = mErrors.size() - 2; i > 0; i--)
    *mErrors[i] = *mErrors[i + 1] * mWeights[i]->transpose();

  // update weights
  for (size_t i = 0, size = mWeights.size(); i < size; i++)
    for (int row = 0, rows = (int)mWeights[i]->rows(); row < rows; row++)
      for (int col = 0, cols = (int)mWeights[i]->cols(); col < cols; col++)
        mWeights[i]->coeffRef(row, col) +=
            mLearningRate * mErrors[i + 1]->coeffRef(col) *
            mNeurons[i]->coeffRef(row) *
            activationDerivative(mNeurons[i + 1]->coeffRef(col));
}

/**
 * @brief обучение на датасете
 * фактически заполняет матрицу нейронов и матрицу ошибок
 * @param input указатель на нормализованное (0 - 1) числовое представление
 * строки датасета
 * @param output указатель на вектор шаблон размером 26 (который заполнен 0,
 * кроме фактического числа (номера буквы) данной строки)
*/
void NeuralNetwork::train(RowVector& input, RowVector& output) {
  forward(input); // заполняем матрицу нейронов
  backward(output); // заполняем матрицу ошибок
}

// mean square error
double NeuralNetwork::mse() {
  return sqrt((*mErrors.back()).dot((*mErrors.back())) /
              mErrors.back()->size());
}

int NeuralNetwork::vote(double& value) {
  auto it = mNeurons.back();
  return vote(*it, value);
}

int NeuralNetwork::vote(RowVector& v, double& value) {
  int index = 0;
  for (int i = 1; i < v.cols(); i++)
    if (v[i] > v[index]) index = i;
  value = v[index];
  return index;
}

double NeuralNetwork::output(int col) {
  auto it = mNeurons.back();
  return (*it)[col];
}

bool NeuralNetwork::save(const char* filename) {
  stringstream tplgy;
  for (auto it = mArchitecture.begin(), _end = mArchitecture.end(); it != _end;
       it++)
    tplgy << *it << (it != _end - 1 ? "," : "");

  stringstream wts;
  for (auto it = mWeights.begin(), _end = mWeights.end(); it != _end; it++)
    wts << **it << (it != _end - 1 ? "," : "") << endl;

  ofstream file(filename);
  file << "learningRate: " << mLearningRate << endl;
  file << "architecture: " << tplgy.str() << endl;
  file << "weights: " << endl << wts.str() << endl;
  file.close();
  return true;
}

bool NeuralNetwork::load(const char* filename) {
  mArchitecture.clear();

  ifstream file(filename);
  if (!file.is_open()) return false;
  string line, name, value;
  if (!getline(file, line, '\n')) return false;
  stringstream lr(line);

  // read learning rate
  getline(lr, name, ':');
  if (name != "learningRate") return false;
  if (!getline(lr, value, '\n')) return false;
  mLearningRate = atof(value.c_str());

  // read topoplogy
  getline(file, line, '\n');
  stringstream ss(line);
  getline(ss, name, ':');
  if (name != "architecture") return false;
  while (getline(ss, value, ',')) mArchitecture.push_back(atoi(value.c_str()));

  // initialize using read architecture
  init(mArchitecture, mLearningRate);

  // read weights
  name = "";
  getline(file, line, '\n');
  stringstream we(line);
  getline(we, name, ':');
  if (name != "weights") return false;

  string matrix;
  for (size_t i = 0; i < mArchitecture.size(); i++)
    if (getline(file, matrix, ',')) {
      stringstream ss(matrix);
      int row = 0;
      while (getline(ss, value, '\n'))
        if (!value.empty()) {
          stringstream word(value);
          int col = 0;
          while (getline(word, value, ' '))
            if (!value.empty())
              mWeights[i]->coeffRef(row, col++) = atof(value.c_str());
          row++;
        }
    }

  file.close();
  return true;
}
