// main.cpp
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <future>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "thread_pool.hpp"   // tu implementación de ThreadPool (header-only preferible)
#include "parser.hpp"
#include "renderer.hpp"
#include "colormap.hpp"      // viridis(), inferno(), RdYlBu(), ...

namespace fs = std::filesystem;

// --------- Helpers ---------
static void readConfigFile(const std::string& fname, std::unordered_map<std::string,std::string>& out) {
    std::ifstream fin(fname);
    if (!fin) return;
    std::string line;
    while (std::getline(fin, line)) {
        // Trim
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        if (line[start] == '#') continue;

        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq+1);

        // trim whitespace
        auto trim = [](std::string &s) {
            auto a = s.find_first_not_of(" \t\r\n");
            auto b = s.find_last_not_of(" \t\r\n");
            if (a==std::string::npos) { s=""; return; }
            s = s.substr(a, b-a+1);
        };
        trim(key); trim(val);
        if (!key.empty()) out[key] = val;
    }
}

static Colormap chooseColormap(const std::string& name) {
    if (name == "viridis") return viridis();
    if (name == "inferno")  return inferno();
    if (name == "RdYlBu")   return RdYlBu();
    if (name == "Greens")   return Greens();
    if (name == "Reds")     return Reds();
    if (name == "winter")   return winter();
    if (name == "autumn")   return autumn();
    if (name == "Blues")    return Blues();
    if (name == "hot")      return hot();
    // default
    std::cerr << "[WARN] Colormap '" << name << "' no reconocido. Usando viridis.\n";
    return viridis();
}

