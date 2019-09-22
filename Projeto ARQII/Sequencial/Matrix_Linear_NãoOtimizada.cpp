#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <windows.h>

//Variáveis globais, configuração desejada deve ser modificada aqui
static const long M_SIZE = 100;
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

void multiplicacao_linear(Matrix& r, const Matrix& m1, const Matrix& m2);
void execucao_linear(Matrix& r, long long& tempo_ocorrido, const Matrix& m1, const Matrix& m2);
void benchmark(void(*execution_function)(Matrix& r, long long& tempo_ocorrido, const Matrix& m1, const Matrix& m2));
long long tempo_atual();

int main() {
  std::cout << "Linear Execution" << std::endl;
  std::cout << "Matrix Sizes: " << M_SIZE << std::endl;
  benchmark(execucao_linear);
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

/*void multiplicacao_linear(Matrix& r, const Matrix& m1, const Matrix& m2) {
  for (int i = 0; i < M_SIZE; ++i) {
    for (int j = 0; j < M_SIZE; ++j) {
      float resultado = 0.0f;
      for (int k = 0; k < M_SIZE; ++k) {
        const float e1 = m1.valores[i][k];
        const float e2 = m2.valores[k][j];
        resultado += e1 * e2;
      }
      r.valores[i][j] = resultado;
    }
  }
}*/

void multiplicacao_linear(Matrix& r, const Matrix& m1, const Matrix& m2) {
  for (int k = 0; k < M_SIZE; ++k)
    for (int i = 0; i < M_SIZE; ++i) {
      for (int j = 0; j < M_SIZE; ++j) {
        r.valores[i][j] = m1.valores[j][i] * m2.valores[j][i];
      }
    }
}

void execucao_linear(Matrix& r, long long& tempo_ocorrido, const Matrix& m1, const Matrix& m2) {
  //std::cout << "Começando execução linear" << std::endl;
  long long tempo_de_inicio = tempo_atual();

  //std::cout << "Calculando..." << std::endl;
  multiplicacao_linear(r, m1, m2);

  long long tempo_de_termino = tempo_atual();
  //std::cout << "Fim da execução linear" << std::endl;

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
