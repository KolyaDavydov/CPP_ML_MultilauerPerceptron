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
    // if (model_type_ == GRAPH_MODEL) {
    is_graph_model_valid_ = graph_net_.Load(filename.c_str());
    // } else {
    is_matrix_model_valid_ = matrix_net_.Load(filename.c_str());
    // }
  }
};

/**
 * @brief проверка удалось ли загрузить модель
 * @return true - модель успешно загруженна
 */
bool MlpModel::GetModelValid() {
  return (is_graph_model_valid_ || is_matrix_model_valid_);
}

QString MlpModel::GetErrorMsg() { return error_msg_; }

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
  RowVector *input;  // указатель на вектор где хранятся числовые значение
                     // входных нейронов
  int desired;  // фактическое значение (номер буквы) текущей строки (первое
                // число строки)
  ReadLetter(line, &desired, input);

  // создаем вектор-шаблон длиной 26 (количество букв)
  // который заподняем нулями, кроме элемента находящегося под индексом
  // фактического значения
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
  int serial = 0;   // номер строки датасета (буквы)
  int success = 0;  // для каждой строки датасета (буквы) успешное определение
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

  int numValues = 0;  // количество букв в тестах (может быть не равно 26)
  for (int i = 0; i < precision->size(); i++) {
    if (precision->coeffRef(i) != 0) numValues++;
  }

  // double precisionVal = precision->sum() / precision->cols();
  // double recallVal = recall->sum() / recall->cols();
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
  RowVector *input;  // указатель на вектор где хранятся числовые значение
                     // входных нейронов
  int desired;  // фактическое значение (номер буквы) текущей строки (первое
                // число строки)
  ReadLetter(line, &desired, input);

  // создаем вектор-шаблон длиной 26 (количество букв)
  // который заподняем нулями, кроме элемента находящегося под индексом
  // фактического значения
  RowVector output(LETTERS);
  for (int c = 0; c < LETTERS; c++)
    output.coeffRef(0, c) = c == desired ? 1 : 0;

  if (testing) {
    net.Test(*input, output);
    net.Evaluate(output);
  } else
    net.Train(*input, output);
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

void MlpModel::RecognizeImage(std::string letter) {
  RowVector *input;  // указатель на вектор где хранятся числовые значение
                     // входных нейронов
  int desired;  // фактическое значение (номер буквы) текущей строки (первое
                // число строки)
  ReadLetter(letter, &desired, input);
  if (!is_matrix_model_valid_ && !is_graph_model_valid_) {
    error_msg_ = "Model not loaded";
  } else {
    // создаем вектор-шаблон длиной 26 (количество букв)
    // который заподняем нулями, кроме элемента находящегося под индексом
    // фактического значения
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
      // 28 * 28, 64, 48, 26}, 0.03  2 - 76%  3 - 73% 4 - 73%
      // 28 * 28, 64, 52, 48, 26}, 0.01  3 - 68%
      // 28 * 28, 72, 64, 52, 48, 26}, 0.01 - 31%
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
    QElapsedTimer t;
    t.start();
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
    test_results_.runtime = t.elapsed();
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
  int serial = 0;   // номер строки датасета (буквы)
  int success = 0;  // для каждой строки датасета (буквы) успешное определение
  train_errors_.clear();
  // tain three times for better accuracy
  for (int trial = 0; trial < epoch; trial++) {
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
 * @param @param test_part (0-100) процент датасета который хотим протетсить
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

  int numValues = 0;  // количество букв в тестах (может быть не равно 26)
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
 * Сначала загружаем тренировочный датасет "Open Dataset", затем:
 * 1) делим загруженный датасет на k_value одинковых частей (при этом эти части
 * не пересекаются) минимум делим датасет на 5 частей. 2) 1 часть из всех будет
 * тестовым датасетом, остальные части помещаем в тестовый датасет 3) Всего
 * будет k_value итераций "обучение - тестирование" в первой итерации тестовым
 * датасетом будет первая часть, остальные треноровочным в следующей итерации
 * тестовым датасетом будет вторая часть, остальные тренировочными и так далее
 * пока тестовым датасетом не станет последняя часть.
 *
 * Для наглядности возьмем исходный датасет из 88 800 элементов и k_value = 5.
 * исходный датасет разделиться на 5 равных частей по 17 760 элементов
 * Таким образом в тестовой выборке будет 17 760 элементов,
 * а в тренировочной 71 040 элементов, причем они не пересекаются
 *
 * В каждой итерации происходит следующее:
 * 3.1)
 *    - происходит обучение на тренировочном датасете из 71 040 элементов
 *    - после обучения веса сохраняются во временный tmp_model.txt файл
 *    - загружаются веса из tmp_model.txt файла
 *    - на загруженной модели происходит тестирование на тестовом датасете из 17
 * 760 элементов
 *    - полученые показатели тестов сохраняются в вектор results
 *    - сравнивается показатель accuracy с показателем на предыдущей итерацией
 * или 0 и если точность после текущей иттерации выше, то эту модель становится
 * основной и когда все иттерации прошли после нажатии кнопки 'Save Model"
 * сохранится имено та модель accuracy которой выше
 */
