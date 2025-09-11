#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Representa un color RGB
struct Color {
  float r, g, b;
};

// Funciones de lectura
std::unordered_map<int, std::unique_ptr<Grain>>
readGrainsFromFile(const std::string &filename);

std::unordered_map<int, double> readScalarsFromFile(
    const std::string &filename,
    std::function<double(const std::vector<double> &)> propertyFunc);

// Colormap estilo OrRd
Color colormapOrRd(double value, double vMin, double vMax);

// Render principal
void renderFileWithColormap(
    const std::string &xyFile, const std::string &scalarFile,
    std::function<double(const std::vector<double> &)> propertyFunc,
    double vMin, double vMax);

// Estas funciones deben estar en tu renderer (para dibujar primitivos gráficos)
void renderCircle(double cx, double cy, double r, const Color &c);
void renderPolygon(const std::vector<std::pair<double, double>> &verts,
                   const Color &c);
