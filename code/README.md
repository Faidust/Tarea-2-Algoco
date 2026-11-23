# Documentación

## Entrega

La entrega se realiza vía **aula.usm.cl** en formato `.zip`.

Esta tarea 2 incluye la implementación de los algoritmos, scripts para generar gráficos y casos de prueba, un programa principal para ejecutar los algoritmos y realizar mediciones y el informe.

## Búsqueda de caminos en grillas

Se analizaron 3 algoritmos de búsqueda de caminos en grillas:

1 - Greedy: Selecciona en cada movimiento la casilla que entrega el mayor puntaje inmediato, sin considerar las consecuencias futuras.

2 - Fuerza Bruta: Genera todas las secuencias posibles de movimientos y evalúa los caminos válidos para encontrar el de mayor puntaje.

3 - Programación Dinámica: Calcula y almacena los mejores puntajes para cada posición y cantidad de vidas disponibles, para luego reconstruir el mejor camino sin tener que evaluar todos los caminos posibles.

### Programa principal

El archivo grid_pathfinding.cpp lee los archivos de entrada (los cuales contienen las grillas), ejecuta el algoritmo seleccionado y mide su tiempo de ejecución y memoria utilizada, para luego guardar los resultados obtenidos en un archivo CSV en data/measurements.

Además, guarda el puntaje, camino encontrado y vidas restantes en data/grid_output.

### Scripts

1- grid_generator.py: genera las grillas utilizadas como casos de prueba, considerando distintos tamaños, tipos de grillas y dominios.

2- plot_generator.py: lee las mediciones obtenidas y genera automáticamente los gráficos de tiempo, memoria, brecha de puntaje y tasas de éxito y fallos en formato png.

## Ejecución

### Grid Pathfinding

Para generar las grillas de prueba se debe ejecutar "python3 scripts/grid_generator.py --seed 2026" desde /code/grid_pathfinding.

Para compilar los algoritmos se utiliza "make" desde /code/grid_pathfinding.

Para ejecutar todos los algoritmos en los casos de prueba se utiliza "make full" desde /code/grid_pathfinding. Fuerza Bruta solo se ejecuta cuando N+M es menor o igual a 32.

Para generar los gráficos de tiempo, memoria y calidad de las soluciones se debe ejecutar "python3 scripts/plot_generator.py" desde /code/grid_pathfinding.

## Repositorio de Github

[Enlace del repositorio de la Tarea 2]

