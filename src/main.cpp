#include "config.hpp"
#include "renderer.hpp"
#include "thread_pool.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  if (argc < 7) {
    std::cerr
        << "Uso: " << argv[0]
        << " <directorio con *.xy> nthreads xmin xmax ymin ymax [margin]\n";
    std::cerr << "Ejemplo: " << argv[0] << " ./frames 8 -10 10 -5 30 0.05\n";
    return 1;
  }

  std::string dir = argv[1];
  int nthreads = std::stoi(argv[2]);
  double xmin = std::stod(argv[3]);
  double xmax = std::stod(argv[4]);
  double ymin = std::stod(argv[5]);
  double ymax = std::stod(argv[6]);

  // margen opcional
  double margin = DEFAULT_MARGIN;
  if (argc >= 8) {
    margin = std::stod(argv[7]);
    if (margin < 0)
      margin = 0.0; // no permitir margen negativo
  }

  // número de hilos
  if (nthreads <= 0) {
    nthreads = std::thread::hardware_concurrency();
    if (nthreads == 0)
      nthreads = 2;
  }

  ThreadPool pool(nthreads);

  for (auto &entry : fs::directory_iterator(dir)) {
    if (entry.path().extension() == ".xy") {
      std::string inputFile = entry.path().string();
      auto outPath = entry.path();
      outPath.replace_extension(".png");
      std::string outputFile = outPath.string();

      pool.enqueue([=]() {
        renderFile(inputFile, outputFile, DEFAULT_WIDTH, DEFAULT_HEIGHT, xmin,
                   xmax, ymin, ymax, margin);
      });
    }
  }

  return 0;
}
