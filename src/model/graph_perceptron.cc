#include "graph_perceptron.h"

namespace s21 {

GraphPerceptron::GraphPerceptron() { mConfusion = nullptr; }

GraphPerceptron::~GraphPerceptron() {
  if (mConfusion) delete mConfusion;
}

GraphPerceptron::GraphPerceptron(vector<int> architecture,
                                 double learningRate) {
  init(architecture, learningRate);
}
/**
 * @brief инициализирует нейронную сеть:
 * исходя из архитектуры заполняет вектора
 * @param architecture список количества нейронов во входном слое, скрытых слоях
 * и выходном
 * @param learningRate скорость обучения
 */
void GraphPerceptron::init(vector<int> architecture, double learningRate) {
  mArchitecture = architecture;
  mLearningRate = learningRate;

  count_hidden_layers_ = architecture.size() - 2;
  count_layers_ = architecture.size();
  number_out_layer_ = count_layers_ - 1;
  layers_.resize(count_layers_);

  layers_[0].SetCountNeuron(architecture[0] + 1);

  for (unsigned int i = 1; i < count_layers_ - 1; i++) {
    layers_[i].SetCountNeuron(architecture[i] + 1);
    layers_[i].SetCountWeightNeuron(architecture[i - 1] + 1);
  }
  layers_[number_out_layer_].SetCountNeuron(architecture[number_out_layer_]);
  layers_[number_out_layer_].SetCountWeightNeuron(
      architecture[number_out_layer_ - 1] + 1);

  // заполняем веса случайными значениями
  for (size_t i = 1; i < layers_.size(); i++) {
    for (size_t l = 0; l < layers_[i].layer_.size(); l++) {
      if (l < layers_[i].layer_.size() - 1) {
        for (size_t j = 0; j < layers_[i].layer_[l].array_weight_.size(); j++) {
          layers_[i].layer_[l].array_weight_[j] =
              (0.001 * (std::rand() % 2001 - 1000));
        }
      } else {
        for (size_t j = 0; j < layers_[i].layer_[l].array_weight_.size(); j++) {
          layers_[i].layer_[l].array_weight_[j] = 0;
        }
      }
    }
    layers_[i].layer_.back().array_weight_.back() = 1.0;
  }
  // задаем размер матрицы для оценки точности 26*26 и заполняем нулями
  mConfusion = new Matrix(architecture.back(), architecture.back());
  mConfusion->setZero();
}

bool GraphPerceptron::Load(const char* filename) {
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
  for (size_t i = 1; i < mArchitecture.size(); i++) {
    if (getline(file, matrix, ',')) {
      stringstream ss(matrix);
      int row = 0;
      while (getline(ss, value, '\n'))
        if (!value.empty()) {
          stringstream word(value);
          int col = 0;
          while (getline(word, value, ' '))
            if (!value.empty())
              layers_[i].SetNeuronWeights(col++, row, atof(value.c_str()));
          row++;
        }
    }
  }
  file.close();
  return true;
}

bool GraphPerceptron::Save(const char* filename) {
  stringstream tplgy;
  for (auto it = mArchitecture.begin(), _end = mArchitecture.end(); it != _end;
       it++)
    tplgy << *it << (it != _end - 1 ? "," : "");

  stringstream wts;
  for (size_t l = 1; l < count_layers_; l++) {
    for (int n = 0; n < layers_[l].layer_[0].count_weight_; n++) {
      for (size_t i = 0; i < layers_[l].GetCountNeuron(); i++) {
        wts << setw(12) << layers_[l].layer_[i].array_weight_[n];
      }
      if (n == layers_[l].layer_[0].count_weight_ - 1 &&
          l != count_layers_ - 1) {
        wts << "," << endl;
      } else {
        wts << endl;
      }
    }
  }

  ofstream file(filename);
  file << "learningRate: " << mLearningRate << endl;
  file << "architecture: " << tplgy.str() << endl;
  file << "weights: " << endl << wts.str() << endl;
  file.close();
  return true;
}

/**
 * @brief обучение на датасете
 * фактически заполняет матрицу нейронов и матрицу ошибок
 * @param input указатель на нормализованное (0 - 1) числовое представление
 * строки датасета
 * @param output указатель на вектор шаблон размером 26 (который заполнен 0,
 * кроме фактического числа (номера буквы) данной строки)
 */
void GraphPerceptron::Train(RowVector& input, RowVector& output) {
  Forward(input);    // заполняем матрицу нейронов
  Backward(output);  // заполняем матрицу ошибок
}

/**
 * @brief функция прямого распространения для вычисления нейронов
 * в скрытых слоях и в otput (26 нейронов)
 * 1) умножение подматрицы весов на подматрицу нейронов = подматрица следующего
 * слоя нейронов
 * @param ссылка на вектор входных нейронов их 28*28+1
 */
void GraphPerceptron::Forward(RowVector& input) {
  // set first layer input
  // заполняет первую подматрицу нейронов фактическим нормализованными
  // значениями
  for (size_t i = 0; i < layers_[0].GetCountNeuron() - 1; i++) {
    layers_[0].layer_[i].SetValue(input[i]);
  }

  // mNeurons.front()->block(0, 0, 1, input.size()) = input;
  // propagate forward (vector multiplication)
  // заполняет подматрицы нейронов остальных слоев полсе входного
  // происходит это следующим образом:
  // по сути это умножение подматрицы весов на подматрицу предыдущего слоя
  // нейронов и потом к каждому полученному нейрону применение функции активации
  // - сигмоида
  for (unsigned int i = 1; i < mArchitecture.size(); i++) {
    // copy values ingoring last neuron as it is a bias
    for (size_t n = 0; n < layers_[i].GetCountNeuron(); n++) {
      if (n == layers_[i].GetCountNeuron() - 1 && i != mArchitecture.size() - 1)
        break;
      double sum = 0;
      for (size_t w = 0; w < layers_[i - 1].GetCountNeuron(); w++) {
        sum +=
            layers_[i - 1].GetNeuronValue(w) * layers_[i].GetWeightNeuron(n, w);
      }
      layers_[i].SetNeuronValue(n + 1, Activation(sum));
    }
  }
}

/**
 * @brief функция активации, сигмоидная функция
 * @param принимает значение нейрона от 0 до 1!!!
 * необходима нормализация нейронов!!!!
 */
double GraphPerceptron::Activation(double x) { return 1.0 / (1.0 + exp(-x)); }

/**
 * @brief метод обратного распространения ошибки
 * заполняет матрицу ошибок по методу градиентного спуска
 * обновляет матрицу весов
 * @param output указатель на вектор шаблон размером 26 (который заполнен 0,
 * кроме фактического числа (номера буквы) данной строки)
 */
void GraphPerceptron::Backward(RowVector& output) {
  for (size_t l = number_out_layer_; l != 0; l--) {
    for (size_t n = 0; n < layers_[l].GetCountNeuron(); n++) {
      double error = 0;
      if (l == number_out_layer_) {
        error = output[n] - layers_[l].layer_[n].GetValue();
      } else {
        for (size_t arr = 0; arr < layers_[l + 1].GetCountNeuron(); arr++) {
          error += layers_[l + 1].layer_[arr].error_ *
                   layers_[l + 1].layer_[arr].array_weight_[n];
        }
      }
      layers_[l].layer_[n].error_ = error;
    }
  }
  for (size_t i = 1; i < mArchitecture.size(); i++) {
    for (size_t row = 0; row < layers_[i].layer_[0].array_weight_.size();
         row++) {
      for (size_t col = 0; col < layers_[i].GetCountNeuron(); col++) {
        layers_[i].layer_[col].array_weight_[row] +=
            mLearningRate * layers_[i].layer_[col].error_ *
            layers_[i - 1].layer_[row].value_ *
            ActivationDerivative(layers_[i].layer_[col].value_);
      }
    }
  }
}

/**
 * @brief функция активации, сигмоидная функция
 * @param принимает значение нейрона от 0 до 1!!!
 * необходима нормализация нейронов!!!!
 */
double GraphPerceptron::ActivationDerivative(double x) { return x * (1.0 - x); }

void GraphPerceptron::WeightsCalculation(int l, int n, double delta_weight) {
  for (size_t w = 0; w < layers_[l - 1].GetCountNeuron(); w++) {
    double new_weight =
        layers_[l].GetWeightNeuron(n, w) -
        layers_[l - 1].GetNeuronValue(w) * delta_weight * mLearningRate;
    layers_[l].SetNeuronWeights(n, w, new_weight);
  }
}

int GraphPerceptron::FindMaximum() {
  double maximum = 0.0;
  int index_max = 0;

  for (size_t n = 0; n < layers_[number_out_layer_].GetCountNeuron(); n++) {
    if (layers_[number_out_layer_].GetNeuronValue(n) > maximum) {
      maximum = layers_[number_out_layer_].GetNeuronValue(n);
      index_max = n;
    }
  }
  return index_max;
}

// mean square error
double GraphPerceptron::Mse() {
  double err = 0;
  double z = 0;
  for (size_t i = 0; i < layers_[number_out_layer_].layer_.size(); i++) {
    z = layers_[number_out_layer_].layer_[i].GetError();
    err += pow(z, 2);
  }
  err = sqrt(err / layers_[number_out_layer_].layer_.size());
  return err;
}

void GraphPerceptron::Test(RowVector& input, RowVector& output) {
  Forward(input);
  // calculate last layer errors
  for (int i = 0; i < output.size(); i++) {
    layers_[number_out_layer_].layer_[i].error_ =
        output[i] - layers_[number_out_layer_].layer_[i].GetValue();
  }
}

void GraphPerceptron::ConfusionMatrix(RowVector*& precision,
                                      RowVector*& recall) {
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
      for (int col = 0; col < cols; col++) mConfusion->coeffRef(row, col) = 0;
    } else {
      for (int col = 0; col < cols; col++)
        mConfusion->coeffRef(row, col) =
            mConfusion->coeffRef(row, col) * 100 / rowSum;
    }
  }
}

void GraphPerceptron::Evaluate(RowVector& output) {
  double desired = 0, actual = 0;
  RowVector mNeurons(output.size());
  for (int i = 0; i < output.size(); i++) {
    mNeurons(i) = (layers_[number_out_layer_].layer_[i].value_);
  }
  mConfusion->coeffRef(Vote(output, desired), Vote(mNeurons, actual))++;
}

int GraphPerceptron::Vote(RowVector& v, double& value) {
  int index = 0;
  for (int i = 1; i < v.cols(); i++)
    if (v[i] > v[index]) index = i;
  value = v[index];
  return index;
}
};  // namespace s21