// --------- Main ---------
int main(int argc, char* argv[]) {
    // Default parameters
    std::string inputDir = ".";
    std::string property = "pressure";
    std::string cmapName = "viridis";
    std::string configFile = "config.txt";
    std::string outputDir = "renders";
    int width = 1000;
    int height = 1000;
    double margin = 40.0; // pixels
    double xmin = -10.0;
    double xmax = 10.0;
    double ymin = -10.0;
    double ymax = 20.0;

    // Simple argv parsing
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if ((a == "--dir" || a == "-d") && i + 1 < argc) { inputDir = argv[++i]; }
        else if ((a == "--property" || a == "--prop" || a == "-p") && i + 1 < argc) { property = argv[++i]; }
        else if ((a == "--cmap") && i + 1 < argc) { cmapName = argv[++i]; }
        else if ((a == "--config") && i + 1 < argc) { configFile = argv[++i]; }
        else if ((a == "--out" || a == "--output") && i + 1 < argc) { outputDir = argv[++i]; }
        else if ((a == "--width") && i + 1 < argc) { width = std::stoi(argv[++i]); }
        else if ((a == "--height") && i + 1 < argc) { height = std::stoi(argv[++i]); }
        else if ((a == "--margin") && i + 1 < argc) { margin = std::stod(argv[++i]); }
        else if ((a == "--xylimits" || a == "-xyl") && (i + 4 < argc)) {
            xmin = std::stod(argv[++i]);
            xmax = std::stod(argv[++i]);
            ymin = std::stod(argv[++i]);
            ymax = std::stod(argv[++i]);
        }
        else if ((a == "--help" || a == "-h") || (argc < 1))  {
            std::cout << "Usage: " << argv[0] << " [--dir <input_dir>] [--property <name>]\n"
                      << "       [--cmap <viridis|inferno|RdYlBu|Greens|Reds|winter|autumn|blues|hot>]\n"
                      << "       [--config <file>]\n"
                      << "       [--out <out_dir>] [--width <px>] [--height <px>] [--margin <px>]\n"
                      << "       [--xylimits xmin xmax ymin ymax]\n";
            return 0;
        }
    }

    // Read config.txt overriding defaults if present
    std::unordered_map<std::string,std::string> cfg;
    readConfigFile(configFile, cfg);
    if (cfg.count("output_dir")) outputDir = cfg["output_dir"];
    if (cfg.count("colormap")) cmapName = cfg["colormap"];
    if (cfg.count("width")) width = std::stoi(cfg["width"]);
    if (cfg.count("height")) height = std::stoi(cfg["height"]);
    if (cfg.count("margin")) margin = std::stod(cfg["margin"]);
    if (cfg.count("property")) property = cfg["property"];
    if (cfg.count("x_min")) xmin = std::stod(cfg["x_min"]);
    if (cfg.count("x_max")) xmax = std::stod(cfg["x_max"]);
    if (cfg.count("y_min")) ymin = std::stod(cfg["y_min"]);
    if (cfg.count("y_max")) ymax = std::stod(cfg["y_max"]);

    // Make output dir if needed
    try {
        if (!fs::exists(outputDir)) fs::create_directories(outputDir);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] No se pudo crear output dir '" << outputDir << "': " << e.what() << "\n";
        return 1;
    }

    std::cout << "Input dir : " << inputDir << "\n";
    std::cout << "Output dir: " << outputDir << "\n";
    std::cout << "Property  : " << property << "\n";
    std::cout << "Colormap  : " << cmapName << "\n";
    std::cout << "Image     : " << width << "x" << height << " (margin " << margin << " px)\n";
    std::cout << "Límites   : " << xmin << " " << xmax << " " << ymin << " " << ymax << " (s.u. - m)\n";

    // choose colormap
    Colormap cmap = chooseColormap(cmapName);

    // thread pool (num threads = hardware concurrency or 4)
    size_t nthreads = std::thread::hardware_concurrency();
    if (nthreads == 0) nthreads = 4;
    ThreadPool pool(nthreads);

    std::vector<std::future<void>> futures;

    // iterate directory and enqueue tasks for each .xy
    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (!entry.is_regular_file()) continue;
        auto path = entry.path();
        if (path.extension() != ".xy") continue;

        std::string xyFile = path.string();
        std::string base = xyFile.substr(0, xyFile.size() - 3); // remove .xy
        std::string sxyFile = base + ".sxy";
        std::string filenameOnly = path.filename().string();
        std::string outFile = fs::path(outputDir) / (path.stem().string() + ".png");

        // enqueue job
        futures.push_back(pool.enqueue([xyFile, sxyFile, outFile, property, width, height, margin, cmap,
                                        xmin, xmax, ymin, ymax]() mutable {
            try {
                // Check sxy exists
                if (!fs::exists(sxyFile)) {
                    std::cerr << "[WARN] Missing paired file: " << sxyFile << " (skipping " << xyFile << ")\n";
                    return;
                }

                // Read sxy raw data (gid -> vector<double>)
                auto sxyData = Parser::readSXY(sxyFile);

                // Build grains from xy and associated scalars
                auto grains = Parser::readXY(xyFile, sxyData, property);

                if (grains.empty()) {
                    std::cerr << "[WARN] No grains parsed from " << xyFile << "\n";
                    return;
                }

                // Determine vmin/vmax from grains' scalars
                double vmin =  1e300;
                double vmax = -1e300;
                for (const auto &gptr : grains) {
                    double v = gptr->scalar();
                    if (v < vmin) vmin = v;
                    if (v > vmax) vmax = v;
                }
                if (vmin == 1e300 || vmax == -1e300) {
                    vmin = 0.0; vmax = 1.0;
                }
                if (vmin == vmax) { // avoid degenerate range
                    double eps = std::abs(vmin) * 1e-6 + 1e-6;
                    vmin -= eps; vmax += eps;
                }

                // Create renderer and render
                Renderer renderer(width, height, margin);
                renderer.renderToPNG(outFile, grains, vmin, vmax, xmin, xmax, ymin, ymax, cmap);

                std::cout << "[OK] " << outFile << "\n";
            } catch (const std::exception &e) {
                std::cerr << "[ERROR] processing " << xyFile << ": " << e.what() << "\n";
            }
        }));
    }

    // wait for tasks
    for (auto &f : futures) {
        try { f.get(); } catch (const std::exception &e) {
            std::cerr << "[ERROR] task exception: " << e.what() << "\n";
        }
    }

    std::cout << "All tasks done.\n";
    return 0;
}

