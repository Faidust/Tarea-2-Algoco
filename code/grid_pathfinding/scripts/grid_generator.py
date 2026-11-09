"""
Generador de casos de prueba -- Tarea 2, Algoritmos y Complejidad (INF-221).

Genera grillas para el problema de grid pathfinding con murallas y minas,
siguiendo el formato descrito en el apéndice «Casos de prueba» del enunciado.

Formato de cada archivo {n}_{m}_{t}_{d}_{s}.txt:
    N M V P
    <N líneas con M identificadores de tipo: 1 normal, 0 muralla, -1 mina>
    <N líneas con M valores (0 en murallas y minas)>

Garantías:
    - Las celdas (1,1) y (N,M) son siempre celdas normales.
    - Existe al menos un camino válido: antes de ubicar murallas y minas se
      reserva un camino monótono aleatorio (solo movimientos R y D) que queda
      libre de murallas y minas.

Uso (desde cualquier directorio):
    python grid_generator.py                 # genera todos los casos
    python grid_generator.py --max-n 16      # solo grillas con N <= 16
    python grid_generator.py --seed 2026     # resultados reproducibles
"""

import argparse
import math
import os
from itertools import product

import numpy as np

# --- Parámetros de los casos de prueba (ver enunciado) ---
NS = [2**2, 2**3, 2**4, 2**6, 2**8, 2**10]       # filas N
FACTORES_M = [1, 2]                               # M in {N, 2N}
TS = ["libre", "murallas", "minas", "mixta"]      # tipos de grilla
DS = {"D10": 10, "D1000": 1000}                   # dominio [-x, x] y penalización P = x
SS = ["a", "b", "c"]                              # muestras
V = 3                                             # vidas iniciales

# --- Identificadores de tipo de celda ---
NORMAL = 1
MURALLA = 0
MINA = -1

CARPETA_SALIDA = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "data", "grid_input"
)


def camino_reservado(n, m, rng):
    """
    Retorna una máscara booleana n x m con las celdas de un camino monótono
    aleatorio desde (0,0) hasta (n-1,m-1).
    """
    movimientos = np.array(["D"] * (n - 1) + ["R"] * (m - 1))
    rng.shuffle(movimientos)

    reservado = np.zeros((n, m), dtype=bool)
    i, j = 0, 0
    reservado[i, j] = True
    for mov in movimientos:
        if mov == "D":
            i += 1
        else:
            j += 1
        reservado[i, j] = True
    return reservado


def generar_grilla(n, m, tipo, dominio, rng):
    """
    Genera las matrices de identificadores y de valores de una grilla n x m.
    - tipo: 'libre', 'murallas', 'minas' o 'mixta'
    - dominio: 'D10' (valores en [-10, 10]) o 'D1000' (valores en [-1000, 1000])
    """
    if dominio not in DS:
        raise ValueError(f"Dominio no válido: {dominio}. Usa {list(DS)}.")
    if tipo not in TS:
        raise ValueError(f"Tipo no válido: {tipo}. Usa {TS}.")

    x = DS[dominio]
    ids = np.full((n, m), NORMAL, dtype=np.int8)
    valores = rng.integers(-x, x + 1, size=(n, m), dtype=np.int64)

    k = math.isqrt(n * m)  # K = floor(sqrt(N * M))
    k_murallas = k if tipo in ("murallas", "mixta") else 0
    k_minas = k if tipo in ("minas", "mixta") else 0

    if k_murallas + k_minas > 0:
        # Celdas disponibles: todas las que no pertenecen al camino reservado
        # (el camino incluye (0,0) y (n-1,m-1)).
        reservado = camino_reservado(n, m, rng)
        disponibles = np.flatnonzero(~reservado)
        total = min(k_murallas + k_minas, disponibles.size)
        elegidas = rng.choice(disponibles, size=total, replace=False)

        k_murallas = min(k_murallas, total)
        ids.flat[elegidas[:k_murallas]] = MURALLA
        ids.flat[elegidas[k_murallas:]] = MINA

    valores[ids != NORMAL] = 0
    return ids, valores


def guardar_grilla(ruta, n, m, v, p, ids, valores):
    """
    Guarda la grilla en el formato del enunciado.
    """
    with open(ruta, "w") as f:
        f.write(f"{n} {m} {v} {p}\n")
        np.savetxt(f, ids, fmt="%d", delimiter=" ")
        np.savetxt(f, valores, fmt="%d", delimiter=" ")


def generar_todos(max_n=None, seed=None, carpeta=CARPETA_SALIDA):
    rng = np.random.default_rng(seed)
    os.makedirs(carpeta, exist_ok=True)

    ns = [n for n in NS if max_n is None or n <= max_n]
    casos = list(product(ns, FACTORES_M, TS, DS, SS))
    print(f"Generando {len(casos)} grillas en {os.path.normpath(carpeta)} ...")

    for n, factor, t, d, s in casos:
        m = factor * n
        ids, valores = generar_grilla(n, m, t, d, rng)
        nombre = f"{n}_{m}_{t}_{d}_{s}.txt"
        guardar_grilla(os.path.join(carpeta, nombre), n, m, V, DS[d], ids, valores)
        print(f"Generado: {nombre}")

    print("Todas las grillas han sido generadas.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generador de grillas para la Tarea 2.")
    parser.add_argument("--max-n", type=int, default=None,
                        help="Genera solo grillas con N <= MAX_N.")
    parser.add_argument("--seed", type=int, default=None,
                        help="Semilla para obtener resultados reproducibles.")
    args = parser.parse_args()

    generar_todos(max_n=args.max_n, seed=args.seed)
