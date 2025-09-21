#ifndef COLORMAP_HPP
#define COLORMAP_HPP

#include <algorithm>
#include <array>
#include <vector>

class Colormap {
public:
  // Constructor: recibe stops {pos,[r,g,b]}
  Colormap(const std::vector<std::pair<double, std::array<double, 3>>> &stops)
      : stops(stops) {
    std::sort(this->stops.begin(), this->stops.end(),
              [](auto &a, auto &b) { return a.first < b.first; });
  }

  std::array<double, 3> operator()(double value, double vmin,
                                   double vmax) const {
    double t = (value - vmin) / (vmax - vmin + 1e-12);
    t = std::clamp(t, 0.0, 1.0);

    // buscar intervalo
    for (size_t i = 1; i < stops.size(); i++) {
      if (t <= stops[i].first) {
        double t0 = stops[i - 1].first, t1 = stops[i].first;
        double u = (t - t0) / (t1 - t0 + 1e-12);
        auto c0 = stops[i - 1].second;
        auto c1 = stops[i].second;
        return {(1 - u) * c0[0] + u * c1[0], (1 - u) * c0[1] + u * c1[1],
                (1 - u) * c0[2] + u * c1[2]};
      }
    }
    return stops.back().second;
  }

private:
  std::vector<std::pair<double, std::array<double, 3>>> stops;
};

// Colormaps de ejemplo:
inline Colormap viridis() {
  return Colormap({{0.0, {0.267, 0.004, 0.329}},
                   {0.25, {0.283, 0.141, 0.458}},
                   {0.5, {0.254, 0.265, 0.530}},
                   {0.75, {0.207, 0.372, 0.553}},
                   {1.0, {0.993, 0.906, 0.144}}});
}

inline Colormap inferno() {
  return Colormap({{0.0, {0.002, 0.000, 0.014}},
                   {0.25, {0.245, 0.033, 0.206}},
                   {0.5, {0.610, 0.076, 0.384}},
                   {0.75, {0.934, 0.281, 0.210}},
                   {1.0, {0.988, 0.998, 0.645}}});
}

inline Colormap RdYlBu() {
  return Colormap({
      {0.0, {0.8, 0.0, 0.0}}, // rojo
      {0.5, {1.0, 1.0, 0.8}}, // amarillo/blanco
      {1.0, {0.0, 0.2, 0.8}}  // azul
  });
}

#endif
