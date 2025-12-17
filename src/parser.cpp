#include "parser.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

// ---------------- readSXY ----------------
std::unordered_map<int, std::vector<double>>
Parser::readSXY(const std::string &filename) {
  std::unordered_map<int, std::vector<double>> data;
  std::ifstream fin(filename);
  if (!fin) {
    std::cerr << "Error al abrir " << filename << "\n";
    return data;
  }

  std::string line;
  while (std::getline(fin, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream iss(line);
    int gid;
    iss >> gid;
    std::vector<double> vals;
    double tmp;
    while (iss >> tmp)
      vals.push_back(tmp);
    data[gid] = vals;
  }
  return data;
}

// ---------------- computeProperty ----------------
double Parser::computeProperty(const std::string &property,
                               const std::vector<double> &values) {
  if (property == "pressure") {
    // stress tensor σ_xx, σ_yy, σ_xy
    if (values.size() >= 3) {
      double sxx = values[0], syy = values[1];
      return -(sxx + syy) / 2.0;
    }
  } else if (property == "kinetic_energy") {
    // valores: vx, vy, mass
    if (values.size() >= 3) {
      double vx = values[0], vy = values[1], m = values[2];
      return 0.5 * m * (vx * vx + vy * vy) *
             0.000245; // TODO  Conversión a unidades exp. (m v^2)
    }
  } else if (property == "velocity_norm") {
    if (values.size() >= 2) {
      double vx = values[3], vy = values[4];
      return -vy * 0.2213594;
      // return std::sqrt(vx * vx + vy * vy) *
      //        0.2213594; // TODO: Factor de conversión a unidades exp. (F/L)
    }
  }
  // valor por defecto
  return values.empty() ? 0.0 : values[0];
}

// ---------------- readXY ----------------
std::vector<std::unique_ptr<Grain>>
Parser::readXY(const std::string &filename,
               const std::unordered_map<int, std::vector<double>> &scalarData,
               const std::string &property) {
  std::vector<std::unique_ptr<Grain>> grains;
  std::ifstream fin(filename);
  if (!fin) {
    std::cerr << "Error al abrir " << filename << "\n";
    return grains;
  }

  std::string line;
  while (std::getline(fin, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream iss(line);
    double scalar = 0.0;

    int gid, nvert, type;
    iss >> gid >> nvert;

    if (gid < 0) {
      // Polígono especial BOX, lo ignoramos por ahora
      std::vector<std::pair<double, double>> vertices;
      for (int i = 0; i < nvert; i++) {
        double vx, vy;
        iss >> vx >> vy;
        vertices.emplace_back(vx, vy);
      }
      std::string word;
      iss >> word;
      scalar = -1.0;
      grains.push_back(
          std::make_unique<BorderGrain>(gid, type, vertices, scalar));
    }

    auto it = scalarData.find(gid);
    if (it != scalarData.end()) {
      scalar = computeProperty(property, it->second);
    }

    if (nvert == 1) {
      // círculo
      double x, y, r;
      iss >> x >> y >> r >> type;
      grains.push_back(
          std::make_unique<CircleGrain>(gid, type, x, y, r, scalar));
    } else {
      // polígono
      std::vector<std::pair<double, double>> vertices;
      for (int i = 0; i < nvert; i++) {
        double vx, vy;
        iss >> vx >> vy;
        vertices.emplace_back(vx, vy);
      }
      iss >> type;
      grains.push_back(
          std::make_unique<PolygonGrain>(gid, type, vertices, scalar));
    }
  }
  return grains;
}
