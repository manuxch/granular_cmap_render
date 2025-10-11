#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from scipy import ndimage  # Para interpolación más avanzada

def load_pressure_data(filename):
    """Carga los datos del histograma guardado por C++"""
    with open(filename, 'r') as f:
        # Leer metadata
        bins_x, bins_y = 100, 100  # defaults
        xmin, xmax, ymin, ymax = -10, 10, -10, 20  # defaults
        
        for line in f:
            if line.startswith('# Bins:'):
                parts = line.split()
                bins_x, bins_y = int(parts[2]), int(parts[3])
            elif line.startswith('# X-range:'):
                parts = line.split()
                xmin, xmax = float(parts[2]), float(parts[3])
            elif line.startswith('# Y-range:'):
                parts = line.split()
                ymin, ymax = float(parts[2]), float(parts[3])
            elif not line.startswith('#'):
                break
    
    # Reiniciar lectura para los datos
    with open(filename, 'r') as f:
        lines = [line.strip() for line in f if not line.startswith('#') and line.strip()]
    
    data = []
    for line in lines:
        if line:
            parts = line.split()
            if len(parts) == 3:
                data.append([float(parts[0]), float(parts[1]), float(parts[2])])
    
    return np.array(data), (bins_x, bins_y), (xmin, xmax, ymin, ymax)

def plot_pressure_map_imshow(data_file, output_file=None, cmap='viridis', 
                           interpolation='bilinear', dpi=150, figsize=(12, 10)):
    """Genera el mapa de presión usando imshow con interpolación"""
    
    data, (bins_x, bins_y), (xmin, xmax, ymin, ymax) = load_pressure_data(data_file)
    
    if len(data) == 0:
        print("No data found!")
        return
    
    x = data[:, 0]
    y = data[:, 1]
    pressure = data[:, 2]
    
    # Crear grilla única y matriz 2D
    x_unique = np.unique(x)
    y_unique = np.unique(y)
    
    # Reorganizar en matriz 2D
    pressure_grid = pressure.reshape(len(y_unique), len(x_unique))
    
    # Reemplazar NaN con valor mínimo para mejor visualización
    pressure_grid_clean = np.where(np.isnan(pressure_grid), np.nanmin(pressure_grid), pressure_grid)
    
    # Crear figura
    fig, ax = plt.subplots(figsize=figsize)
    
    # Usar imshow con interpolación
    extent = [xmin, xmax, ymin, ymax]
    im = ax.imshow(pressure_grid_clean, 
                   extent=extent,
                   origin='lower',  # Importante: coord (0,0) en esquina inferior
                   cmap=cmap,
                   interpolation=interpolation,
                   aspect='equal')
    
    # Barra de colores
    cbar = plt.colorbar(im, ax=ax, shrink=0.8)
    cbar.set_label('Pressure (N/m)', rotation=270, labelpad=20, fontsize=12)
    
    ax.set_xlabel('X (m)', fontsize=12)
    ax.set_ylabel('Y (m)', fontsize=12)
    ax.set_title(f'Average Pressure Distribution\nInterpolation: {interpolation}', fontsize=14)
    
    # Grid opcional
    ax.grid(True, alpha=0.3)
    
    if output_file:
        plt.savefig(output_file, dpi=dpi, bbox_inches='tight', facecolor='white')
        print(f"Pressure map saved to: {output_file}")
    
    plt.show()
    
    return pressure_grid_clean

