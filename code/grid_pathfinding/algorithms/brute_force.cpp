#include "../path_result.h"
#include <vector>
#include <string>
#include <functional>

using namespace std;

// Fuerza bruta mediante recursion y backtracking.
// Se exploran todos los caminos validos sin optimizaciones.
// Referencia: Enunciado Tarea 2 INF-221.

PathResult solveBruteForce(int n, int m, int v, int p,
                          const vector<vector<int>>& types,
                          const vector<vector<long long>>& values) {

    PathResult mejor = {false, 0, "", 0};

    if (n <= 0 || m <= 0 || v <= 0) {
        return mejor;
    }

    string camino;

    // Funcion recursiva para explorar los caminos
    function<void(int, int, int, long long)> explorar =
        [&](int fila, int columna, int vidas, long long puntaje) {

        // Caso base: llegamos al destino
        if (fila == n - 1 && columna == m - 1) {

            if (!mejor.found || puntaje > mejor.score) {
                mejor = {true, puntaje, camino, vidas};
            }

            return;
        }

        // Explorar movimiento hacia la derecha
        if (columna + 1 < m) {

            int tipo = types[fila][columna + 1];

            if (tipo != 0 && !(tipo == -1 && vidas == 1)) {

                camino.push_back('R');

                if (tipo == -1) {
                    explorar(fila, columna + 1, vidas - 1,
                             puntaje - p);
                } else {
                    explorar(fila, columna + 1, vidas,
                             puntaje + values[fila][columna + 1]);
                }

                camino.pop_back();
            }
        }

        // Explorar movimiento hacia abajo
        if (fila + 1 < n) {

            int tipo = types[fila + 1][columna];

            if (tipo != 0 && !(tipo == -1 && vidas == 1)) {

                camino.push_back('D');

                if (tipo == -1) {
                    explorar(fila + 1, columna, vidas - 1,
                             puntaje - p);
                } else {
                    explorar(fila + 1, columna, vidas,
                             puntaje + values[fila + 1][columna]);
                }

                camino.pop_back();
            }
        }
    };

    // Comenzar desde la esquina superior izquierda
    explorar(0, 0, v, values[0][0]);

    return mejor;
}
