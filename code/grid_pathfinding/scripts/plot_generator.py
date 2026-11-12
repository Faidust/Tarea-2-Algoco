"""
Script para generar gráficos a partir de las mediciones experimentales.
Lee los resultados desde data/measurements/ y guarda los gráficos en data/plots/.

Métricas a graficar:
  - Tiempo de ejecución vs tamaño (N * M) para cada algoritmo y tipo de grilla.
  - Uso de memoria vs tamaño para cada algoritmo.
  - Brecha de puntaje Delta = s(OPT) - s(Greedy) para casos donde greedy encuentra solución.
  - Tasa de falla de Greedy (% de casos con 'SIN SOLUCION') según tipo de grilla y dominio.
"""


from pathlib import Path
import matplotlib.pyplot as plt
import pandas as pd

# Carpetas del proyecto
RAIZ = Path(__file__).resolve().parent.parent
CSV = RAIZ / "data" / "measurements" / "measurements.csv"
SALIDA = RAIZ / "data" / "plots"

TIPOS = ["libre", "murallas", "minas", "mixta"]
ALGORITMOS = ["greedy", "dp", "bruteforce"]


def guardar(nombre):
    plt.grid(True, alpha=0.25)
    plt.tight_layout()
    plt.savefig(SALIDA / nombre, dpi=180)
    plt.close()
    print("Generado:", nombre)


def cargar_datos():
    datos = pd.read_csv(CSV)

    # Compatibilidad con los encabezados originales
    datos = datos.rename(columns={
        "instance": "instancia",
        "algorithm": "algoritmo",
        "n": "filas",
        "m": "columnas",
        "found": "encontro_solucion",
        "score": "puntaje",
        "time_ms": "tiempo_ms",
        "peak_rss_kb": "memoria_maxima_kib"
    })

    # Obtener tipo de grilla y dominio del nombre
    partes = datos["instancia"].str.extract(
        r"^\d+_\d+_(libre|murallas|minas|mixta)_(D10|D1000)_[abc]$"
    )

    datos["tipo"] = partes[0]
    datos["dominio"] = partes[1]
    datos["tamano"] = datos["filas"] * datos["columnas"]

    if datos.duplicated(["instancia", "algoritmo"]).any():
        raise ValueError("Existen mediciones duplicadas")

    print("Mediciones encontradas:", len(datos))
    return datos


def graficar_tiempos(datos):
    # Comparacion de los algoritmos por tipo de grilla
    for tipo in TIPOS:
        plt.figure(figsize=(9, 5.5))

        for algoritmo in ALGORITMOS:
            sub = datos[
                (datos["tipo"] == tipo) &
                (datos["algoritmo"] == algoritmo)
            ]

            resumen = sub.groupby("tamano")["tiempo_ms"].median()

            if not resumen.empty:
                plt.plot(
                    resumen.index, resumen.values,
                    marker="o", label=algoritmo
                )

        plt.xscale("log")
        plt.yscale("log")
        plt.title("Tiempo de ejecucion - " + tipo)
        plt.xlabel("Tamano de grilla (N x M)")
        plt.ylabel("Tiempo mediano (ms)")
        plt.legend()

        guardar("tiempo_" + tipo + ".png")


def graficar_memoria(datos):
    plt.figure(figsize=(9, 5.5))

    for algoritmo in ALGORITMOS:
        sub = datos[datos["algoritmo"] == algoritmo]

        resumen = sub.groupby("tamano")[
            "memoria_maxima_kib"
        ].median()

        if not resumen.empty:
            plt.plot(
                resumen.index, resumen.values,
                marker="o", label=algoritmo
            )

    plt.xscale("log")
    plt.title("Memoria maxima por algoritmo")
    plt.xlabel("Tamano de grilla (N x M)")
    plt.ylabel("Pico de memoria del proceso (KiB)")
    plt.legend()

    guardar("memoria_algoritmos.png")


def graficar_brecha(datos):
    # DP representa el puntaje optimo
    optimo = datos[datos["algoritmo"] == "dp"][
        ["instancia", "puntaje"]
    ].rename(columns={"puntaje": "puntaje_optimo"})

    greedy = datos[
        (datos["algoritmo"] == "greedy") &
        (datos["encontro_solucion"] == 1)
    ]

    comparacion = greedy.merge(optimo, on="instancia")

    # Delta = OPT - Greedy
    comparacion["brecha"] = (
        comparacion["puntaje_optimo"] -
        comparacion["puntaje"]
    )

    if (comparacion["brecha"] < 0).any():
        raise ValueError("Greedy supera al optimo en alguna instancia")

    for dominio in ["D10", "D1000"]:
        plt.figure(figsize=(9, 5.5))

        for tipo in TIPOS:
            sub = comparacion[
                (comparacion["dominio"] == dominio) &
                (comparacion["tipo"] == tipo)
            ]

            resumen = sub.groupby("tamano")["brecha"].mean()

            if not resumen.empty:
                plt.plot(
                    resumen.index, resumen.values,
                    marker="o", label=tipo
                )

        plt.xscale("log")
        plt.title("Brecha de puntaje - " + dominio)
        plt.xlabel("Tamano de grilla (N x M)")
        plt.ylabel("Brecha media de Greedy")
        plt.legend()

        guardar("brecha_" + dominio + ".png")


def graficar_fallas(datos):
    greedy = datos[datos["algoritmo"] == "greedy"]

    tasas = greedy.groupby(
        ["tipo", "dominio"]
    )["encontro_solucion"].apply(
        lambda valores: 100 * (1 - valores.mean())
    )

    posiciones = list(range(len(TIPOS)))

    plt.figure(figsize=(9, 5.5))

    for dominio, desplazamiento in [
        ("D10", -0.2),
        ("D1000", 0.2)
    ]:
        alturas = [
            tasas.get((tipo, dominio), 0)
            for tipo in TIPOS
        ]

        plt.bar(
            [x + desplazamiento for x in posiciones],
            alturas,
            width=0.38,
            label=dominio
        )

    plt.xticks(posiciones, TIPOS)
    plt.ylim(0, 100)
    plt.title("Tasa de falla de Greedy")
    plt.xlabel("Tipo de grilla")
    plt.ylabel("Casos sin solucion (%)")
    plt.legend()

    guardar("fallas_greedy.png")


def main():
    SALIDA.mkdir(parents=True, exist_ok=True)

    datos = cargar_datos()

    graficar_tiempos(datos)
    graficar_memoria(datos)
    graficar_brecha(datos)
    graficar_fallas(datos)

    print("Graficos generados correctamente")


if __name__ == "__main__":
    main()

