#include "colorbar.hpp"
#include <cmath>
#include <sstream>

// Renderiza barra de color vertical con gradiente lineal
void renderColorbar(cairo_t* cr,
                    double x, double y, double width, double height,
                    double vMin, double vMax,
                    ColormapFunc cmap, int nTicks) {
    cairo_pattern_t* pat = cairo_pattern_create_linear(0, y, 0, y + height);

    int steps = 100; // resolución del gradiente
    for (int i = 0; i <= steps; i++) {
        double t = double(i) / steps;
        double value = vMin + t * (vMax - vMin);
        Color c = cmap(value, vMin, vMax);
        cairo_pattern_add_color_stop_rgb(pat, t, c.r, c.g, c.b);
    }

    cairo_rectangle(cr, x, y, width, height);
    cairo_set_source(cr, pat);
    cairo_fill(cr);
    cairo_pattern_destroy(pat);

    // ticks y etiquetas
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 10);

    for (int i = 0; i < nTicks; i++) {
        double t = double(i) / (nTicks - 1);
        double yy = y + (1 - t) * height;
        cairo_move_to(cr, x + width, yy);
        cairo_line_to(cr, x + width + 5, yy);
        cairo_stroke(cr);

        double value = vMin + t * (vMax - vMin);
        std::ostringstream oss;
        oss.precision(3);
        oss << std::fixed << value;
        cairo_move_to(cr, x + width + 8, yy + 3);
        cairo_show_text(cr, oss.str().c_str());
    }
}

