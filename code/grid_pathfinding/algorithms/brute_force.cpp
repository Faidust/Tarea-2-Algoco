#include "../path_result.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

PathResult solveBruteForce(
    int n, int m, int v, int p,
    const vector<vector<int>>& types,
    const vector<vector<long long>>& values
) {
    PathResult mejor = {false, 0, "", 0};

    // Crear la primera secuencia de movimientos.
    string camino(n - 1, 'D');
    camino += string(m - 1, 'R');

    // Generar todas las permutaciones posibles.
    do {
        int i = 0;
        int j = 0;
        int vidas = v;
        long long puntaje = values[0][0];
        bool valido = true;

        // Evaluar la secuencia actual.
        for (char movimiento : camino) {

            if (movimiento == 'R') {
                j++;
            } else {
                i++;
            }

            // Verificar si encontramos una muralla.
            if (types[i][j] == 0) {
                valido = false;
                break;
            }

            // Verificar si encontramos una mina.
            if (types[i][j] == -1) {
                vidas--;
                puntaje -= p;

                if (vidas <= 0) {
                    valido = false;
                    break;
                }
            } else {
                puntaje += values[i][j];
            }
        }

        // Guardar el mejor camino valido.
        if (valido && (!mejor.found || puntaje > mejor.score)) {
            mejor.found = true;
            mejor.score = puntaje;
            mejor.path = camino;
            mejor.remaining_lives = vidas;
        }

    } while (next_permutation(camino.begin(), camino.end()));

    return mejor;
}
