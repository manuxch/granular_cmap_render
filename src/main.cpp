#include "thread_pool.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  std::string inputDir = ".";
  std::string property = "pressure"; // valor por defecto

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--dir" && i + 1 < argc) {
      inputDir = argv[++i];
    } else if (arg == "--property" && i + 1 < argc) {
      property = argv[++i];
    } else if (arg == "--help") {
      std::cout << "Uso: " << argv[0]
                << " [--dir directorio] [--property propiedad]\n";
      return 0;
    }
  }

  std::cout << "Directorio: " << inputDir << "\n";
  std::cout << "Propiedad: " << property << "\n";

  // Inicializar thread pool con #hilos = #cores
  size_t numThreads = std::thread::hardware_concurrency();
  ThreadPool pool(numThreads);

  std::vector<std::future<void>> futures;

  // Recorremos archivos .xy
  for (const auto &entry : fs::directory_iterator(inputDir)) {
    if (entry.path().extension() == ".xy") {
      std::string xyFile = entry.path().string();

      // Nombre del archivo .sxy apareado
      std::string sxyFile = xyFile.substr(0, xyFile.size() - 3) + "sxy";

      futures.push_back(pool.enqueue([xyFile, sxyFile, property] {
        // ⬇️ Aquí luego usaremos Renderer
        std::cout << "Procesando: " << xyFile << " con " << sxyFile
                  << " (propiedad = " << property << ")\n";
      }));
    }
  }

  for (auto &f : futures) {
    f.get();
  }

  return 0;
}
