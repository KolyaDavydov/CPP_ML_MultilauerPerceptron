#include "model.h"

#include <iostream>
#define CMD_RESET "\x1b[0m"
#define CMD_RED "\x1b[31;1m"
#define CMD_GREEN "\x1b[32;1m"
#define LETTERS 26

namespace s21 {

/**
 * @brief конструктор с параметром
 *
 * @param filename путь и название файла
 *
 * Если Файла нет или ошибка чтения, то 'is_valid_' == 1
 */
bool MlpModel::openModel(std::string filename) {
  is_valid_ = false;
  file_.open(filename);
  if (file_.is_open()) {
    net_.load(filename.c_str());
    is_valid_ = true;
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
  dataset_.erase(dataset_.begin(), dataset_.end());
  is_dataset_loaded_ = false;
  dataset_size_ = 0;
  std::ifstream ifs(filepath);
  if (ifs.is_open()) {
    while (std::getline(ifs, line)) {
      if (line.length() > 1569) {
        dataset_.push_back(line);
        dataset_size_++;
      }
    }
    is_dataset_loaded_ = true;
    // std::sort(dataset_.begin(), dataset_.end());
    ifs.close();
  }
}

bool MlpModel::saveModel(std::string filename) {
  return net_.save(filename.c_str());
};

/**
 * @brief парсит строку из датасета
 * @param line строка датасета
 * @param desired указатель под каким номером буквы должна быть данная строка
 * (первое число строки)
 * @param data вектор в числовом представлении строки датасета после
 * нормализации (от 0 до 1)
 */
void MlpModel::readLetter(const std::string line, int *desired,
                          RowVector *&data) {
  int width = 28;
  int height = 28;
  std::string substring;
  std::stringstream ss;
  data = new RowVector(width * height);
  // split CSV by commas
  ss.str(line);
  getline(ss, substring, ',');
  *desired = atoi(substring.c_str()) - 1;
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++) {
      getline(ss, substring, ',');
      data->coeffRef(0, y * width + x) = atof(substring.c_str()) / 255;
    }
  // for (int i = 0; i < height * width; i++) {
  //   getline(ss, substring, ',');
  //   data->coeffRef(0, i) = atof(substring.c_str());
  // }
}
/**
 * @brief функция тренировки для каждой строки (буквы датасета)
 * @param net указатель на нейросеть
 * @param line строка (буква) датасета
 * @param serial номер строки (буквы) датасета
 * @param testing если true - тестируемый датасет, false - тренировочный датасет
 *
 * @return true - если строка (буква) успешно прошла тренировку или тетсирование
 */
bool MlpModel::train(NeuralNetwork &net, std::string line, int serial,
                     bool testing) {
  RowVector *input;  // указатель на вектор где хранятся числовые значение
                     // входных нейронов
  int desired;  // фактическое значение (номер буквы) текущей строки (первое
                // число строки)
  readLetter(line, &desired, input);

  // создаем вектор-шаблон длиной 26 (количество букв)
  // который заподняем нулями, кроме элемента находящегося под индексом
  // фактического значения
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
  cout << serial << ' ' << char(desired + 65) << " >> ";
  if (desired != actual)
    cout << CMD_RED << char(actual + 65) << CMD_RESET " ("
         << value - net.output(desired) << ")" << endl;
  else
    cout << CMD_GREEN << char(actual + 65) << CMD_RESET << endl;

  return desired == actual;
}
bool MlpModel::trainModel(int epoch, int hiden_layers) {
  vector<int> init_vector{};
  switch (hiden_layers) {  // количество нейронов подобрано эмпирически
    case 2:
      init_vector = {28 * 28, 64, 48, LETTERS};
      break;
    case 3:
      init_vector = {28 * 28, 64, 52, 48, LETTERS};
      break;
    case 4:
      init_vector = {28 * 28, 64, 52, 48, 40, LETTERS};
      break;
    case 5:
      init_vector = {28 * 28, 128, 64, 52, 48, 32, LETTERS};
      break;
    default:
      init_vector = {28 * 28, 64, 48, LETTERS};
  }
  net_.init(init_vector, 0.01);
  // 28 * 28, 64, 48, 26}, 0.03  2 - 76%  3 - 73% 4 - 73%
  // 28 * 28, 64, 52, 48, 26}, 0.01  3 - 68%
  // 28 * 28, 72, 64, 52, 48, 26}, 0.01 - 31%
  train(net_, epoch);
  evaluate(net_);
  return true;
}

bool MlpModel::testModel(int test_part) {
  bool result = false;
  if (!is_dataset_loaded_) {
    cout << "Dataset not loaded" << endl;
  } else if (!is_valid_) {
    cout << "Model not loaded" << endl;
  } else if (test_part == 0) {
    cout << "Nothing to test, test part ==0" << endl;
  } else {
    test(net_, test_part);
    evaluate(net_);
    result = true;
  }
  return result;
}

void MlpModel::train(NeuralNetwork &net, int epoch) {
  double cost = 0;
  int serial = 0;   // номер строки датасета (буквы)
  int success = 0;  // для каждой строки датасета (буквы) успешное определение
  // tain three times for better accuracy
  for (int trial = 0; trial < epoch; trial++) {
    for (size_t n = 0; n < dataset_size_; n++) {
      if (train(net, dataset_[n].c_str(), ++serial, false)) success++;
      cost += net.mse();
    }
  }
  double error = (double)(serial - success) / serial * 100;
  cout << "TRAINING:" << endl;
  cout << "cost\t" << cost / (serial * 2) << endl;
  cout << "success\t" << success << endl;
  cout << "fail\t" << serial - success << endl;
  cout << "error\t" << error << "%" << endl;
  cout << endl;
  if (error < 30) is_valid_ = true;

  // net.save("params.txt");
}

void MlpModel::test(NeuralNetwork &net, int test_part) {
  double cost = 0;
  int serial = 0, success = 0;
  for (size_t n = 0; n < dataset_size_ * test_part / 100; n++) {
    if (train(net, dataset_[n].c_str(), ++serial, true)) success++;
    cost += net.mse();
  }
  cout << "TESTING:" << endl;
  cout << "cost\t" << cost / (serial * 2) << endl;
  cout << "success\t" << success << endl;
  cout << "fail\t" << serial - success << endl;
  cout << "error\t" << (double)(serial - success) / serial * 100 << "%" << endl;
  cout << endl;
}

void MlpModel::evaluate(NeuralNetwork &net) {
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
