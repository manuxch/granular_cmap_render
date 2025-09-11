#include "colorbar.hpp"
#include <cairo.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

// Cairo context global o pasado desde fuera
extern cairo_t* cr;

static void setCairoColor(const Color& c) {
    cairo_set_source_rgb(cr, c.r, c.g, c.b);
}

static void renderText(double x, double y, const std::string& text, double size, const Color& c) {
    setCairoColor(c);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, size);

    cairo_move_to(cr, x, y);
    cairo_show_text(cr, text.c_str());
}

void renderColorbar(double x, double y, double width, double height,
                    double vMin, double vMax, int nTicks) {
    // Crear gradiente vertical
    cairo_pattern_t* gradient = cairo_pattern_create_linear(x, y, x, y + height);

    // Agregar stops según el colormap OrRd
    int nSteps = 200; // más pasos = más suave
    for (int i = 0; i <= nSteps; i++) {
        double t = static_cast<double>(i) / nSteps;
        double v = vMin + t * (vMax - vMin);
        Color c = colormapOrRd(v, vMin, vMax);
        cairo_pattern_add_color_stop_rgb(gradient, t, c.r, c.g, c.b);
    }

    // Dibujar rectángulo con el gradiente
    cairo_rectangle(cr, x, y, width, height);
    cairo_set_source(cr, gradient);
    cairo_fill(cr);

    cairo_pattern_destroy(gradient);

    // Dibujar marco de la barra
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, x, y, width, height);
    cairo_stroke(cr);

    // Dibujar ticks y etiquetas
    Color textColor = {0.0f, 0.0f, 0.0f};
    double tickLength = 6.0;
    double fontSize   = width * 0.8;

    for (int i = 0; i < nTicks; i++) {
        double t = static_cast<double>(i) / (nTicks - 1);
        double v = vMin + t * (vMax - vMin);
        double yTick = y + height * t;

        // Línea de tick
        cairo_move_to(cr, x + width, yTick);
        cairo_line_to(cr, x + width + tickLength, yTick);
        cairo_stroke(cr);

        // Texto
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << v;
        renderText(x + width + tickLength + 4, yTick + fontSize / 2, ss.str(), fontSize, textColor);
    }
}

