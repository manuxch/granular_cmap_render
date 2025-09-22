#!/bin/bash
# doVideo.sh
# Script para concatenar imágenes secuenciales para armar una animación con efecto de persistencia
# Manuel Carlevaro - 2025.09.22

if [ "$#" -lt 3 ]; then
  echo "Uso: "
  echo "  ./dovideo prefijo nombre_video frame_rate [opacidad] [frames] [pesos]"
  echo "  "
  echo "      prefijo de archivos de imagen"
  echo "      nombre_video sin extensión (agrega .mp4)"
  echo "      frame_rate sugeridos: 20, 25, 30."
  echo "      opacidad: intensidad de persistencia (0.0-1.0, default: 0.5)"
  echo "      frames: número de frames a mezclar (default: 2)"
  echo "      pesos: pesos relativos separados por espacios (default: '1 1')"
  echo "  "
  echo "Ejemplos: "
  echo "  ./dovideo frm anim 25"
  echo "  ./dovideo frm anim 25 0.7"
  echo "  ./dovideo frm anim 25 0.3 3 '1 2 1'"
  echo "  ./dovideo frm anim 25 0.5 2 '3 1'"
  echo "  "
  exit 1
fi

if ! [[ "$3" =~ ^[0-9]+$ ]]; then
  echo "El tercer argumento (frame_rate) debe ser un entero." >&2
  exit 1
fi

# Valores por defecto
OPACITY=${4:-0.5}
FRAMES=${5:-2}
WEIGHTS=${6:-"1 1"}

# Validaciones básicas
if (($(echo "$OPACITY < 0.0 || $OPACITY > 1.0" | bc -l))); then
  echo "Error: La opacidad debe estar entre 0.0 y 1.0" >&2
  exit 1
fi

if ! [[ "$FRAMES" =~ ^[0-9]+$ ]] || [ "$FRAMES" -lt 1 ]; then
  echo "Error: El número de frames debe ser un entero positivo" >&2
  exit 1
fi

# Construir el filtro según los parámetros
if [ "$FRAMES" -eq 2 ] && [ "$WEIGHTS" = "1 1" ]; then
  # Caso simple: usar tblend para mejor rendimiento
  FILTER="-vf \"tblend=all_mode=average:all_opacity=$OPACITY\""
else
  # Caso complejo: usar tmix con los parámetros especificados
  FILTER="-vf \"tmix=frames=$FRAMES:weights=$WEIGHTS\""
fi

echo "Generando video con parámetros:"
echo "  Prefijo: $1"
echo "  Video: $2.mp4"
echo "  Frame rate: $3"
echo "  Opacidad: $OPACITY"
echo "  Frames a mezclar: $FRAMES"
echo "  Pesos: $WEIGHTS"
echo "  Filtro: $FILTER"
echo ""

# Ejecutar ffmpeg
eval ffmpeg -framerate $3 -pattern_type glob -i \"$1_*.png\" \
  -c:v libx264 -pix_fmt yuv420p \
  -crf 23 -preset medium \
  $FILTER \
  $2.mp4
