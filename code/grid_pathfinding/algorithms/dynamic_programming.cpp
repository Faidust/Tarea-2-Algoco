#include "../path_result.h"
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <cstddef>

using namespace std;

// Programacion dinamica bottom-up.
// El estado considera la posicion y las vidas restantes.
// Se guarda tambien el movimiento anterior para reconstruir el camino.

PathResult solveDP(int n, int m, int v, int p,
                   const vector<vector<int>>& types,
                   const vector<vector<long long>>& values) {

    PathResult sinSolucion = {false, 0, "", 0};

    if (n <= 0 || m <= 0 || v <= 0 ||
        types[0][0] != 1 || types[n - 1][m - 1] != 1) {
        return sinSolucion;
    }

    const long long NEG_INF = numeric_limits<long long>::lowest();

    // Tablas para puntajes y reconstruccion del camino
    const size_t cantidad = static_cast<size_t>(n) * m * (v + 1);
    vector<long long> dp(cantidad, NEG_INF);
    vector<char> padre(cantidad, '\0');

    // Convertir el estado (fila, columna, vidas) en un indice
    auto indice = [m, v](int fila, int columna, int vidas) -> size_t {
        return (static_cast<size_t>(fila) * m + columna) * (v + 1) + vidas;
    };

    // Caso inicial
    dp[indice(0, 0, v)] = values[0][0];

    // Recorrer toda la grilla
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {

            if ((i == 0 && j == 0) || types[i][j] == 0) {
                continue;
            }

            bool esMina = (types[i][j] == -1);
            long long ganancia = esMina ? -static_cast<long long>(p)
                                       : values[i][j];

            // Evaluar cada cantidad posible de vidas restantes
            for (int vidas = 1; vidas <= v; vidas++) {

                int vidasAntes = vidas + (esMina ? 1 : 0);

                if (vidasAntes > v) {
                    continue;
                }

                size_t actual = indice(i, j, vidas);

                // Llegar desde la izquierda (movimiento R)
                if (j > 0) {
                    size_t anterior = indice(i, j - 1, vidasAntes);

                    if (dp[anterior] != NEG_INF) {
                        long long candidato = dp[anterior] + ganancia;

                        if (candidato > dp[actual]) {
                            dp[actual] = candidato;
                            padre[actual] = 'R';
                        }
                    }
                }

                // Llegar desde arriba (movimiento D)
                if (i > 0) {
                    size_t anterior = indice(i - 1, j, vidasAntes);

                    if (dp[anterior] != NEG_INF) {
                        long long candidato = dp[anterior] + ganancia;

                        if (candidato > dp[actual]) {
                            dp[actual] = candidato;
                            padre[actual] = 'D';
                        }
                    }
                }
            }
        }
    }

    // Buscar el mejor resultado en la celda final
    long long mejorPuntaje = NEG_INF;
    int mejoresVidas = 0;

    for (int vidas = v; vidas >= 1; vidas--) {
        long long puntaje = dp[indice(n - 1, m - 1, vidas)];

        if (puntaje > mejorPuntaje) {
            mejorPuntaje = puntaje;
            mejoresVidas = vidas;
        }
    }

    if (mejoresVidas == 0) {
        return sinSolucion;
    }

    // Reconstruir el camino desde el destino hasta el inicio
    string camino;
    int i = n - 1, j = m - 1;
    int vidas = mejoresVidas;

    while (i != 0 || j != 0) {

        char movimiento = padre[indice(i, j, vidas)];

        if (movimiento != 'R' && movimiento != 'D') {
            return sinSolucion;
        }

        camino.push_back(movimiento);

        // Recuperar la vida consumida si la celda actual es mina
        if (types[i][j] == -1) {
            vidas++;
        }

        if (movimiento == 'R') {
            j--;
        } else {
            i--;
        }
    }

    // El recorrido fue reconstruido al reves
    reverse(camino.begin(), camino.end());

    return {true, mejorPuntaje, camino, mejoresVidas};
}
