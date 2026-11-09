"""
Script para generar gráficos a partir de las mediciones experimentales.
Lee los resultados desde data/measurements/ y guarda los gráficos en data/plots/.

Métricas a graficar:
  - Tiempo de ejecución vs tamaño (N * M) para cada algoritmo y tipo de grilla.
  - Uso de memoria vs tamaño para cada algoritmo.
  - Brecha de puntaje Delta = s(OPT) - s(Greedy) para casos donde greedy encuentra solución.
  - Tasa de falla de Greedy (% de casos con 'SIN SOLUCION') según tipo de grilla y dominio.
"""

import os
import matplotlib.pyplot as plt
import pandas as pd

MEASUREMENTS_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "measurements")
PLOTS_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "plots")

def main():
    os.makedirs(PLOTS_DIR, exist_ok=True)
    print("TODO: Leer archivos de mediciones en:", MEASUREMENTS_DIR)
    print("TODO: Generar figuras y guardarlas en:", PLOTS_DIR)

if __name__ == "__main__":
    main()