std::vector<testResults> MlpModel::CrossValidation(int k_value, int epoch,
                                                   int hiden_layers) {
  // вектор для хранения результатов тестов для всех иттераций
  std::vector<testResults> results;
  // выбираем архитектуру для кросс-валидации так же как и при обычном
  // обучении
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
        init_vector = {28 * 28, 500, 300, 200, 100, LETTERS};
        break;
      case 5:
        init_vector = {28 * 28, 128, 64, 52, 48, 32, LETTERS};
        break;
      default:
        init_vector = {28 * 28, 64, 48, LETTERS};
    }
    // создаем вектор содержащий объекты перцептронов в количестве = k_value
    // и каждый объект инициализируем соответствующей архитектурой
    std::vector<GraphPerceptron> graph_parts(
        k_value);  // будет храниться несколько объектов перцептронов в
                   // количестве k_value
    std::vector<NeuralNetwork> matrix_parts(k_value);
    for (int i = 0; i < k_value; ++i) {
      if (model_type_ == GRAPH_MODEL) {
        graph_parts[i].init(init_vector, 0.01);
      } else {
        matrix_parts[i].init(init_vector, 0.01);
      }
    }
    // перемешиваем исходный датасет
    std::default_random_engine rng_{};
    std::shuffle(std::begin(dataset_), std::end(dataset_), rng_);
    // исходный датасет разбиваем на k_value не пересекающихся частей
    // и каждая часть будет находится в векторе datasets
    std::vector<std::vector<std::string>> datasets(k_value);
    int part_size = (int)(dataset_size_ / k_value);  // размер части датасета

    // процесс разбиения исходного датасета и помещения каждой части в datasets
    auto iter_begin = dataset_.begin();
    auto iter_end = dataset_.begin() + part_size;
    for (int i = 0; i < k_value; i++) {
      datasets[i].insert(datasets[i].begin(), iter_begin, iter_end);
      iter_begin = iter_end;
      iter_end = iter_begin + part_size;
    }

    double accuray_k = 0.0;  // Будет хранить показатель точности для каждой
                             // итерации и сравниваться с предыдущей
    const char *tmp_model = "tmp_model.txt";  // для временного хранения модели
    for (int i = 0; i < k_value; i++) {
      // при каждой иттерации мы как бы обновляем значения параметров текущего
      // класса
      this->dataset_.clear();
      this->test_dataset_.clear();

      // создаем тестовый датасет - одна часть из набора
      this->test_dataset_ = datasets[i];
      // создаем тренировочный датасет - остальные части кроме одной,
      auto iter = this->dataset_.cbegin();
      for (int j = 0; j < k_value; ++j) {
        if (j != i) {
          dataset_.insert(iter, datasets[j].begin(), datasets[j].end());
        }
      }

      // Обновляем размеры текущих датасетов
      dataset_size_ = dataset_.size();
      test_dataset_size_ = test_dataset_.size();
      if (model_type_ == GRAPH_MODEL) {
        Train(graph_parts[i], epoch);  // обучение
        graph_parts[i].Save(tmp_model);  // сохранение модели в временный файл
        graph_parts[i].Load(tmp_model);  // загрузка модели из временного файла
        Test(graph_parts[i], 100);  // тестирвоание на загруженной модели
        Evaluate(graph_parts[i]);  // определение метрик после тестов
      } else {
        Train(matrix_parts[i], epoch);  // обучение
        matrix_parts[i].Save(tmp_model);  // сохранение модели в временный файл
        matrix_parts[i].Load(tmp_model);  // загрузка модели из временного файла
        Test(matrix_parts[i], 100);  // тестирвоание на загруженной модели
        Evaluate(matrix_parts[i]);  // определение метрик после тестов
      }
      results.push_back(
          this->test_results_);  // запихиваем полученные метрики в вектор
      // сравниваем метрику accuracy c предыдущей иттерацией
      // если она выше, то как бы загружаем модель в основной перцептрон (net_)
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
