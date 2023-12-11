#include "model.h"

#include <QElapsedTimer>
#include <iostream>
#define CMD_RESET "\x1b[0m"
#define CMD_RED "\x1b[31;1m"
#define CMD_GREEN "\x1b[32;1m"
#define LETTERS 26

namespace s21 {

/**
 * @brief загружает обученную модель
 * @param filename путь и название файла (.txt)
 * @return Если Файла нет или ошибка чтения, то 'is_valid_' == 1
 */
bool MlpModel::openModel(std::string filename) {
  is_model_valid_ = false;
  file_.open(filename);
  if (file_.is_open()) {
    net_.load(filename.c_str());
    is_model_valid_ = true;
  } else {
    is_model_valid_ = false;
  }
  return is_model_valid_;
};

/**
 * @brief проверка удалось ли загрузить модель
 * @return true - модель успешно загруженна
 */
bool MlpModel::getModelValid() { return is_model_valid_; }

/**
 * @brief проверка удалось ли загрузить датасет
 * @return true - датасет успешно загружен
 */
bool MlpModel::getDatasetLoaded() { return is_dataset_loaded_; }

bool MlpModel::getTestDatasetLoaded() { return is_test_dataset_loaded_; }

testResults MlpModel::getTestResults() { return test_results_; }

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

/**
 * @brief открывает датасет из .csl файла и переносит информацию
 * в параметр dataset_ в строковом виде,
 * считает размер датасета
 * @param filepath путь к файлу .csl
 */
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
    ifs.close();
  }
}

void MlpModel::openTestDataset(std::string filepath) {
  std::string line;
  test_dataset_.erase(test_dataset_.begin(), test_dataset_.end());
  is_test_dataset_loaded_ = false;
  test_dataset_size_ = 0;
  std::ifstream ifs(filepath);
  if (ifs.is_open()) {
    while (std::getline(ifs, line)) {
      if (line.length() > 1569) {
        test_dataset_.push_back(line);
        test_dataset_size_++;
      }
    }
    is_test_dataset_loaded_ = true;
    ifs.close();
  }
}

/**
 * @brief сохраняет модель (значения весов) только после обучения
 * @param filepath путь к файлу .txt
 */
bool MlpModel::saveModel(std::string filename) {
  return net_.save(filename.c_str());
};

