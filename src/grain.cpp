#include "grain.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

// -----------------------------------------------------------------
// Render de CircleGrain
// -----------------------------------------------------------------
void CircleGrain::render(cairo_t *cr, const Color &c, double scale,
                         double offsetX, double offsetY) const {
  cairo_set_source_rgb(cr, c.r, c.g, c.b);
  cairo_arc(cr, offsetX + x * scale, offsetY + y * scale, r * scale, 0,
            2 * M_PI);
  cairo_fill(cr);
}

// -----------------------------------------------------------------
// Render de PolygonGrain
// -----------------------------------------------------------------
void PolygonGrain::render(cairo_t *cr, const Color &c, double scale,
                          double offsetX, double offsetY) const {
  if (vertices.empty())
    return;
  cairo_set_source_rgb(cr, c.r, c.g, c.b);

  cairo_move_to(cr, offsetX + vertices[0].first * scale,
                offsetY + vertices[0].second * scale);

  for (size_t i = 1; i < vertices.size(); i++) {
    cairo_line_to(cr, offsetX + vertices[i].first * scale,
                  offsetY + vertices[i].second * scale);
  }
  cairo_close_path(cr);
  cairo_fill(cr);
}

// -----------------------------------------------------------------
// Lectura de archivo .xy
// Formato esperado:
// gID nv ... datos ...
// si nv = 1 → x y r type
// si nv = N > 1 → v1x v1y v2x v2y ... vNx vNy type
// -----------------------------------------------------------------
std::unordered_map<int, std::unique_ptr<Grain>>
readGrainsFromFile(const std::string &filename) {
  std::unordered_map<int, std::unique_ptr<Grain>> grains;
  std::ifstream file(filename);
  if (!file.is_open())
    throw std::runtime_error("No se pudo abrir " + filename);

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    int gID, nv, type;
    iss >> gID >> nv;

    if (nv == 1) {
      double x, y, r;
      iss >> x >> y >> r >> type;
      grains[gID] = std::make_unique<CircleGrain>(gID, type, x, y, r);
    } else {
      std::vector<std::pair<double, double>> vertices;
      for (int i = 0; i < nv; i++) {
        double vx, vy;
        iss >> vx >> vy;
        vertices.emplace_back(vx, vy);
      }
      iss >> type;
      grains[gID] = std::make_unique<PolygonGrain>(gID, nv, type, vertices);
    }
  }
  return grains;
}
