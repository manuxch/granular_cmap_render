#pragma once
#include "colormap_renderer.hpp" // para usar Color
#include <utility>
#include <vector>

// Clase base abstracta
class Grain {
protected:
  int gID;
  int nv; // número de vértices (1 = círculo)
  int type;

public:
  Grain(int id, int nVertices, int t) : gID(id), nv(nVertices), type(t) {}
  virtual ~Grain() = default;

  int getID() const { return gID; }
  int getNv() const { return nv; }
  int getType() const { return type; }

  // Render genérico (cada derivada lo implementa)
  virtual void render(const Color &c) const = 0;
};

// -----------------------------
// Círculo (nv = 1)
// -----------------------------
class CircleGrain : public Grain {
  double x, y, r;

public:
  CircleGrain(int id, double cx, double cy, double radius, int t)
      : Grain(id, 1, t), x(cx), y(cy), r(radius) {}

  void render(const Color &c) const override;
};

// -----------------------------
// Polígono (nv > 1)
// -----------------------------
class PolygonGrain : public Grain {
  std::vector<std::pair<double, double>> vertices;

public:
  PolygonGrain(int id, const std::vector<std::pair<double, double>> &verts,
               int t)
      : Grain(id, verts.size(), t), vertices(verts) {}

  void render(const Color &c) const override;
};
