#ifndef PRESSURE_HISTOGRAM_HPP
#define PRESSURE_HISTOGRAM_HPP

#include <vector>
#include <mutex>
#include <atomic>
#include <fstream>
#include <string>

class MagnitudeHistogram {
public:
    MagnitudeHistogram(double xmin, double xmax, double ymin, double ymax);
    // MagnitudeHistogram(int bins_x, int bins_y, double xmin, double xmax, double ymin, double ymax);
    
    // Thread-safe: agregar un punto de datos
    void addPoint(double x, double y, double magnitude);
    
    // Thread-safe: agregar múltiples puntos (para eficiencia)
    void addPoints(const std::vector<std::tuple<double, double, double>>& points);
    
    // Calcular promedios (llamar después de que todos los hilos terminen)
    void computeAverages();
    
    // Guardar en formato legible por matplotlib
    void saveForMatplotlib(const std::string& filename) const;
    
    // Guardar en formato CSV simple
    void saveCSV(const std::string& filename) const;
    
    // Getters para información de la grilla
    int getBinsX() const { return bins_x_; }
    int getBinsY() const { return bins_y_; }
    double getXMin() const { return xmin_; }
    double getXMax() const { return xmax_; }
    double getYMin() const { return ymin_; }
    double getYMax() const { return ymax_; }
    
    // Obtener el valor promedio en una celda específica
    double getAverage(int i, int j) const;

private:
    int bins_x_, bins_y_;
    double xmin_, xmax_, ymin_, ymax_;
    double cell_width_, cell_height_;
    
    std::vector<std::vector<double>> magnitude_sums_;
    std::vector<std::vector<int>> counts_;
    std::vector<std::vector<double>> averages_;
    
    mutable std::mutex mutex_;
    std::atomic<bool> averages_computed_{false};
};

#endif
