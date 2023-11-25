#include "model.h"

#include <iostream>
#define CMD_RESET "\x1b[0m"
#define CMD_RED "\x1b[31;1m"
#define CMD_GREEN "\x1b[32;1m"
#define LETTERS 27

namespace s21 {

/**
 * @brief конструктор с параметром
 *
 * @param filename путь и название файла
 *
 * Если Файла нет или ошибка чтения, то 'is_valid_' == 1
 */
bool MlpModel::openModel(std::string filename) {
  file_.open(filename);
  if (file_.is_open()) {
    net_.load(filename.c_str());
  } else {
    is_valid_ = false;
  }
  return is_valid_;
};

bool MlpModel::getValid() { return is_valid_; }

bool MlpModel::getDatasetLoaded() { return is_dataset_loaded_; }

size_t GetFileSize(std::string filename) {
  std::ifstream file_;
  file_.open(filename);
  std::string str;
  size_t n = 0;
  if (file_.is_open()) {
    while (getline(file_, str)) n++;
  }
  file_.close();
  return n;
}

void MlpModel::openDataset(std::string filepath) {
  std::string line;
  is_dataset_loaded_ = false;
  dataset_size_ = 0;
  std::ifstream ifs(filepath);
  if (ifs.is_open()) {
    while (std::getline(ifs, line)) {
      dataset_.push_back(line);
      dataset_size_++;
    }
    is_dataset_loaded_ = true;
  }
}

bool MlpModel::saveModel(std::string filename) {
  return net_.save(filename.c_str());
};

void MlpModel::readLetter(const std::string line, int &desired,
                          RowVector *&data) {
  int width = 28;
  int height = 28;
  std::string substring;
  std::stringstream ss;
  data = new RowVector(width * height);
  // split CSV by commas
  ss.str(line);
  getline(ss, substring, ',');
  desired = atoi(substring.c_str());
  for (int i = 0; i < height * width; i++) {
    getline(ss, substring, ',');
    data->coeffRef(0, i) = atof(substring.c_str());
  }
}

bool MlpModel::train(NeuralNetwork &net, std::string line, int serial,
                     bool testing) {
  RowVector *input;
  int desired;
  readLetter(line, desired, input);

  RowVector output(LETTERS);
  for (int c = 0; c < LETTERS; c++)
    output.coeffRef(0, c) = c == desired ? 1 : 0;
  if (testing) {
    net.test(*input, output);
    net.evaluate(output);
  } else
    net.train(*input, output);
  delete input;

  double value = 0;
  int actual = net.vote(value);
  cout << serial << ' ' << desired << " >> ";
  if (desired != actual)  // && desired != 26)  // need to fix
    cout << CMD_RED << actual << CMD_RESET " (" << value - net.output(desired)
         << ")" << endl;
  else
    cout << CMD_GREEN << actual << CMD_RESET << endl;

  return desired == actual;
}
bool MlpModel::trainModel(int epoch, int hiden_layers) {
  vector<int> init_vector{};
  switch (hiden_layers) {
    case 2:
      init_vector = {28 * 28, 128, 64, 48, 27};
      break;
    case 3:
      init_vector = {28 * 28, 128, 64, 48, 32, 27};
      break;
    case 4:
      init_vector = {28 * 28, 128, 100, 64, 48, 32, 27};
      break;
    case 5:
      init_vector = {28 * 28, 128, 100, 80, 64, 48, 32, 27};
      break;
    default:
      init_vector = {28 * 28, 128, 64, 48, 27};
  }
  net_.init(init_vector, 0.05);
  // 28 * 28, 128, 64, 48, 26}, 0.05  - 85%
  // 28 * 28, 128, 64, 48, 32, 26}, 0.05  - 81%
  // 28 * 28, 128, 100, 64, 48, 32, 26}, 0.05 - 81%
  // 28 * 28, 128, 100, 80, 64, 48, 32, 26}, 0.05 - 84%
  train(net_, epoch);
  return true;
}

void MlpModel::train(NeuralNetwork &net, int epoch) {
  double cost = 0;
  int serial = 0, success = 0;
  // tain three times for better accuracy
  for (int trial = 0; trial < epoch; trial++) {
    for (size_t n = 0; n < dataset_size_; n++) {
      if (train(net, dataset_[n].c_str(), ++serial, false)) success++;
      cost += net.mse();
    }
  }
  cout << "TRAINING:" << endl;
  cout << "cost\t" << cost / (serial * 2) << endl;
  cout << "success\t" << success << endl;
  cout << "fail\t" << serial - success << endl;
  cout << "error\t" << (double)(serial - success) / serial * 100 << "%" << endl;
  cout << endl;

  // net.save("params.txt");
}

void MlpModel::test(NeuralNetwork &net) {
  double cost = 0;
  int serial = 0, success = 0;
  for (int trial = 0; trial < 3; trial++) {
    for (size_t n = 0; n < dataset_size_; n++) {
      if (train(net, dataset_[n].c_str(), ++serial, false)) success++;
      cost += net.mse();
    }
  }
  cout << "TESTING:" << endl;
  cout << "cost\t" << cost / (serial * 2) << endl;
  cout << "success\t" << success << endl;
  cout << "fail\t" << serial - success << endl;
  cout << "error\t" << (double)(serial - success) / serial * 100 << "%" << endl;
  cout << endl;
}

void evaluate(NeuralNetwork &net) {
  RowVector *precision, *recall;
  net.confusionMatrix(precision, recall);

  double precisionVal = precision->sum() / precision->cols();
  double recallVal = recall->sum() / recall->cols();
  double f1score = 2 * precisionVal * recallVal / (precisionVal + recallVal);

  cout << "Confusion matrix:" << endl;
  cout << *net.mConfusion << endl;
  cout << "Precision: " << (int)(precisionVal * 100) << '%' << endl;
  cout << *precision << endl;
  cout << "Recall: " << (int)(recallVal * 100) << '%' << endl;
  cout << *recall << endl;
  cout << "F1 score: " << (int)(f1score * 100) << '%' << endl;
  delete precision;
  delete recall;
};

}  // namespace s21
