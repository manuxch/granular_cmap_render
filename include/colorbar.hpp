#pragma once
#include "grain.hpp" // para Color
#include <cairo.h>
#include <string>

// Función colormap: valor normalizado [0,1] → Color
using ColormapFunc = Color (*)(double value, double vMin, double vMax);

// Renderiza la barra de colores
void renderColorbar(cairo_t *cr, double x, double y, double width,
                    double height, double vMin, double vMax, ColormapFunc cmap,
                    int nTicks = 5);