/**
 * @brief парсит строку из датасета
 * @param line строка датасета
 * @param desired указатель под каким номером буквы должна быть данная строка (первое число строки)
 * @param data вектор в числовом представлении строки датасета после нормализации (от 0 до 1)
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
      data->coeffRef(0, y * width + x) = atof(substring.c_str()) / 255.0;
    }
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
bool MlpModel::train(GraphPerceptron &net, std::string line, int serial,
                     bool testing) {
  RowVector *input; // указатель на вектор где хранятся числовые значение входных нейронов
  int desired; // фактическое значение (номер буквы) текущей строки (первое число строки)
  readLetter(line, &desired, input);

  // создаем вектор-шаблон длиной 26 (количество букв)
  // который заподняем нулями, кроме элемента находящегося под индексом фактического значения
  RowVector output(LETTERS);
  for (int c = 0; c < LETTERS; c++)
    output.coeffRef(0, c) = c == desired ? 1 : 0;
  
  if (testing) {
    net.test(*input, output);
    net.evaluate(output);
  } else
    net.train(*input, output);
  delete input;

  // double value = 0;
  int actual = net.FindMaximum();
  cout << serial << ' ' << char(desired + 65) << " >> ";
  if (desired != actual)
    cout << CMD_RED << char(actual + 65) << CMD_RESET << endl;
  else
    cout << CMD_GREEN << char(actual + 65) << CMD_RESET << endl;

  return desired == actual;
}

void MlpModel::recognizeImage(std::string letter) {
  RowVector *input;  // указатель на вектор где хранятся числовые значение
                     // входных нейронов
  int desired;  // фактическое значение (номер буквы) текущей строки (первое
                // число строки)
  readLetter(letter, &desired, input);
  if (!is_model_valid_) {
    cout << "Model not loaded" << endl;
  } else {
    // создаем вектор-шаблон длиной 26 (количество букв)
    // который заподняем нулями, кроме элемента находящегося под индексом
    // фактического значения
    RowVector output(LETTERS);
    for (int c = 0; c < LETTERS; c++)
      output.coeffRef(0, c) = c == desired ? 1 : 0;
    net_.test(*input, output);
    net_.evaluate(output);
    delete input;

    // double value = 0;
    int actual = net_.FindMaximum() + 65;
    cout << CMD_GREEN << char(actual) << CMD_RESET << endl;
    recognizedLetter_ = actual;
  }
}

/**
 * @brief обучение модели
 * @param epoch количество эпох
 * @param hiden_layers количество скрытых слоев
*/
bool MlpModel::trainModel(int model_type, int epoch, int hiden_layers) {
  vector<int> init_vector{};
  if (!is_dataset_loaded_) {
    cout << "Dataset not loaded" << endl;
  } else {
    if (model_type == MATRIX_MODEL) {
      switch (hiden_layers) {  // количество нейронов подобрано эмпирически
        case 2:
          init_vector = {28 * 28, 64, 48, LETTERS};
          break;
        case 3:
          init_vector = {28 * 28, 120, 91, 48, LETTERS};
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
      net_.init(init_vector, 0.05);
      // 28 * 28, 64, 48, 26}, 0.03  2 - 76%  3 - 73% 4 - 73%
      // 28 * 28, 64, 52, 48, 26}, 0.01  3 - 68%
      // 28 * 28, 72, 64, 52, 48, 26}, 0.01 - 31%
      train(net_, epoch);
      // is_model_valid_ = true;
      return true;
    } else {
      cout << "Model in development" << endl;
    }
  }
  return false;
}

/**
 * @brief запуск тестирования на тестовом датасете
 * @param test_part (0-100) процент датасета который хотим протетсить
 * @return true - если тестирование завершилось
*/
bool MlpModel::testModel(int test_part) {
  bool result = false;
  if (!is_test_dataset_loaded_) {
    cout << "Test_dataset not loaded" << endl;
  } else if (!is_model_valid_) {
    cout << "Model not loaded" << endl;
  } else if (test_part == 0) {
    cout << "Nothing to test, test part ==0" << endl;
  } else {
    QElapsedTimer t;
    t.start();
    // для тестирования времени работы Part 2 (10, 100, 1000 раз)
    for (int i = 0; i < 1; ++i) {
      test(net_, test_part);
      evaluate(net_);
    }
    result = true;
    test_results_.runtime = t.elapsed();
    std::cout << "Operation in testModel " << t.elapsed() << "ms" << endl;
  }
  return result;
}

/**
 * @brief обучение модели и вывод результатов для каждой буквы
 * @param net указатель на нейросеть
 * @param epoch - количество эпох
*/
void MlpModel::train(GraphPerceptron &net, int epoch) {
  double cost = 0;
  int serial = 0; // номер строки датасета (буквы)
  int success = 0; // для каждой строки датасета (буквы) успешное определение 
  train_errors_.clear();
  // tain three times for better accuracy
  for (int trial = 0; trial < epoch; trial++) {
    for (size_t n = 0; n < dataset_size_; n++) {
      if (train(net, dataset_[n].c_str(), ++serial, false)) success++;
      cost += net.mse();
    }
    train_errors_.push_back((double)(serial - success) / serial * 100);
  }
  double error = (double)(serial - success) / serial * 100;
  cout << "TRAINING:" << endl;
  cout << "cost\t" << cost / (serial * 2) << endl;
  cout << "success\t" << success << endl;
  cout << "fail\t" << serial - success << endl;
  cout << "error\t" << error << "%" << endl;
  cout << endl;
  if (error < 30) is_model_valid_ = true;
}

/**
 * @brief тестирование тестового датасета на обученой модели
 * @param net указатель на нейросеть
 * @param @param test_part (0-100) процент датасета который хотим протетсить
*/
void MlpModel::test(GraphPerceptron &net, int test_part) {
  double cost = 0;
  int serial = 0, success = 0;
  for (size_t n = 0; n < test_dataset_size_ * test_part / 100; n++) {
    if (train(net, test_dataset_[n].c_str(), ++serial, true)) success++;
    cost += net.mse();
  }
  double error = (double)(serial - success) / serial * 100;
  test_results_.accuracy = 100 - error;
  cout << "TESTING:" << endl;
  cout << "cost\t" << cost / (serial * 2) << endl;
  cout << "success\t" << success << endl;
  cout << "fail\t" << serial - success << endl;
  cout << "error\t" << (double)(serial - success) / serial * 100 << "%" << endl;
  cout << endl;
}

/**
 * @brief вычисление параметров точности на модели для тестового датасета
 * @param net указатель на нейросеть
*/
void MlpModel::evaluate(GraphPerceptron &net) {
  RowVector *precision, *recall;
  net.confusionMatrix(precision, recall);

  int numValues = 0; // количество букв в тестах (может быть не равно 26)
  for (int i = 0; i < precision->size(); i++) {
    if (precision->coeffRef(i) != 0)
      numValues++;
  }

  double precisionVal = precision->sum() / numValues;
  double recallVal = recall->sum() / numValues;
  double f1score = 2 * precisionVal * recallVal / (precisionVal + recallVal);
  test_results_.precision = precisionVal;
  test_results_.recall = recallVal;
  test_results_.fmeasure = f1score;
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
