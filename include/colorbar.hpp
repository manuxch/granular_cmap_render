#pragma once
#include "colormap_renderer.hpp"

// Renderiza una barra de color vertical a la derecha
// x, y = esquina inferior izquierda
// width, height = dimensiones de la barra
// vMin, vMax = rango de valores escalares
// nSteps = cantidad de subdivisiones para aproximar el gradiente
void renderColorbar(double x, double y, double width, double height,
                    double vMin, double vMax, int nTicks = 5);
