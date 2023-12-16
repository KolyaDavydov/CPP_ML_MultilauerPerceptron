#include "model.h"

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
void MlpModel::OpenModel(std::string filename) {
  is_graph_model_valid_ = is_matrix_model_valid_ = false;
  file_.open(filename);
  if (file_.is_open()) {
    is_graph_model_valid_ = graph_net_.Load(filename.c_str());
    is_matrix_model_valid_ = matrix_net_.Load(filename.c_str());
  }
};

/**
 * @brief проверка удалось ли загрузить модель
 * @return true - модель успешно загруженна
 */
bool MlpModel::GetModelValid() {
  return (is_graph_model_valid_ || is_matrix_model_valid_);
}

std::string MlpModel::GetErrorMsg() { return error_msg_; }

void MlpModel::SetModelType(int model_type) { model_type_ = model_type; }

/**
 * @brief проверка удалось ли загрузить датасет
 * @return true - датасет успешно загружен
 */
bool MlpModel::GetDatasetLoaded() { return is_dataset_loaded_; }

bool MlpModel::GetTestDatasetLoaded() { return is_test_dataset_loaded_; }

testResults MlpModel::GetTestResults() { return test_results_; }

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
void MlpModel::OpenDataset(std::string filepath) {
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

void MlpModel::OpenTestDataset(std::string filepath) {
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
bool MlpModel::SaveModel(std::string filename) {
  error_msg_ = "";
  if (model_type_ == GRAPH_MODEL && is_graph_model_valid_) {
    return graph_net_.Save(filename.c_str());
  } else {
    if (is_matrix_model_valid_) {
      return matrix_net_.Save(filename.c_str());
    } else {
      error_msg_ = "Model not loaded";
      return false;
    }
  }
};

/**
 * @brief парсит строку из датасета
 * @param line строка датасета
 * @param desired указатель под каким номером буквы должна быть данная строка
 * (первое число строки)
 * @param data вектор в числовом представлении строки датасета после
 * нормализации (от 0 до 1)
 */
void MlpModel::ReadLetter(const std::string line, int *desired,
                          RowVector *&data) {
  int width = 28;
  int height = 28;
  std::string substring;
  std::stringstream ss;
  data = new RowVector(width * height);
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
bool MlpModel::Train(NeuralNetwork &net, std::string line, int serial,
                     bool testing) {
  RowVector *input;
  int desired;
  ReadLetter(line, &desired, input);
  RowVector output(LETTERS);
  for (int c = 0; c < LETTERS; c++)
    output.coeffRef(0, c) = c == desired ? 1 : 0;

  if (testing) {
    net.Test(*input, output);
    net.Evaluate(output);
  } else
    net.Train(*input, output);
  delete input;

  double value = 0;
  int actual = net.Vote(value);
  cout << serial << ' ' << char(desired + 65) << " >> ";
  if (desired != actual)
    cout << CMD_RED << char(actual + 65) << CMD_RESET " ("
         << value - net.Output(desired) << ")" << endl;
  else
    cout << CMD_GREEN << char(actual + 65) << CMD_RESET << endl;

  return desired == actual;
}

void MlpModel::Train(NeuralNetwork &net, int epoch) {
  double cost = 0;
  int serial = 0;
  int success = 0;
  train_errors_.clear();
  for (int trial = 0; trial < epoch; trial++) {
    serial = 0;
    success = 0;
    std::default_random_engine rng_{};
    std::shuffle(std::begin(dataset_), std::end(dataset_), rng_);
    for (size_t n = 0; n < dataset_size_; n++) {
      if (Train(net, dataset_[n].c_str(), ++serial, false)) success++;
      cost += net.Mse();
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
  if (error < 30) is_matrix_model_valid_ = true;
}

void MlpModel::Test(NeuralNetwork &net, int test_part) {
  double cost = 0;
  int serial = 0, success = 0;
  for (size_t n = 0; n < test_dataset_size_ * test_part / 100; n++) {
    if (Train(net, test_dataset_[n].c_str(), ++serial, true)) success++;
    cost += net.Mse();
  }
  double error = (double)(serial - success) / serial * 100;
  test_results_.accuracy = 100 - error;
  cout << "TESTING:" << endl;
  cout << "cost\t" << cost / (serial * 2) << endl;
  cout << "success\t" << success << endl;
  cout << "fail\t" << serial - success << endl;
  cout << "error\t" << error << "%" << endl;
  cout << endl;
}

void MlpModel::Evaluate(NeuralNetwork &net) {
  RowVector *precision, *recall;
  net.ConfusionMatrix(precision, recall);

  int numValues = 0;
  for (int i = 0; i < precision->size(); i++) {
    if (precision->coeffRef(i) != 0) numValues++;
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

/**
 * @brief функция тренировки для каждой строки (буквы датасета)
 * @param net указатель на нейросеть
 * @param line строка (буква) датасета
 * @param serial номер строки (буквы) датасета
 * @param testing если true - тестируемый датасет, false - тренировочный датасет
 *
 * @return true - если строка (буква) успешно прошла тренировку или тетсирование
 */
bool MlpModel::Train(GraphPerceptron &net, std::string line, int serial,
                     bool testing) {
  RowVector *input;
  int desired;
  ReadLetter(line, &desired, input);
  RowVector output(LETTERS);
  for (int c = 0; c < LETTERS; c++)
    output.coeffRef(0, c) = c == desired ? 1 : 0;

  if (testing) {
    net.Test(*input, output);
    net.Evaluate(output);
  } else
    net.Train(*input, output);
  delete input;
  int actual = net.FindMaximum();
  cout << serial << ' ' << char(desired + 65) << " >> ";
  if (desired != actual)
    cout << CMD_RED << char(actual + 65) << CMD_RESET << endl;
  else
    cout << CMD_GREEN << char(actual + 65) << CMD_RESET << endl;

  return desired == actual;
}

void MlpModel::RecognizeImage(std::string letter) {
  RowVector *input;
  int desired;
  ReadLetter(letter, &desired, input);
  if (!is_matrix_model_valid_ && !is_graph_model_valid_) {
    error_msg_ = "Model not loaded";
  } else {
    RowVector output(LETTERS);
    for (int c = 0; c < LETTERS; c++)
      output.coeffRef(0, c) = c == desired ? 1 : 0;
    if (model_type_ == GRAPH_MODEL) {
      graph_net_.Test(*input, output);
      graph_net_.Evaluate(output);
    } else {
      matrix_net_.Test(*input, output);
      matrix_net_.Evaluate(output);
    }
    delete input;

    int actual;
    if (model_type_ == GRAPH_MODEL) {
      actual = graph_net_.FindMaximum() + 65;
    } else {
      double value = 0;
      actual = matrix_net_.Vote(value) + 65;
    }
    cout << CMD_GREEN << char(actual) << CMD_RESET << endl;
    recognizedLetter_ = actual;
    error_msg_ = "";
  }
};

/**
 * @brief обучение модели
 * @param epoch количество эпох
 * @param hiden_layers количество скрытых слоев
 */
void MlpModel::TrainModel(int epoch, int hiden_layers) {
  vector<int> init_vector{};
  if (!is_dataset_loaded_) {
    error_msg_ = "Dataset not loaded";
  } else {
    switch (hiden_layers) {  // количество нейронов подобрано эмпирически
      case 2:
        init_vector = {28 * 28, 64, 48, LETTERS};
        break;
      case 3:
        init_vector = {28 * 28, 120, 91, 48, LETTERS};
        break;
      case 4:
        // init_vector = {28 * 28, 64, 52, 48, 40, LETTERS};
        init_vector = {28 * 28, 500, 300, 200, 100, LETTERS};
        break;
      case 5:
        init_vector = {28 * 28, 128, 64, 52, 48, 32, LETTERS};
        break;
      default:
        init_vector = {28 * 28, 64, 48, LETTERS};
    }
    if (model_type_ == GRAPH_MODEL) {
      graph_net_.init(init_vector, 0.05);
      Train(graph_net_, epoch);
    } else {
      matrix_net_.init(init_vector, 0.05);
      Train(matrix_net_, epoch);
      error_msg_ = "";
    }
  }
}

/**
 * @brief запуск тестирования на тестовом датасете
 * @param test_part (0-100) процент датасета который хотим протетсить
 * @return true - если тестирование завершилось
 */
bool MlpModel::TestModel(int test_part) {
  bool result = false;
  if (!is_test_dataset_loaded_) {
    error_msg_ = "Test_dataset not loaded";
  } else if (!is_matrix_model_valid_ && !is_graph_model_valid_) {
    error_msg_ = "Model not loaded";
  } else if (test_part == 0) {
    error_msg_ = "Nothing to test, test part ==0";
  } else {
    auto start_time = std::chrono::steady_clock::now();
    // для тестирования времени работы Part 2 (10, 100, 1000 раз)
    for (int i = 0; i < 1; ++i) {
      if (model_type_ == GRAPH_MODEL) {
        Test(graph_net_, test_part);
        Evaluate(graph_net_);
      } else {
        Test(matrix_net_, test_part);
        Evaluate(matrix_net_);
      }
    }
    result = true;
    auto end_time = std::chrono::steady_clock::now();
    auto timer = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    test_results_.runtime = timer.count() / 1000.0;
    error_msg_ = "";
  }
  return result;
}

/**
 * @brief обучение модели и вывод результатов для каждой буквы
 * @param net указатель на нейросеть
 * @param epoch - количество эпох
 */
void MlpModel::Train(GraphPerceptron &net, int epoch) {
  double cost = 0;
  int serial = 0;
  int success = 0;
  train_errors_.clear();
  for (int trial = 0; trial < epoch; trial++) {
    serial = 0;
    success = 0;
    std::default_random_engine rng_{};
    std::shuffle(std::begin(dataset_), std::end(dataset_), rng_);
    for (size_t n = 0; n < dataset_size_; n++) {
      if (Train(net, dataset_[n].c_str(), ++serial, false)) success++;
      cost += net.Mse();
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
  if (error < 30) is_graph_model_valid_ = true;
}

/**
 * @brief тестирование тестового датасета на обученой модели
 * @param net указатель на нейросеть
 * @param test_part (0-100) процент датасета который хотим протетсить
 */
void MlpModel::Test(GraphPerceptron &net, int test_part) {
  double cost = 0;
  int serial = 0, success = 0;
  for (size_t n = 0; n < test_dataset_size_ * test_part / 100; n++) {
    if (Train(net, test_dataset_[n].c_str(), ++serial, true)) success++;
    cost += net.Mse();
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
void MlpModel::Evaluate(GraphPerceptron &net) {
  RowVector *precision, *recall;
  net.ConfusionMatrix(precision, recall);

  int numValues = 0;
  for (int i = 0; i < precision->size(); i++) {
    if (precision->coeffRef(i) != 0) numValues++;
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

/**
 * @brief для обучения исопльзуется кросс-валидация:
 * @param  k_value части датасета
 * @param epoch количество эпох
 * @param hiden_layers количество скрытых слоев
 */
std::vector<testResults> MlpModel::CrossValidation(int k_value, int epoch,
                                                   int hiden_layers) {
  std::vector<testResults> results;
  vector<int> init_vector{};
  if (!is_dataset_loaded_) {
    error_msg_ = "Dataset not loaded";
  } else {
    switch (hiden_layers) {
      case 2:
        init_vector = {28 * 28, 64, 48, LETTERS};
        break;
      case 3:
        init_vector = {28 * 28, 120, 91, 48, LETTERS};
        break;
      case 4:
        init_vector = {28 * 28, 500, 300, 200, 100, LETTERS};
        break;
      case 5:
        init_vector = {28 * 28, 128, 64, 52, 48, 32, LETTERS};
        break;
      default:
        init_vector = {28 * 28, 64, 48, LETTERS};
    }
    std::vector<GraphPerceptron> graph_parts(k_value);
    std::vector<NeuralNetwork> matrix_parts(k_value);
    for (int i = 0; i < k_value; ++i) {
      if (model_type_ == GRAPH_MODEL) {
        graph_parts[i].init(init_vector, 0.01);
      } else {
        matrix_parts[i].init(init_vector, 0.01);
      }
    }
    std::default_random_engine rng_{};
    std::shuffle(std::begin(dataset_), std::end(dataset_), rng_);
    std::vector<std::vector<std::string>> datasets(k_value);
    int part_size = (int)(dataset_size_ / k_value);
    auto iter_begin = dataset_.begin();
    auto iter_end = dataset_.begin() + part_size;
    for (int i = 0; i < k_value; i++) {
      datasets[i].insert(datasets[i].begin(), iter_begin, iter_end);
      iter_begin = iter_end;
      iter_end = iter_begin + part_size;
    }

    double accuray_k = 0.0;
    const char *tmp_model = "tmp_model.txt";
    for (int i = 0; i < k_value; i++) {
      this->dataset_.clear();
      this->test_dataset_.clear();
      this->test_dataset_ = datasets[i];
      auto iter = this->dataset_.cbegin();
      for (int j = 0; j < k_value; ++j) {
        if (j != i) {
          dataset_.insert(iter, datasets[j].begin(), datasets[j].end());
        }
      }
      dataset_size_ = dataset_.size();
      test_dataset_size_ = test_dataset_.size();
      if (model_type_ == GRAPH_MODEL) {
        Train(graph_parts[i], epoch);
        graph_parts[i].Save(tmp_model);
        graph_parts[i].Load(tmp_model);
        Test(graph_parts[i], 100);
        Evaluate(graph_parts[i]);
      } else {
        Train(matrix_parts[i], epoch);
        matrix_parts[i].Save(tmp_model);
        matrix_parts[i].Load(tmp_model);
        Test(matrix_parts[i], 100);
        Evaluate(matrix_parts[i]);
      }
      results.push_back(this->test_results_);
      if (results[i].accuracy > accuray_k) {
        if (model_type_ == GRAPH_MODEL) {
          graph_net_.Load(tmp_model);
        } else {
          matrix_net_.Load(tmp_model);
        }
        accuray_k = results[i].accuracy;
      }
      error_msg_ = "";
    }
  }
  return results;
};

}  // namespace s21
