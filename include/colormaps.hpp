#pragma once
#include "grain.hpp" // para struct Color
#include <string>

// Función tipo colormap
using ColormapFunc = Color (*)(double value, double vMin, double vMax);

// Declaración de colormaps
Color cmapViridis(double value, double vMin, double vMax);
Color cmapPlasma(double value, double vMin, double vMax);
Color cmapInferno(double value, double vMin, double vMax);
Color cmapMagma(double value, double vMin, double vMax);
Color cmapJet(double value, double vMin, double vMax);
Color cmapGray(double value, double vMin, double vMax);

// Devuelve el colormap por nombre
ColormapFunc getColormap(const std::string &name);

