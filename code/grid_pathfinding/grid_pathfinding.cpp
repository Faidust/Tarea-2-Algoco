#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/resource.h>  // getrusage (Linux/WSL)
#include <vector>
#include "path_result.h"

using namespace std;
namespace fs = std::filesystem;

struct Grid {
    int n, m, v, p;
    vector<vector<int>> types;
    vector<vector<long long>> values;
};

PathResult solveBruteForce(int n, int m, int v, int p,
                          const vector<vector<int>>& types,
                          const vector<vector<long long>>& values);

PathResult solveDP(int n, int m, int v, int p,
                   const vector<vector<int>>& types,
                   const vector<vector<long long>>& values);

PathResult solveGreedy(int n, int m, int v, int p,
                       const vector<vector<int>>& types,
                       const vector<vector<long long>>& values);

// Leer la grilla desde el archivo de entrada
bool cargarGrilla(const string& ruta, Grid& grid) {
    ifstream archivo(ruta);

    if (!archivo) {
        cerr << "Error al abrir: " << ruta << '\n';
        return false;
    }

    if (!(archivo >> grid.n >> grid.m >> grid.v >> grid.p) ||
        grid.n <= 0 || grid.m <= 0 ||
        grid.v <= 0 || grid.p < 0) {
        cerr << "Encabezado de grilla invalido\n";
        return false;
    }

    grid.types.assign(grid.n, vector<int>(grid.m));
    grid.values.assign(grid.n, vector<long long>(grid.m));

    // Leer matriz de tipos
    for (int i = 0; i < grid.n; ++i) {
        for (int j = 0; j < grid.m; ++j) {
            if (!(archivo >> grid.types[i][j]) ||
                (grid.types[i][j] != -1 &&
                 grid.types[i][j] != 0 &&
                 grid.types[i][j] != 1)) {
                cerr << "Matriz de tipos invalida\n";
                return false;
            }
        }
    }

    // Leer matriz de valores
    for (int i = 0; i < grid.n; ++i) {
        for (int j = 0; j < grid.m; ++j) {
            if (!(archivo >> grid.values[i][j])) {
                cerr << "Matriz de valores incompleta\n";
                return false;
            }
        }
    }

    if (grid.types[0][0] != 1 ||
        grid.types[grid.n - 1][grid.m - 1] != 1) {
        cerr << "Inicio y destino deben ser celdas normales\n";
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        cerr << "Uso: " << argv[0]
             << " <greedy|dp|bruteforce> <archivo_entrada>\n";
        return 1;
    }

    const string algoritmo = argv[1];

    if (algoritmo != "greedy" &&
        algoritmo != "dp" &&
        algoritmo != "bruteforce") {
        cerr << "Algoritmo desconocido: " << algoritmo << '\n';
        return 1;
    }

    Grid grid;

    if (!cargarGrilla(argv[2], grid)) {
        return 1;
    }

    // Respetar el limite de fuerza bruta
    if (algoritmo == "bruteforce" && grid.n + grid.m > 32) {
        cerr << "Fuerza bruta omitida: N + M > 32\n";
        return 1;
    }

    // Medir tiempo de ejecucion del algoritmo
    auto inicio = chrono::steady_clock::now();

    PathResult resultado;

    if (algoritmo == "greedy") {
        resultado = solveGreedy(
            grid.n, grid.m, grid.v, grid.p,
            grid.types, grid.values
        );
    }
    else if (algoritmo == "dp") {
        resultado = solveDP(
            grid.n, grid.m, grid.v, grid.p,
            grid.types, grid.values
        );
    }
    else {
        resultado = solveBruteForce(
            grid.n, grid.m, grid.v, grid.p,
            grid.types, grid.values
        );
    }

    auto fin = chrono::steady_clock::now();

    double tiempoMs =
        chrono::duration<double, milli>(fin - inicio).count();

    // Medir memoria del proceso
    struct rusage uso {};

    if (getrusage(RUSAGE_SELF, &uso) != 0) {
        cerr << "No se pudo medir memoria\n";
        return 1;
    }

    long memoriaKB = uso.ru_maxrss;

    // Verificar que fuerza bruta y DP coincidan
    // Esta comprobacion no se incluye en el tiempo medido
    if (algoritmo == "bruteforce") {

        PathResult referencia = solveDP(
            grid.n, grid.m, grid.v, grid.p,
            grid.types, grid.values
        );

        if (resultado.found != referencia.found ||
            (resultado.found &&
             resultado.score != referencia.score)) {
            cerr << "ERROR: Fuerza bruta y DP no coinciden\n";
            return 1;
        }
    }

    // Crear carpetas de salida
    fs::create_directories("data/grid_output");
    fs::create_directories("data/measurements");

    string instancia = fs::path(argv[2]).stem().string();

    fs::path salida = fs::path("data/grid_output") /
        (instancia + "_" + algoritmo + "_out.txt");

    // Guardar resultado del algoritmo
    ofstream archivoSalida(salida);

    if (!archivoSalida) {
        cerr << "No se pudo escribir: " << salida << '\n';
        return 1;
    }

    if (resultado.found) {
        archivoSalida << resultado.score << '\n'
                      << resultado.path << '\n'
                      << resultado.remaining_lives << '\n';
    }
    else {
        archivoSalida << "SIN SOLUCION\n";
    }

    archivoSalida.close();

    // Guardar mediciones experimentales
    fs::path mediciones = "data/measurements/measurements.csv";

    bool cabecera =
        !fs::exists(mediciones) ||
        fs::file_size(mediciones) == 0;

    ofstream csv(mediciones, ios::app);

    if (!csv) {
        cerr << "No se pudieron guardar mediciones\n";
        return 1;
    }

    if (cabecera) {
        csv << "instancia,algoritmo,filas,columnas,vidas_iniciales,"
            "penalizacion_mina,encontro_solucion,puntaje,"
            "tiempo_ms,memoria_maxima_kib\n";
    }

    csv << instancia << ','
        << algoritmo << ','
        << grid.n << ','
        << grid.m << ','
        << grid.v << ','
        << grid.p << ','
        << (resultado.found ? 1 : 0) << ',';

    if (resultado.found) {
        csv << resultado.score;
    }
    else {
        csv << "NA";
    }

    csv << ','
        << fixed << setprecision(6) << tiempoMs
        << ',' << memoriaKB << '\n';

    // Mostrar resumen por terminal
    cout << instancia << " | " << algoritmo << " | ";

    if (resultado.found) {
        cout << "puntaje " << resultado.score;
    }
    else {
        cout << "SIN SOLUCION";
    }

    cout << " | "
         << fixed << setprecision(3) << tiempoMs
         << " ms | " << memoriaKB << " KiB\n";

    return 0;
}