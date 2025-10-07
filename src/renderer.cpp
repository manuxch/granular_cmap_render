#include <cairo/cairo.h>
#include <string>

#include "colormap.hpp"
#include "renderer.hpp"

Renderer::Renderer(int width, int height, double margin, double valmin,
                   double valmax)
    : width_(width), height_(height), margin_(margin), valmin_(valmin),
      valmax_(valmax) {}

void Renderer::renderToPNG(const std::string &filename,
                           const std::vector<std::unique_ptr<Grain>> &grains,
                           double vmin, double vmax, double xmin, double xmax,
                           double ymin, double ymax, const Colormap &cmap,
                           const std::string &cbar_title,
                           const std::string &cbar_unit) {
  // Crear superficie Cairo
  cairo_surface_t *surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_);
  cairo_t *cr = cairo_create(surface);

  // Fondo blanco
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);

  // Corregir rangos si están invertidos
  if (xmax < xmin)
    std::swap(xmax, xmin);
  if (ymax < ymin)
    std::swap(ymax, ymin);

  // Calcular área disponible para el dibujo
  double available_width = width_ - 2 * margin_;
  double available_height = height_ - 2 * margin_;

  // Calcular escalas manteniendo relación de aspecto
  double scaleX = available_width / (xmax - xmin);
  double scaleY = available_height / (ymax - ymin);
  double scale = std::min(scaleX, scaleY);

  // Calcular offsets para centrar
  double offsetX = 0, offsetY = 0;
  if (scaleX > scaleY) {
    // Espacio sobrante a los lados (formato horizontal)
    offsetX = (width_ - 2 * margin_ - (xmax - xmin) * scale) / 2;
  } else {
    // Espacio sobrante arriba/abajo (formato vertical)
    offsetY = (height_ - 2 * margin_ - (ymax - ymin) * scale) / 2;
  }

  // Función para convertir coordenadas físicas a pantalla (CENTRADO)
  auto toScreen = [&](double x, double y) {
    double sx = margin_ + offsetX + (x - xmin) * scale;
    double sy = height_ - margin_ - offsetY - (y - ymin) * scale;
    return std::make_pair(sx, sy);
  };

  // Dibujar granos
  for (const auto &g : grains) {
    double value = g->scalar();
    std::array<double, 3> col = cmap(value, valmin_, valmax_);
    // std::array<double, 3> col = cmap(value, vmin, vmax);

    cairo_set_source_rgb(cr, col[0], col[1], col[2]);
    // cairo_set_source_rgb(cr, col.r, col.g, col.b);
    g->render(cr, toScreen, scale);
  }

  // Dibujar barra de escala de colores
  double colorbar_width = 30;                           // Ancho de la barra
  double colorbar_height = height_ - 2 * margin_ - 100; // Alto de la barra
  double colorbar_x = width_ - margin_ - colorbar_width - 20; // Posición X
  double colorbar_y = margin_ + 50;                           // Posición Y

  drawColorbar(cr, colorbar_x, colorbar_y, colorbar_width, colorbar_height,
               valmin_, valmax_, cmap, "Pressure", "MPa");

  // Guardar en archivo
  cairo_surface_write_to_png(surface, filename.c_str());

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

void Renderer::drawColorbar(cairo_t *cr, double x, double y, double width,
                            double height, double vmin, double vmax,
                            const Colormap &cmap, const std::string &title,
                            const std::string &unit) {
  // Guardar el estado actual de Cairo
  cairo_save(cr);

  // Dibujar la barra de color gradiente
  cairo_pattern_t *pattern = cairo_pattern_create_linear(x, y + height, x, y);

  // Agregar stops al gradiente
  int num_stops = 256;
  for (int i = 0; i <= num_stops; ++i) {
    double t = static_cast<double>(i) / num_stops;
    double value = vmin + t * (vmax - vmin);
    std::array<double, 3> color = cmap(value, vmin, vmax);
    cairo_pattern_add_color_stop_rgb(pattern, t, color[0], color[1], color[2]);
  }

  // Dibujar el rectángulo con el gradiente
  cairo_set_source(cr, pattern);
  cairo_rectangle(cr, x, y, width, height);
  cairo_fill(cr);
  cairo_pattern_destroy(pattern);

  // Dibujar borde alrededor de la barra
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_set_line_width(cr, 1.0);
  cairo_rectangle(cr, x, y, width, height);
  cairo_stroke(cr);

  // Configurar texto
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 12);
  cairo_set_source_rgb(cr, 0, 0, 0);

  // Dibujar marcas y valores
  int num_ticks = 5;
  for (int i = 0; i <= num_ticks; ++i) {
    double t = static_cast<double>(i) / num_ticks;
    double value = vmin + t * (vmax - vmin);
    double tick_y = y + height - t * height;

    // Dibujar marca
    cairo_move_to(cr, x + width, tick_y);
    cairo_line_to(cr, x + width + 5, tick_y);
    cairo_stroke(cr);

    // Formatear el valor
    std::string value_str;
    if (std::abs(value) < 1e-4 && value != 0) {
      char buffer[32];
      std::snprintf(buffer, sizeof(buffer), "%.2e", value);
      value_str = buffer;
    } else {
      char buffer[32];
      std::snprintf(buffer, sizeof(buffer), "%.2f", value);
      value_str = buffer;
    }

    // Dibujar texto del valor
    cairo_text_extents_t extents;
    cairo_text_extents(cr, value_str.c_str(), &extents);
    cairo_move_to(cr, x + width + 8, tick_y + extents.height / 2);
    cairo_show_text(cr, value_str.c_str());
  }

  // Dibujar título si se proporciona
  if (!title.empty()) {
    cairo_set_font_size(cr, 14);
    cairo_text_extents_t extents;

    std::string full_title = title;
    if (!unit.empty()) {
      full_title += " (" + unit + ")";
    }

    cairo_text_extents(cr, full_title.c_str(), &extents);

    // Rotar texto verticalmente
    cairo_save(cr);
    cairo_move_to(cr, x + width / 2 - extents.height / 2, y + height + 40);
    cairo_rotate(cr, -std::numbers::pi / 2);
    cairo_show_text(cr, full_title.c_str());
    cairo_restore(cr);
  }

  // Restaurar el estado de Cairo
  cairo_restore(cr);
}
