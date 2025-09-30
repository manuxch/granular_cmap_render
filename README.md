# granular_cmap_render

Programa que asocia a cada grano en un archivo de coordenadas generadas por simulaciones granulares con [Box2D](https://box2d.org/), una magnitud escalar que extrae de un archivo asociado al correspondiente de las coordenadas. Realiza entonces un mapa de calor con la representación de esa magnitud.

Este repositorio contiene también el script `doVideo.sh` que concatena las imágenes generadas para producir una animación temporal que muestra la evolución de la magnitud escalar asociada.

## Construir el ejecutable

    ./build.sh 

Este script genera el directorio `build/`, y construye allí el ejecutable.

## Uso 

```
./granular_cmap_render [--dir <input_dir>] [--property <name>]
       [--cmap <viridis|inferno|RdYlBu|Greens|Reds|winter|autumn|blues|hot>]
       [--config <file>]
       [--out <out_dir>] [--width <px>] [--height <px>] [--margin <px>]
       [--xylimits xmin xmax ymin ymax]

```

donde:

- `<input_dir>` es el directorio donde se encuentran los archivos de coordenadas (`*.xy`) y de propiedades asociadas (`*.sxy`). Por defecto es el directorio `.`.
- `<name>` es la propiedad escalar obtenida de los archivos `*.sxy` (por defecto: `pressure`). Actualmente incluye:
  - `pressure`
  - `kinetic_energy`
  - `velocity_norm`
- El argumento de `--cmap` es alguno de los nombres del mapa de colores. Por defecto es `viridis`.
- `<file>` es un archivo de confiuguración que contiene estos parámetros, para no pasarlos por linea de comandos.
- `<out>` es el directorio en el que se generarán las imágenes con los mapas de calor. Por defecto es `./renders`.
- `<px>` es el tamaño en pixels de la imagen a generar por cada par de archivos `*.[xy, sxy]`. Por defecto:
  - `--width = 1000`
  - `--height = 1000`
  - `--margin = 40`
- `xmin`,  `xmax`, `ymin` y `ymax` son los limites, en unidades de simulación, que se representarán en la imagen de la magnitud escalar asociada. Los valores por defecto son:
  - `xmin = -10.0`
  - `xmax = 10.0`
  - `ymin = -10.0`
  - `ymax = 20.0`

Ejemplo:

    ./granular_cmap_render . --property pressure --cmap Greens --xylimits -12.5 12.5 -5.0 30.0 

## Formato de archivo xy 

El programa lee archivos de texto con extensión `.xy` que tiene el siguiente formato:

```
# linea de comentario (puede contener el timestep o tiempo de simulación)
id nv v1x v1y v2x v2y ... vnvx vnvy type
...
id 1 x y r type
...
id nv v1x v1y v2x v2y ... vnvx vnvy BOX
...
```
Cada grano tiene un `id` entero positivo, `nv` es el número de vértices que tiene el polígono 
que representa al grano. A continuación está la lista de coordenadas de cada vértice,
y finalmente `type` es un entero que denota el tipo de grano.

Cuando `nv = 1`, la línea representa un grano circular o disco, `x` y `y` son las coordenadas
del centro del disco, y `r` su radio.

Las paredes de los contenedores son también poligonales, pero con `id < 0`. En vez 
de un `id` entero, las paredes finalizan la línea con la palabra `BOX`.

## Formato de archivo sxy 

Este archivo tiene la información necesaria para generar la magnitud escalar correspondiente a cada grano de la simulación. Para el caso de la presión sobre cada partícula, el formato es:

```
# gID stress.xx stress.xy stress.yx stress.yy
0 0-sxx 0-sxy 0-syx 0-syy
1 1-sxx 1-sxy 1-syx 1-syy
2 2-sxx 2-sxy 2-syx 2-syy
3 3-sxx 3-sxy 3-syx 3-syy
...
n n-sxx n-sxy n-syx n-syy
```

donde `gID` es la identificación de cada grano (entero positivo), y los cuatro números siguientes son el tensor de estrés del grano.

Para el caso de `--property pressure`, para cada grano se calcula como:

$$ p_i = -\frac{s_{xx} + s_{yy}}{2} $$

## TODO 

- Modificar los programas de salida para uniformizar el nombre del frame y cambiar la extensión (por ejemplo cambiar los archivos `ve_frm_nnnnn.dat` por `frm_nnnnn.ve`).
- Modificar la entrada del programa para que el archivo asociado no sea exclusivamente `*.sxy` sino que se pueda establecer la extensión como parámetro de entrada (`*.ve`).
- Poner como opción la misma escala de colores para todos los frames a partir de los valores máximo y mínimo de toda la simulación (además de los valores máximos y mínimos de cada frame).
- Agregar una barra lateral con la escala de colores.
