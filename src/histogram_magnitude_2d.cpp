#include "histogram_magnitude_2d.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

// MagnitudeHistogram::MagnitudeHistogram(int bins_x, int bins_y, 
//                                    double xmin, double xmax, 
//                                    double ymin, double ymax)
MagnitudeHistogram::MagnitudeHistogram(double xmin, double xmax, double ymin, double ymax)
    : bins_x_(xmax - xmin), bins_y_(ymax - ymin)
    , xmin_(xmin), xmax_(xmax), ymin_(ymin), ymax_(ymax)
    , cell_width_(1.0)
    , cell_height_(1.0)
    // , cell_width_((xmax - xmin) / bins_x)
    // , cell_height_((ymax - ymin) / bins_y)
    , magnitude_sums_(bins_y_, std::vector<double>(bins_x_, 0.0))
    , counts_(bins_y_, std::vector<int>(bins_x_, 0))
    , averages_(bins_y_, std::vector<double>(bins_x_, 0.0)) {
}

void MagnitudeHistogram::addPoint(double x, double y, double magnitude) {
    // Solo procesar puntos dentro del rango
    if (x < xmin_ || x > xmax_ || y < ymin_ || y > ymax_) {
        return;
    }
    
    int i = static_cast<int>((y - ymin_) / cell_height_);
    int j = static_cast<int>((x - xmin_) / cell_width_);
    
    // Asegurar que estamos dentro de los límites
    i = std::clamp(i, 0, bins_y_ - 1);
    j = std::clamp(j, 0, bins_x_ - 1);
    
    std::lock_guard<std::mutex> lock(mutex_);
    magnitude_sums_[i][j] += magnitude;
    counts_[i][j]++;
}

void MagnitudeHistogram::addPoints(const std::vector<std::tuple<double, double, double>>& points) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& point : points) {
        double x = std::get<0>(point);
        double y = std::get<1>(point);
        double magnitude = std::get<2>(point);
        
        if (x < xmin_ || x > xmax_ || y < ymin_ || y > ymax_) {
            continue;
        }
        
        int i = static_cast<int>((y - ymin_) / cell_height_);
        int j = static_cast<int>((x - xmin_) / cell_width_);
        
        i = std::clamp(i, 0, bins_y_ - 1);
        j = std::clamp(j, 0, bins_x_ - 1);
        
        magnitude_sums_[i][j] += magnitude;
        counts_[i][j]++;
    }
}

void MagnitudeHistogram::computeAverages() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (int i = 0; i < bins_y_; ++i) {
        for (int j = 0; j < bins_x_; ++j) {
            if (counts_[i][j] > 0) {
                averages_[i][j] = magnitude_sums_[i][j] / counts_[i][j];
            } else {
                averages_[i][j] = std::numeric_limits<double>::quiet_NaN();
            }
        }
    }
    averages_computed_ = true;
}

double MagnitudeHistogram::getAverage(int i, int j) const {
    if (!averages_computed_) {
        throw std::runtime_error("Averages not computed yet. Call computeAverages() first.");
    }
    if (i < 0 || i >= bins_y_ || j < 0 || j >= bins_x_) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return averages_[i][j];
}

void MagnitudeHistogram::saveForMatplotlib(const std::string& filename) const {
    if (!averages_computed_) {
        throw std::runtime_error("Averages not computed yet. Call computeAverages() first.");
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    // Guardar metadata
    file << "# Magnitude Histogram Data for Matplotlib\n";
    file << "# Format: X Y Pressure Average\n";
    file << "# Bins: " << bins_x_ << " " << bins_y_ << "\n";
    file << "# X-range: " << xmin_ << " " << xmax_ << "\n";
    file << "# Y-range: " << ymin_ << " " << ymax_ << "\n";
    
    // Guardar datos en formato grid
    file << std::fixed << std::setprecision(6);
    for (int i = 0; i < bins_y_; ++i) {
        for (int j = 0; j < bins_x_; ++j) {
            double center_x = xmin_ + (j + 0.5) * cell_width_;
            double center_y = ymin_ + (i + 0.5) * cell_height_;
            double magnitude = averages_[i][j];
            
            file << center_x << " " << center_y << " " << magnitude << "\n";
        }
        file << "\n"; // Línea vacía para separar filas (formato de contour)
    }
    
    file.close();
}

void MagnitudeHistogram::saveCSV(const std::string& filename) const {
    if (!averages_computed_) {
        throw std::runtime_error("Averages not computed yet. Call computeAverages() first.");
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    // Encabezado CSV
    file << "x_center,y_center,magnitude_average,count\n";
    file << std::fixed << std::setprecision(6);
    
    for (int i = 0; i < bins_y_; ++i) {
        for (int j = 0; j < bins_x_; ++j) {
            double center_x = xmin_ + (j + 0.5) * cell_width_;
            double center_y = ymin_ + (i + 0.5) * cell_height_;
            double magnitude = averages_[i][j];
            int count = counts_[i][j];
            
            file << center_x << "," << center_y << "," << magnitude << "," << count << "\n";
        }
    }
    
    file.close();
}
