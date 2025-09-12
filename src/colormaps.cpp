#include "colormaps.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

// Normalización a [0,1]
static double normalize(double value, double vMin, double vMax) {
  if (vMax == vMin)
    return 0.0;
  double t = (value - vMin) / (vMax - vMin);
  return std::clamp(t, 0.0, 1.0);
}

// ------------------------------
// Viridis (aprox con polinomios)
// ------------------------------
Color cmapViridis(double value, double vMin, double vMax) {
  double t = normalize(value, vMin, vMax);

  // valores aproximados de la tabla viridis
  double r = 0.267 + 0.633 * t - 0.25 * t * t;
  double g = 0.005 + 1.3 * t - 1.6 * t * t + 0.9 * t * t * t;
  double b = 0.329 + 1.4 * t - 2.1 * t * t + 1.1 * t * t * t;

  return {std::clamp(r, 0.0, 1.0), std::clamp(g, 0.0, 1.0),
          std::clamp(b, 0.0, 1.0)};
}

// ------------------------------
// Plasma
// ------------------------------
Color cmapPlasma(double value, double vMin, double vMax) {
  double t = normalize(value, vMin, vMax);
  double r = 0.05 + 2.0 * t - 2.5 * t * t + 1.5 * t * t * t;
  double g = 0.0 + 0.9 * t - 0.8 * t * t;
  double b = 0.55 + 0.7 * t - 1.6 * t * t + 1.4 * t * t * t;
  return {std::clamp(r, 0.0, 1.0), std::clamp(g, 0.0, 1.0),
          std::clamp(b, 0.0, 1.0)};
}

// ------------------------------
// Inferno
// ------------------------------
Color cmapInferno(double value, double vMin, double vMax) {
  double t = normalize(value, vMin, vMax);
  double r = std::pow(t, 0.5);
  double g = std::pow(t, 1.5) * 0.8;
  double b = std::pow(1 - t, 3.0);
  return {r, g, b};
}

// ------------------------------
// Magma
// ------------------------------
Color cmapMagma(double value, double vMin, double vMax) {
  double t = normalize(value, vMin, vMax);
  double r = std::pow(t, 0.8);
  double g = std::pow(t, 1.2) * 0.7;
  double b = std::pow(1 - t, 2.0);
  return {r, g, b};
}

// ------------------------------
// Jet (azul → cian → verde → amarillo → rojo)
// ------------------------------
Color cmapJet(double value, double vMin, double vMax) {
  double t = normalize(value, vMin, vMax);
  double r = std::clamp(1.5 - std::abs(4.0 * t - 3.0), 0.0, 1.0);
  double g = std::clamp(1.5 - std::abs(4.0 * t - 2.0), 0.0, 1.0);
  double b = std::clamp(1.5 - std::abs(4.0 * t - 1.0), 0.0, 1.0);
  return {r, g, b};
}

// ------------------------------
// Grayscale
// ------------------------------
Color cmapGray(double value, double vMin, double vMax) {
  double t = normalize(value, vMin, vMax);
  return {t, t, t};
}

// ------------------------------
// getColormap
// ------------------------------
ColormapFunc getColormap(const std::string &name) {
  if (name == "viridis")
    return &cmapViridis;
  if (name == "plasma")
    return &cmapPlasma;
  if (name == "inferno")
    return &cmapInferno;
  if (name == "magma")
    return &cmapMagma;
  if (name == "jet")
    return &cmapJet;
  if (name == "gray" || name == "grayscale")
    return &cmapGray;
  throw std::runtime_error("Colormap desconocido: " + name);
}

