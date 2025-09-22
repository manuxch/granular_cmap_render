#include "renderer.hpp"
#include "colormap.hpp"
#include <cairo/cairo.h>
#include <algorithm>


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

    // Escalas: determinamos bounding box de granos
    /*double xmin = 1e9, xmax = -1e9, ymin = 1e9, ymax = -1e9;*/
    /*for (const auto& g : grains) {*/
    /*    xmin = std::min(xmin, g->xmin());*/
    /*    xmax = std::max(xmax, g->xmax());*/
    /*    ymin = std::min(ymin, g->ymin());*/
    /*    ymax = std::max(ymax, g->ymax());*/
    /*}*/

    double scaleX = (width_ - 2*margin_) / (xmax - xmin);
    double scaleY = (height_ - 2*margin_) / (ymax - ymin);
    double scale = std::min(scaleX, scaleY);

    // Función para convertir coordenadas físicas a pantalla
    auto toScreen = [&](double x, double y) {
        double sx = margin_ + (x - xmin) * scale;
        double sy = height_ - margin_ - (y - ymin) * scale;
        return std::make_pair(sx, sy);
    };

    // Dibujar granos
    for (const auto& g : grains) {
        double value = g->scalar();
        // double t = (value - vmin) / (vmax - vmin);
        // if (t < 0) t = 0;
        // if (t > 1) t = 1;
        // Color col = cmap(t);
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

