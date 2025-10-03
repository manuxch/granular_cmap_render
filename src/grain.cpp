#include "grain.hpp"

// ---------------- CircleGrain ----------------
CircleGrain::CircleGrain(int gid, int type, double x, double y, double r, double scalar)
    : Grain(gid, type, 1, scalar), x_(x), y_(y), r_(r) {}

void CircleGrain::render(cairo_t* cr,
                         const TransformFunc& toScreen,
                         double scale) const {
    auto [sx, sy] = toScreen(x_, y_);
    double sr = r_ * scale;

    cairo_arc(cr, sx, sy, sr, 0, 2 * std::numbers::pi);
    cairo_fill(cr);
}

// ---------------- PolygonGrain ----------------
PolygonGrain::PolygonGrain(int gid, int type,
                           const std::vector<std::pair<double,double>>& vertices,
                           double scalar)
    : Grain(gid, type, static_cast<int>(vertices.size()), scalar),
      vertices_(vertices) {}

void PolygonGrain::render(cairo_t* cr,
                          const TransformFunc& toScreen,
                          double scale) const {
    if (vertices_.empty()) return;

    auto [sx0, sy0] = toScreen(vertices_[0].first, vertices_[0].second);
    cairo_move_to(cr, sx0, sy0);

    for (size_t i = 1; i < vertices_.size(); i++) {
        auto [sx, sy] = toScreen(vertices_[i].first, vertices_[i].second);
        cairo_line_to(cr, sx, sy);
    }
    cairo_close_path(cr);
    cairo_fill(cr);
}

double PolygonGrain::xmin() const {
    double minv = 1e9;
    for (auto& v : vertices_) minv = std::min(minv, v.first);
    return minv;
}
double PolygonGrain::xmax() const {
    double maxv = -1e9;
    for (auto& v : vertices_) maxv = std::max(maxv, v.first);
    return maxv;
}
double PolygonGrain::ymin() const {
    double minv = 1e9;
    for (auto& v : vertices_) minv = std::min(minv, v.second);
    return minv;
}
double PolygonGrain::ymax() const {
    double maxv = -1e9;
    for (auto& v : vertices_) maxv = std::max(maxv, v.second);
    return maxv;
}


// ---------------- BorderGrain ----------------
BorderGrain::BorderGrain(int gid, int type,
                         const std::vector<std::pair<double, double>>& vertices,
                         double scalar)
    : Grain(gid, type, static_cast<int>(vertices.size()), scalar),
      vertices_(vertices) {}

void BorderGrain::render(cairo_t* cr,
                         const TransformFunc& toScreen,
                         double scale) const {
    if (vertices_.empty()) return;

    auto [sx0, sy0] = toScreen(vertices_[0].first, vertices_[0].second);
    cairo_move_to(cr, sx0, sy0);

    for (size_t i = 1; i < vertices_.size(); i++) {
        auto [sx, sy] = toScreen(vertices_[i].first, vertices_[i].second);
        cairo_line_to(cr, sx, sy);
    }
    // cairo_set_source_rgb(cr, 1, 1, 1);
    // cairo_fill_preserve(cr);
    // cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    cairo_stroke(cr);
}


double BorderGrain::xmin() const {
    double minv = 1e9;
    for (auto& v : vertices_) minv = std::min(minv, v.first);
    return minv;
}
double BorderGrain::xmax() const {
    double maxv = -1e9;
    for (auto& v : vertices_) maxv = std::max(maxv, v.first);
    return maxv;
}
double BorderGrain::ymin() const {
    double minv = 1e9;
    for (auto& v : vertices_) minv = std::min(minv, v.second);
    return minv;
}
double BorderGrain::ymax() const {
    double maxv = -1e9;
    for (auto& v : vertices_) maxv = std::max(maxv, v.second);
    return maxv;
}

