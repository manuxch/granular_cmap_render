#include <cairo.h>
#include <cairo-pdf.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <memory>

#include "grain.hpp"
#include "colormaps.hpp"
#include "colorbar.hpp"

// -------------------------------------------------------------
// Lee archivo .sxy y devuelve valores escalares por gID
// En este ejemplo asumimos que el archivo tiene:
// gID σxx σyy σxy ...
// y usamos la presión p = -(σxx + σyy)/2
// -------------------------------------------------------------
std::unordered_map<int, double> readStressValues(const std::string& filename) {
    std::unordered_map<int, double> values;
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("No se pudo abrir " + filename);

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int gID;
        double sxx, syy, sxy;
        if (!(iss >> gID >> sxx >> syy >> sxy)) continue;

        double pressure = -(sxx + syy) / 2.0;
        values[gID] = pressure;
    }
    return values;
}

// -------------------------------------------------------------
// Render principal: lee granos + escalares y genera PDF
// -------------------------------------------------------------
void renderFileWithScalars(const std::string& xyFile,
                           const std::string& sxyFile,
                           const std::string& outFile,
                           const std::string& cmapName,
                           double Lx, double Ly) {
    // 1. Leer geometría
    auto grains = readGrainsFromFile(xyFile);

    // 2. Leer valores escalares
    auto values = readStressValues(sxyFile);

    // 3. Determinar rango [vMin, vMax]
    double vMin = 1e30, vMax = -1e30;
    for (auto& kv : values) {
        vMin = std::min(vMin, kv.second);
        vMax = std::max(vMax, kv.second);
    }

    // 4. Crear superficie Cairo
    double margin = 40;
    int widthPx  = 800;
    int heightPx = 800;

    cairo_surface_t* surface = cairo_pdf_surface_create(outFile.c_str(), widthPx, heightPx);
    cairo_t* cr = cairo_create(surface);

    // Fondo blanco
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    // Escalas
    double scaleX = (widthPx - 2*margin) / Lx;
    double scaleY = (heightPx - 2*margin) / Ly;
    double scale  = std::min(scaleX, scaleY);
    double offsetX = margin;
    double offsetY = margin;

    // 5. Colormap elegido
    ColormapFunc cmap = getColormap(cmapName);

    // 6. Dibujar granos
    for (const auto& [gID, grainPtr] : grains) {
        auto it = values.find(gID);
        if (it == values.end()) continue;

        double scalar = it->second;
        Color c = cmap(scalar, vMin, vMax);

        grainPtr->render(cr, c, scale, offsetX, offsetY);
    }

    // 7. Dibujar colorbar
    double barWidth  = 20;
    double barHeight = 200;
    renderColorbar(cr, widthPx - margin - barWidth, margin,
                   barWidth, barHeight, vMin, vMax, cmap, 6);

    // 8. Finalizar
    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