def plot_comparison(data_file, output_dir=None, cmap='viridis', dpi=150):
    """Genera múltiples plots con diferentes métodos de interpolación"""
    
    data, (bins_x, bins_y), (xmin, xmax, ymin, ymax) = load_pressure_data(data_file)
    
    if len(data) == 0:
        print("No data found!")
        return
    
    x = data[:, 0]
    y = data[:, 1]
    pressure = data[:, 2]
    
    # Crear grilla única y matriz 2D
    x_unique = np.unique(x)
    y_unique = np.unique(y)
    pressure_grid = pressure.reshape(len(y_unique), len(x_unique))
    pressure_grid_clean = np.where(np.isnan(pressure_grid), np.nanmin(pressure_grid), pressure_grid)
    
    # Métodos de interpolación a comparar
    interpolation_methods = [
        ('none', 'Sin interpolación'),
        ('nearest', 'Vecino más cercano'),
        ('bilinear', 'Bilineal'),
        ('bicubic', 'Bicúbica'),
        ('spline16', 'Spline 16'),
        ('hanning', 'Hanning')
    ]
    
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    axes = axes.flatten()
    
    extent = [xmin, xmax, ymin, ymax]
    
    for idx, (interp, title) in enumerate(interpolation_methods):
        ax = axes[idx]
        im = ax.imshow(pressure_grid_clean, 
                      extent=extent,
                      origin='lower',
                      cmap=cmap,
                      interpolation=interp,
                      aspect='equal')
        
        ax.set_title(title, fontsize=12)
        ax.set_xlabel('X (m)')
        ax.set_ylabel('Y (m)')
        ax.grid(True, alpha=0.3)
        
        # Añadir colorbar a cada subplot
        plt.colorbar(im, ax=ax, shrink=0.8)
    
    # Ocultar el último subplot si no se usa
    if len(interpolation_methods) < len(axes):
        for idx in range(len(interpolation_methods), len(axes)):
            axes[idx].set_visible(False)
    
    plt.tight_layout()
    
    if output_dir:
        comparison_file = f"{output_dir}/pressure_interpolation_comparison.png"
        plt.savefig(comparison_file, dpi=dpi, bbox_inches='tight', facecolor='white')
        print(f"Comparison plot saved to: {comparison_file}")
    
    plt.show()

# Función con suavizado gaussiano adicional
def plot_pressure_smoothed(data_file, output_file=None, cmap='viridis', sigma=1.0):
    """Usa filtro gaussiano para suavizado adicional"""
    
    data, (bins_x, bins_y), (xmin, xmax, ymin, ymax) = load_pressure_data(data_file)
    
    if len(data) == 0:
        print("No data found!")
        return
    
    x = data[:, 0]
    y = data[:, 1]
    pressure = data[:, 2]
    
    # Crear grilla única y matriz 2D
    x_unique = np.unique(x)
    y_unique = np.unique(y)
    pressure_grid = pressure.reshape(len(y_unique), len(x_unique))
    pressure_grid_clean = np.where(np.isnan(pressure_grid), np.nanmin(pressure_grid), pressure_grid)
    
    # Aplicar filtro gaussiano
    pressure_smoothed = ndimage.gaussian_filter(pressure_grid_clean, sigma=sigma)
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))
    
    extent = [xmin, xmax, ymin, ymax]
    
    # Original
    im1 = ax1.imshow(pressure_grid_clean, extent=extent, origin='lower', 
                    cmap=cmap, interpolation='bilinear', aspect='equal')
    ax1.set_title('Original (bilinear)', fontsize=12)
    ax1.set_xlabel('X (m)')
    ax1.set_ylabel('Y (m)')
    plt.colorbar(im1, ax=ax1, shrink=0.8)
    
    # Suavizado
    im2 = ax2.imshow(pressure_smoothed, extent=extent, origin='lower', 
                    cmap=cmap, interpolation='bilinear', aspect='equal')
    ax2.set_title(f'Suavizado (Gaussiano, σ={sigma})', fontsize=12)
    ax2.set_xlabel('X (m)')
    ax2.set_ylabel('Y (m)')
    plt.colorbar(im2, ax=ax2, shrink=0.8)
    
    plt.tight_layout()
    
    if output_file:
        plt.savefig(output_file, dpi=150, bbox_inches='tight', facecolor='white')
        print(f"Smoothed plot saved to: {output_file}")
    
    plt.show()

if __name__ == "__main__":
    import sys
    import os
    
    if len(sys.argv) < 2:
        print("Usage: python plot_pressure_map.py <histogram_data_file> [interpolation_method]")
        print("Available interpolation methods: none, nearest, bilinear, bicubic, spline16, hanning")
        sys.exit(1)
    
    data_file = sys.argv[1]
    interpolation = sys.argv[2] if len(sys.argv) > 2 else 'bilinear'
    
    output_dir = os.path.dirname(data_file) or "."
    base_name = os.path.splitext(os.path.basename(data_file))[0]
    
    # Plot individual
    output_file = f"{output_dir}/{base_name}_{interpolation}.png"
    plot_pressure_map_imshow(data_file, output_file, interpolation=interpolation)
    
    # Plot de comparación
    comparison_dir = output_dir
    plot_comparison(data_file, comparison_dir)
    
    # Plot con suavizado
    smoothed_file = f"{output_dir}/{base_name}_smoothed.png"
    plot_pressure_smoothed(data_file, smoothed_file, sigma=1.0)
