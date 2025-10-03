#pragma once
#include <string>
#include <vector>
#include <memory>
#include "grain.hpp"
#include "colormap.hpp"

class Renderer {
public:
    Renderer(int width, int height, double margin, double valmin, double valmax);

    void renderToPNG(const std::string& filename,
                     const std::vector<std::unique_ptr<Grain>>& grains,
                     double vmin, double vmax,
                     double xmin, double xmax, double ymin, double ymax,
                     const Colormap& cmap);

private:
    int width_;
    int height_;
    double margin_;
    double valmin_;
    double valmax_;
};

