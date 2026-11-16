#include <iostream>
#include <pthread.h>
#include <cstring>
#include <limits>
#define NUM_HILOS 5
int variable = 0;
pthread_mutex_t mutex;
struct DatosHilo {
    int identificador;
    int tope;
};
void* incrementar(void* argumento) {
    DatosHilo* datos = (DatosHilo*)argumento;
    for (int i = 0; i < datos->tope; i++) {
        int resultado = pthread_mutex_lock(&mutex);
        if (resultado != 0) {
            std::cerr << "Error al bloquear el mutex: "
                      << strerror(resultado) << std::endl;
            return nullptr;
        }
        variable++;
        resultado = pthread_mutex_unlock(&mutex);
        if (resultado != 0) {
            std::cerr << "Error al desbloquear el mutex: "
                      << strerror(resultado) << std::endl;
            return nullptr;
        }
    }
    std::cout << "Hilo " << datos->identificador
              << " finalizado correctamente." << std::endl;

    return nullptr;
}
int main() {
    pthread_t hilos[NUM_HILOS];
    DatosHilo datos[NUM_HILOS];
    int tope;
    std::cout << "Ingrese el numero de incrementos por hilo: ";
    if (!(std::cin >> tope) || tope <= 0) {
        std::cerr << "Error: ingrese un numero entero positivo."
                  << std::endl;
        return 1;
    }
    int resultado = pthread_mutex_init(&mutex, nullptr);
    if (resultado != 0) {
        std::cerr << "Error al inicializar el mutex: "
                  << strerror(resultado) << std::endl;
        return 1;
    }
    int hilos_creados = 0;
    bool error = false;
    std::cout << "\nValor inicial: " << variable << std::endl;
    for (int i = 0; i < NUM_HILOS; i++) {
        datos[i].identificador = i;
        datos[i].tope = tope;
        resultado = pthread_create(
            &hilos[i], nullptr, incrementar, &datos[i]
        );
        if (resultado != 0) {
            std::cerr << "Error al crear el hilo " << i
                      << ": " << strerror(resultado) << std::endl;
            error = true;
            break;
        }
        hilos_creados++;
    }
    for (int i = 0; i < hilos_creados; i++) {
        resultado = pthread_join(hilos[i], nullptr);
        if (resultado != 0) {
            std::cerr << "Error al esperar el hilo " << i
                      << ": " << strerror(resultado) << std::endl;
            error = true;
        }
    }
    resultado = pthread_mutex_destroy(&mutex);
    if (resultado != 0) {
        std::cerr << "Error al destruir el mutex: "
                  << strerror(resultado) << std::endl;
        error = true;
    }
    std::cout << "\nValor final: " << variable << std::endl;
    std::cout << "Incrementos esperados: "
              << hilos_creados * tope << std::endl;
    if (!error && hilos_creados == NUM_HILOS) {
        std::cout << "Todos los hilos terminaron correctamente."
                  << std::endl;
    }
    return error ? 1 : 0;
}