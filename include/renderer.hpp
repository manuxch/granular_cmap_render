#pragma once
#include "colormap.hpp"
#include "grain.hpp"
#include <memory>
#include <string>
#include <vector>

class Renderer {
public:
  Renderer(int width, int height, double margin, double valmin, double valmax);

  void renderToPNG(const std::string &filename,
                   const std::vector<std::unique_ptr<Grain>> &grains,
                   double vmin, double vmax, double xmin, double xmax,
                   double ymin, double ymax, const Colormap &cmap,
                   const std::string &cbar_title = "",
                   const std::string &cbar_unit = "");

  void drawColorbar(cairo_t *cr, double x, double y, double width,
                    double height, double vmin, double vmax,
                    const Colormap &cmap, const std::string &title,
                    const std::string &unit);

private:
  int width_;
  int height_;
  double margin_;
  double valmin_;
  double valmax_;
};
