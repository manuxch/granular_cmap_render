#pragma once
#include <vector>
#include <memory>
#include <cairo.h>

// Estructura de color
struct Color {
    double r, g, b;
};

// Clase abstracta base
class Grain {
protected:
    int gID;
    int nv;
    int type;
public:
    Grain(int gID, int nv, int type) : gID(gID), nv(nv), type(type) {}
    virtual ~Grain() = default;

    int getID() const { return gID; }
    int getType() const { return type; }

    // Render polimórfico
    virtual void render(cairo_t* cr, const Color& c,
                        double scale, double offsetX, double offsetY) const = 0;
};

// -----------------------------------------------------------------
// Grano circular
// -----------------------------------------------------------------
class CircleGrain : public Grain {
    double x, y, r;
public:
    CircleGrain(int gID, int type, double x, double y, double r)
        : Grain(gID, 1, type), x(x), y(y), r(r) {}

    void render(cairo_t* cr, const Color& c,
                double scale, double offsetX, double offsetY) const override;
};

// -----------------------------------------------------------------
// Grano poligonal
// -----------------------------------------------------------------
class PolygonGrain : public Grain {
    std::vector<std::pair<double,double>> vertices;
public:
    PolygonGrain(int gID, int nv, int type,
                 const std::vector<std::pair<double,double>>& vertices)
        : Grain(gID, nv, type), vertices(vertices) {}

    void render(cairo_t* cr, const Color& c,
                double scale, double offsetX, double offsetY) const override;
};

// -----------------------------------------------------------------
// Función de lectura de archivo .xy
// -----------------------------------------------------------------
std::unordered_map<int, std::unique_ptr<Grain>> readGrainsFromFile(const std::string& filename);

