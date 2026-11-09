#include "../path_result.h"
#include <vector>
#include <string>

using namespace std;

// Implementacion de la regla Greedy del enunciado.
// En cada paso elige la celda de mayor ganancia inmediata.
// En caso de empate, prioriza el movimiento hacia la derecha.

PathResult solveGreedy(int n, int m, int v, int p,
                       const vector<vector<int>>& types,
                       const vector<vector<long long>>& values) {

    if (n <= 0 || m <= 0 || v <= 0) {
        return {false, 0, "", 0};
    }

    int fila = 0, columna = 0;
    int vidas = v;
    long long puntaje = values[0][0];
    string camino;

    // Continuar hasta llegar a la esquina inferior derecha
    while (fila != n - 1 || columna != m - 1) {

        // Comprobar si se puede avanzar a la derecha
        bool derecha = columna + 1 < m &&
            types[fila][columna + 1] != 0 &&
            !(types[fila][columna + 1] == -1 && vidas == 1);

        // Comprobar si se puede avanzar hacia abajo
        bool abajo = fila + 1 < n &&
            types[fila + 1][columna] != 0 &&
            !(types[fila + 1][columna] == -1 && vidas == 1);

        // No existen movimientos validos
        if (!derecha && !abajo) {
            return {false, 0, "", 0};
        }

        long long gananciaDerecha = 0;
        long long gananciaAbajo = 0;

        // Calcular ganancia de la celda derecha
        if (derecha) {
            if (types[fila][columna + 1] == -1) {
                gananciaDerecha = -p;
            } else {
                gananciaDerecha = values[fila][columna + 1];
            }
        }

        // Calcular ganancia de la celda inferior
        if (abajo) {
            if (types[fila + 1][columna] == -1) {
                gananciaAbajo = -p;
            } else {
                gananciaAbajo = values[fila + 1][columna];
            }
        }

        // Elegir la mayor ganancia, priorizando derecha
        if (derecha && (!abajo || gananciaDerecha >= gananciaAbajo)) {
            columna++;
            camino += 'R';
        } else {
            fila++;
            camino += 'D';
        }

        // Actualizar puntaje y vidas
        if (types[fila][columna] == -1) {
            vidas--;
            puntaje -= p;
        } else {
            puntaje += values[fila][columna];
        }
    }

    return {true, puntaje, camino, vidas};
}
