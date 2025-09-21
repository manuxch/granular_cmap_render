#pragma once
#include "grain.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Lee frm_XXX.xy y frm_XXX.sxy, devuelve lista de granos con valor escalar
// calculado
namespace Parser {

// Lee archivo .sxy -> devuelve un mapa gid -> vector<double>
std::unordered_map<int, std::vector<double>>
readSXY(const std::string &filename);

// Construye los granos leyendo frm_XXX.xy y asociando el valor de la propiedad
std::vector<std::unique_ptr<Grain>>
readXY(const std::string &filename,
       const std::unordered_map<int, std::vector<double>> &scalarData,
       const std::string &property);

// Calcula propiedad a partir de valores crudos de .sxy
double computeProperty(const std::string &property,
                       const std::vector<double> &values);

} // namespace Parser

