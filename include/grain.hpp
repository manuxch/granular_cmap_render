#pragma once
#include <vector>
#include <memory>
#include <numbers>
#include <functional>
#include <cairo/cairo.h>

// Conversión de coordenadas (x,y) físicas a (sx,sy) de pantalla
using TransformFunc = std::function<std::pair<double,double>(double,double)>;

class Grain {
public:
    Grain(int gid, int type, int nv, double scalar)
        : gid_(gid), type_(type), nv_(nv), scalar_(scalar) {}
    virtual ~Grain() = default;

    virtual void render(cairo_t* cr,
                        const TransformFunc& toScreen,
                        double scale) const = 0;

    virtual double xmin() const = 0;
    virtual double xmax() const = 0;
    virtual double ymin() const = 0;
    virtual double ymax() const = 0;

    double scalar() const { return scalar_; }
    int gid() const { return gid_; }
    int type() const { return type_; }
    int nv() const { return nv_; }

protected:
    int gid_;
    int type_;
    int nv_;
    double scalar_;
};

// --------------------- CircleGrain ---------------------
class CircleGrain : public Grain {
public:
    CircleGrain(int gid, int type, double x, double y, double r, double scalar);

    void render(cairo_t* cr,
                const TransformFunc& toScreen,
                double scale) const override;

    double xmin() const override { return x_ - r_; }
    double xmax() const override { return x_ + r_; }
    double ymin() const override { return y_ - r_; }
    double ymax() const override { return y_ + r_; }

private:
    double x_, y_, r_;
};

// --------------------- PolygonGrain ---------------------
class PolygonGrain : public Grain {
public:
    PolygonGrain(int gid, int type,
                 const std::vector<std::pair<double,double>>& vertices,
                 double scalar);

    void render(cairo_t* cr,
                const TransformFunc& toScreen,
                double scale) const override;

    double xmin() const override;
    double xmax() const override;
    double ymin() const override;
    double ymax() const override;

private:
    std::vector<std::pair<double,double>> vertices_;
};

class BorderGrain : public Grain {
  public:
    BorderGrain(int gid, int type, 
                const std::vector<std::pair<double, double>>& vertices,
                double scalar);   // scalar = -1 for walls -> color BLACK
    
    void render (cairo_t* cr, 
                  const TransformFunc& toScreen,
                  double scale) const override;

    double xmin() const override;
    double xmax() const override;
    double ymin() const override;
    double ymax() const override;

  private:
    std::vector<std::pair<double, double>> vertices_;
};
