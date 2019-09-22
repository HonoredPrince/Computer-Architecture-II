#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <windows.h>

//Variáveis globais, configuração desejada deve ser modificada aqui
static const long M_SIZE = 2;
static const int NUMERO_THREADS = 1;
static const long N_EXECUCOES = 1e3;

//Estrutura da matriz
struct Matrix {
  float ** valores;

  void set_default_matrix() {
    valores = new float*[M_SIZE];
    for (int i = 0; i < M_SIZE; ++i) {
      valores[i] = new float[M_SIZE];
      for (int j = 0; j < M_SIZE; ++j) {
        valores[i][j] = 0.0f;
      }
    }
  }

  void set_random_matrix() {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(-1e9, -1e9);
    auto random = std::bind(dist, mt);
    valores = new float*[M_SIZE];
    for (int i = 0; i < M_SIZE; ++i) {
      valores[i] = new float[M_SIZE];
      for (int j = 0; j < M_SIZE; ++j) {
        valores[i][j] = random();
      }
    }
  }

  void print() {
    std::cout << std::endl;
    for (int i = 0; i < M_SIZE; ++i) {
      std::cout << "|\t";

      for (int j = 0; j < M_SIZE; ++j) {
        std::cout << valores[i][j] << "\t";
      }
      std::cout << "|" << std::endl;
    }
  }

};

void multiplicacao_paralela(Matrix& resultado, const int thread_number, const Matrix& m1, const Matrix& m2);
void execucao_paralela(Matrix& r, long long& tempo_ocorrido, const Matrix& m1, const Matrix& m2);
void benchmark(void(*execution_function)(Matrix& r, long long& tempo_ocorrido, const Matrix& m1, const Matrix& m2));
long long tempo_atual();

int main() {
  //std::cout << "Linear Execution" << std::endl;
  //benchmark(execucao_linear);
  std::cout << "Multithread Execution" << std::endl;
  std::cout << "Number Of Threads: " << NUMERO_THREADS << std::endl;
  std::cout << "Matrix Sizes: " << M_SIZE << std::endl;
  benchmark(execucao_paralela);
  Sleep(100000);
  std::cout << "Fim do programa" << std::endl;
}

void benchmark(void(*execution_function)(Matrix& r, long long& tempo_ocorrido, const Matrix& m1, const Matrix& m2)) {
  Matrix m1, m2, r;

  long long tempo_total = 0.0;
  for (int i = 0; i < N_EXECUCOES; ++i) {
    long long tempo_ocorrido = 0.0;
    m1.set_random_matrix();
    m2.set_random_matrix();
    r.set_default_matrix();

    execution_function(r, tempo_ocorrido, m1, m2);
    tempo_total += tempo_ocorrido;
  }
  std::cout << "\tAVG Time percorrido\t" << (double) tempo_total / N_EXECUCOES << " milisegundos" << std::endl;
}

void multiplicacao_paralela(Matrix& resultado, const int thread_number, const Matrix& m1, const Matrix& m2) {
  const int n_valores = (M_SIZE * M_SIZE);
  const int n_operations = n_valores / NUMERO_THREADS;
  const int rest_operations = n_valores % NUMERO_THREADS;

  int start_op, end_op;

  if (thread_number == 0) {
    start_op = n_operations * thread_number;
    end_op = (n_operations * (thread_number + 1)) + rest_operations;
  }
  else {
    start_op = n_operations * thread_number + rest_operations;
    end_op = (n_operations * (thread_number + 1)) + rest_operations;
  }

  for (int op = start_op; op < end_op; ++op) {
    const int row = op % M_SIZE;
    const int col = op / M_SIZE;
    float r = 0.0f;
    for (int i = 0; i < M_SIZE; ++i) {
      const float e1 = m1.valores[row][i];
      const float e2 = m2.valores[i][col];
      r += e1 * e2;
    }

    resultado.valores[row][col] = r;
  }
}

void execucao_paralela(Matrix& r, long long& tempo_ocorrido, const Matrix& m1, const Matrix& m2) {
  //std::cout << "Começando execução multithreading..." << std::endl;
  long long tempo_de_inicio = tempo_atual();

  std::thread threads[NUMERO_THREADS];

  for (int i = 0; i < NUMERO_THREADS; ++i) {
    //std::cout << "Inicializando thread " << i << std::endl;
    threads[i] = std::thread(multiplicacao_paralela, std::ref(r), i, std::ref(m1), std::ref(m2));
  }

  //std::cout << "Calculando...." << std::endl;

  for (int i = 0; i < NUMERO_THREADS; ++i) {
    //std::cout << "Join no thread " << i << std::endl;
    threads[i].join();
  }

  long long tempo_de_termino = tempo_atual();
  //std::cout << "Término da execução multithreading" << std::endl;

  tempo_ocorrido = tempo_de_termino - tempo_de_inicio;
}

long long tempo_atual() {
  static LARGE_INTEGER s_frequency;
  static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
  if (s_use_qpc) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (1000LL * now.QuadPart) / s_frequency.QuadPart;
  }
  else {
    return GetTickCount();
  }
}
