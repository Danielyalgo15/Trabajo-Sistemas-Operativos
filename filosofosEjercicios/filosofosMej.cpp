#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <mutex>

// Recurso compartido y su respectivo mecanismo de sincronización protegido
unsigned int fideos = 256;
std::mutex platoMutex;

class Filosofo {
private:
    int id;
    int estomago;
    double tiempoPensando;
    double tiempoComiendoSecs; // Mide el tiempo real transcurrido en segundos

public:
    Filosofo(int id) : id(id), estomago(0), tiempoPensando(0.0), tiempoComiendoSecs(0.0) {}

    void pensar() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(1.0, 5.0); // Rango optimizado de tiempo de pensamiento
        double tiempo = dis(gen);
        
        tiempoPensando += tiempo;
        std::cout << "Filosofo " << id << " está pensando (" << tiempo << "s)\n";
        std::this_thread::sleep_for(std::chrono::duration<double>(tiempo));
    }

    bool comer() {
        int cantidadComida = 0;

        // Sección crítica protegida mediante RAII (std::lock_guard)
        {
            std::lock_guard<std::mutex> lock(platoMutex);
            if (fideos == 0) {
                return false; // No quedan fideos
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 30);
            cantidadComida = dis(gen);

            if (fideos < static_cast<unsigned int>(cantidadComida)) {
                cantidadComida = fideos;
            }
            fideos -= cantidadComida;
        } // El mutex se libera automáticamente aquí al salir del ámbito

        // Medición real del tiempo de consumo
        auto inicioComida = std::chrono::steady_clock::now();
        
        estomago += cantidadComida;
        
        // Simulación de lapso de tiempo comiendo proporcional a la cantidad obtenida
        std::this_thread::sleep_for(std::chrono::milliseconds(cantidadComida * 15));
        
        auto finComida = std::chrono::steady_clock::now();
        std::chrono::duration<double> duracion = finComida - inicioComida;
        tiempoComiendoSecs += duracion.count();

        std::cout << "Filosofo " << id << " comió " << cantidadComida << " fideos. Estómago total: " << estomago << "\n";
        return true;
    }

    int getEstomago() const { return estomago; }
    int getId() const { return id; }
    double getTiempoPensando() const { return tiempoPensando; }
    double getTiempoComiendo() const { return tiempoComiendoSecs; }
};

int main() {
    int numFilosofos = 0;
    std::cout << "Fideos iniciales en el plato = " << fideos << std::endl;
    std::cout << "Ingrese cantidad de filósofos: ";
    if (!(std::cin >> numFilosofos) || numFilosofos <= 0) {
        std::cerr << "Cantidad inválida de filósofos." << std::endl;
        return 1;
    }

    std::vector<Filosofo> filosofos;
    std::vector<std::thread> threads;
    filosofos.reserve(numFilosofos);

    for (int i = 1; i <= numFilosofos; i++) {
        filosofos.emplace_back(i);
    }

    // Creación de hilos concurrentes seguros
    for (Filosofo& filosofo : filosofos) {
        threads.emplace_back([&filosofo]() {
            while (true) {
                // Verificación segura del recurso compartido bajo bloqueo para evitar carreras
                {
                    std::lock_guard<std::mutex> lock(platoMutex);
                    if (fideos == 0) break;
                }

                filosofo.pensar();

                {
                    std::lock_guard<std::mutex> lock(platoMutex);
                    if (fideos == 0) break;
                }

                if (!filosofo.comer()) {
                    break;
                }
            }
        });
    }

    // Sincronización final: espera a que todos los hilos terminen
    for (std::thread& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Procesamiento y despliegue de estadísticas
    std::cout << "\nESTADÍSTICAS FINALES \n";
    double tiempoTotalPensando = 0.0;
    double tiempoTotalComiendo = 0.0;
    int maxEstomago = -1;
    int maxFilosofoEstomago = 0;
    int totalEstomagos = 0;

    for (const Filosofo& filosofo : filosofos) {
        std::cout << "Estómago Filósofo " << filosofo.getId() << " = " << filosofo.getEstomago() << " fideos\n";

        if (filosofo.getEstomago() > maxEstomago) {
            maxEstomago = filosofo.getEstomago();
            maxFilosofoEstomago = filosofo.getId();
        }

        tiempoTotalPensando += filosofo.getTiempoPensando();
        tiempoTotalComiendo += filosofo.getTiempoComiendo();
        totalEstomagos += filosofo.getEstomago();
    }

    std::cout << "Fideos restantes en el plato = " << fideos << std::endl;
    std::cout << "Filósofo magordito: Filósofo " << maxFilosofoEstomago << " (" << maxEstomago << " fideos)\n";
    
    if (tiempoTotalPensando > 0.0) {
        std::cout << "\nPorcentaje de tiempo pensando por filósofo:\n";
        for (const Filosofo& filosofo : filosofos) {
            double pctPensando = (filosofo.getTiempoPensando() / tiempoTotalPensando) * 100.0;
            std::cout << "Filosofo " << filosofo.getId() << ": " << pctPensando << "%\n";
        }
    }

    if (tiempoTotalComiendo > 0.0) {
        std::cout << "\nPorcentaje de tiempo comiendo por filósofo:\n";
        for (const Filosofo& filosofo : filosofos) {
            double pctComiendo = (filosofo.getTiempoComiendo() / tiempoTotalComiendo) * 100.0;
            std::cout << "Filosofo " << filosofo.getId() << ": " << pctComiendo << "%\n";
        }
    }

    double promedioEstomagos = static_cast<double>(totalEstomagos) / numFilosofos;
    std::cout << "\nPromedio de fideos por estómago: " << promedioEstomagos << std::endl;

    return 0;
}
