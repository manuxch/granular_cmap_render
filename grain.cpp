#include "grain.hpp"
#include "colormap_renderer.hpp" // para renderCircle/renderPolygon

// Render de círculo
void CircleGrain::render(const Color &c) const {
  renderCircle(x, y, r, c); // función implementada en tu renderer actual
}

// Render de polígono
void PolygonGrain::render(const Color &c) const {
  renderPolygon(vertices, c); // función implementada en tu renderer actual
}
