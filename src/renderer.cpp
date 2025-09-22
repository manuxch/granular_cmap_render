#include <cairo/cairo.h>
#include <algorithm>

#include "renderer.hpp"
#include "colormap.hpp"

Renderer::Renderer(int width, int height, double margin)
    : width_(width), height_(height), margin_(margin) {}

void Renderer::renderToPNG(const std::string& filename,
                           const std::vector<std::unique_ptr<Grain>>& grains,
                           double vmin, double vmax,
                           double xmin, double xmax, double ymin, double ymax,
                           const Colormap& cmap) {
    // Crear superficie Cairo
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_);
    cairo_t* cr = cairo_create(surface);

    // Fondo blanco
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    // Calcular márgenes absolutos basados en porcentaje
    double margin_pixels = margin_ * std::min(width_, height_); // margin_ es porcentaje (ej: 0.1 para 10%)

    // Calcular área disponible para el dibujo
    double available_width = width_ - 2 * margin_pixels;
    double available_height = height_ - 2 * margin_pixels;

    // Calcular escalas manteniendo relación de aspecto
    double scaleX = available_width / (xmax - xmin);
    double scaleY = available_height / (ymax - ymin);
    double scale = std::min(scaleX, scaleY);

    // Calcular offsets para centrar
    double offsetX = 0, offsetY = 0;
    if (scaleX > scaleY) {
        // Espacio sobrante a los lados (formato horizontal)
        offsetX = (width_ - 2 * margin_pixels - (xmax - xmin) * scale) / 2;
    } else {
        // Espacio sobrante arriba/abajo (formato vertical)
        offsetY = (height_ - 2 * margin_pixels - (ymax - ymin) * scale) / 2;
    }

    // Función para convertir coordenadas físicas a pantalla (CENTRADO)
    auto toScreen = [&](double x, double y) {
        double sx = margin_pixels + offsetX + (x - xmin) * scale;
        double sy = height_ - margin_pixels - offsetY - (y - ymin) * scale;
        return std::make_pair(sx, sy);
    };


    // Dibujar granos
    for (const auto& g : grains) {
        double value = g->scalar();
        std::array<double, 3> col = cmap(value, vmin, vmax);

        cairo_set_source_rgb(cr, col[0], col[1], col[2]);
        // cairo_set_source_rgb(cr, col.r, col.g, col.b);
        g->render(cr, toScreen, scale);
    }

    // // Dibujar barra de color
    // Colorbar::draw(cr, width_ - 60, 50, 20, height_ - 100, vmin, vmax, cmap);

    // Colorbar a la derecha
    // drawColorbar(cr, width, height, margin, vmin, vmax, cmap);

    // Guardar en archivo
    cairo_surface_write_to_png(surface, filename.c_str());

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

