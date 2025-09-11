#pragma once

#include <vector>

// Dimensiones por defecto del canvas
constexpr int DEFAULT_WIDTH = 1000;
constexpr int DEFAULT_HEIGHT = 1000;

// Porcentaje de margen de la imagen (5%)
constexpr double DEFAULT_MARGIN = 0.05;

// Grosor de líneas
constexpr double BORDER_WIDTH_GRAIN = 0.01;
constexpr double BORDER_WIDTH_CONTAINER = 0.02;

// Colores disponibles (paleta)
static const std::vector<std::tuple<double, double, double>> COLORS = {
    {0.83922, 0.15294, 0.15686}, // tab:red
    {0.12157, 0.46667, 0.70588}, // tab:blue
    {1.00000, 0.49804, 0.05490}, // tab:orange
    {0.17255, 0.62745, 0.17255}, // tab:green
    {0.58039, 0.40392, 0.74118}, // tab:purple
    {0.54902, 0.33725, 0.29412}, // tab:brown
    {0.89020, 0.46667, 0.76078}, // tab:pink
    {0.49804, 0.49804, 0.49804}, // tab:gray
    {0.73725, 0.74118, 0.13333}, // tab:olive
    {0.09020, 0.74510, 0.81176}, // tab:cyan
};
