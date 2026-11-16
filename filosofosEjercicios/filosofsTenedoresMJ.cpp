#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

// Mutex global para evitar que los mensajes en consola se mezclen
mutex mutexConsola;

class Filo {
public:
    Filo(int id, int numFilos, int& fide, mutex& mutexFide, vector<mutex>& tenedores)
        : id(id), numFilos(numFilos), fide(fide), mutexFide(mutexFide),
          tenedores(tenedores), estmag(0), timPensa(0) {}

    void operator()() {
        while (true) {
            // Verificación segura antes de pensar
            {
                lock_guard<mutex> lock(mutexFide);
                if (fide <= 0) break;
            }

            pensa();

            // Verificación segura antes de intentar tomar tenedores
            {
                lock_guard<mutex> lock(mutexFide);
                if (fide <= 0) break;
            }

            comerConTenedores();
        }
    }

    int getId() const { return id; }
    int getEstmag() const { return estmag; }
    int getTimPensa() const { return timPensa; }

private:
    void pensa() {
        int timp = getNumAlea(1, 5);
        {
            lock_guard<mutex> lock(mutexConsola);
            cout << "Filosofo N°" << id << " está pensando (" << timp << "s)" << endl;
        }
        this_thread::sleep_for(chrono::seconds(timp));
        timPensa += timp;
    }

    // Se unifica tomar -> comer -> dejar en un mismo ámbito RAII para no perder el bloqueo
    void comerConTenedores() {
        int tenedorIzq = id - 1;
        int tenedorDer = id % numFilos; // Adaptado dinámicamente a N filósofos

        // Caso especial si solo hay 1 filósofo en la mesa
        if (numFilos == 1) {
            unique_lock<mutex> lockUnico(tenedores[0]);
            consumirFideos(1, 1);
            return;
        }

        // Jerarquía de recursos (evita Deadlock circular)
        int primero = min(tenedorIzq, tenedorDer);
        int segundo = max(tenedorIzq, tenedorDer);

        // std::scoped_lock (C++17) bloquea ambos tenedores sin deadlock y los libera al salir del método
        scoped_lock lockTenedores(tenedores[primero], tenedores[segundo]);

        {
            lock_guard<mutex> lock(mutexConsola);
            cout << "Filosofo N°" << id << " ha tomado los tenedores "
                 << primero + 1 << " y " << segundo + 1 << endl;
        }

        consumirFideos(primero + 1, segundo + 1);

        {
            lock_guard<mutex> lock(mutexConsola);
            cout << "Filosofo N°" << id << " ha dejado los tenedores "
                 << primero + 1 << " y " << segundo + 1 << endl;
        }
    } // Aquí scoped_lock libera automáticamente ambos tenedores de forma segura

    void consumirFideos(int t1, int t2) {
        int porci = 0;
        int estomagoAnterior = estmag;
        bool comio = false;

        {
            lock_guard<mutex> lock(mutexFide);
            if (fide > 0) {
                porci = min(getNumAlea(1, 5), fide);
                fide -= porci;
                estmag += porci;
                comio = true;
            }
        }

        if (comio) {
            {
                lock_guard<mutex> lock(mutexConsola);
                cout << "Filosofo N° " << id << " está comiendo (" << porci << ")" << endl;
                cout << "Filosofo N° " << id << " incrementó el valor del estómago ("
                     << estomagoAnterior << " + " << porci << " = " << estmag << ")" << endl;
            }
            // Simula el tiempo físico mientras sostiene los tenedores
            this_thread::sleep_for(chrono::milliseconds(300));
        }
    }

    int getNumAlea(int minVal, int maxVal) const {
        // thread_local evita recrear random_device en cada llamada y es seguro entre hilos
        thread_local mt19937 gen(random_device{}());
        uniform_int_distribution<int> dist(minVal, maxVal);
        return dist(gen);
    }

    int id;
    int numFilos;
    int& fide;
    mutex& mutexFide;
    vector<mutex>& tenedores;
    int estmag;
    int timPensa;
};

int main() {
    long long fideInput = 0;
    int numFilos = 0;

    cout << "\n--------SIMULADOR DE FILOSOFOS COMENSALES--------" << endl;
    cout << "\nBienvenido Usuario, Escriba los datos pedidos:" << endl;
    cout << "\n(Recuerda que el máximo de fideos es 2,147,483,647)" << endl;
    cout << "\n- Cuantos Fideos Pondras: ";

    if (!(cin >> fideInput) || fideInput <= 0 || fideInput > numeric_limits<int>::max()) {
        cout << "\n-------------------------------------------" << endl;
        cout << "¡Se produjo un error!, cantidad de fideos inválida o excede el máximo." << endl;
        return 1;
    }
    int fide = static_cast<int>(fideInput);

    cout << "\n- Escriba la cantidad de filósofos que habrá: ";
    if (!(cin >> numFilos) || numFilos <= 0) {
        cout << "\n-------------------------------------------" << endl;
        cout << "¡Se produjo un error!, debe haber al menos 1 filósofo." << endl;
        return 1;
    }

    cout << "\n-------------------------------------------" << endl;
    cout << "\nEjecutando procedimiento......." << endl;
    cout << "\n-------------------------------------------" << endl;

    mutex mutexFide;
    // Se crean exactamente N tenedores (1 por cada filósofo)
    vector<mutex> tenedores(numFilos);

    vector<Filo> filos;
    filos.reserve(numFilos);
    vector<thread> hilos;
    hilos.reserve(numFilos);

    for (int i = 0; i < numFilos; ++i) {
        filos.emplace_back(i + 1, numFilos, fide, mutexFide, tenedores);
    }

    for (int i = 0; i < numFilos; ++i) {
        hilos.emplace_back(ref(filos[i]));
    }

    for (int i = 0; i < numFilos; ++i) {
        if (hilos[i].joinable()) {
            hilos[i].join();
        }
    }

    cout << "\n-------------------------------------------" << endl;
    cout << "\n....La ejecución del programa ha terminado...." << endl;
    cout << "\nMostrando datos finales........" << endl;
    cout << "\n---------------Estadísticas:----------------\n" << endl;

    long long totalEstmag = 0;
    long long totalTimPensa = 0;

    for (int i = 0; i < numFilos; ++i) {
        cout << "+ Filosofo N°(" << filos[i].getId() << ") :" << endl;
        cout << "\t* Tiempo pensando: " << filos[i].getTimPensa() << "s" << endl;
        cout << "\t* Estómago: " << filos[i].getEstmag() << endl;
        totalEstmag += filos[i].getEstmag();
        totalTimPensa += filos[i].getTimPensa();
        cout << endl;
    }

    cout << "\n---------------Datos finales---------------\n" << endl;
    cout << "1.- Total de comida consumida por los filósofos: " << totalEstmag << endl;
    cout << "2.- Fideos que restan: " << fide << endl;
    cout << "3.- Total de tiempo pensando: " << totalTimPensa << "s" << endl;
    cout << "\n-------------------------------------------" << endl;
    cout << "\t\t\n-----FIN DE LA EJECUCIÓN------" << endl;

    return 0;
}
