El Problema de los Filósofos (C++ Multithreading)

Este proyecto presenta un análisis crítico, reflexivo y la refactorización técnica de una simulación concurrente en C++ utilizando hilos de ejecución (std::thread) y exclusión mutua (std::mutex). El objetivo es resolver problemas clásicos de concurrencia como las condiciones de carrera (Data Races), el manejo inseguro de bloqueos y la inconsistencia en la recolección de métricas.

1. Análisis Crítico y Reflexivo del Código Original

Al auditar la implementación inicial, se identificaron cuatro deficiencias fundamentales a nivel de arquitectura concurrente y lógica de programación:

A. Condiciones de Carrera (Data Races) en el Recurso Compartido

Hallazgo: En la función main(), las evaluaciones while (fideos > 0) e if (fideos > 0) se ejecutaban fuera de cualquier sección crítica protegida.

Reflexión: Cuando múltiples hilos leen y escriben simultáneamente sobre una variable global sin sincronización, el comportamiento del programa es indefinido. Un hilo puede leer un valor en caché desactualizado mientras otro ya agotó el recurso, generando inconsistencias en el flujo de terminación.

B. Inconsistencia Semántica y Lógica en Métricas

Hallazgo: Dentro del método comer(), la instrucción tiempoComiendo += cantidad; sumaba la cantidad de fideos consumidos dentro de una variable destinada a contabilizar tiempo en segundos.

Reflexión: Mezclar unidades físicas distintas (cantidad de elementos vs. tiempo transcurrido) invalida por completo los porcentajes y estadísticas finales del sistema, produciendo reportes erróneos.

C. Violación del Principio RAII (Resource Acquisition Is Initialization)

Hallazgo: El bloqueo del mutex se realizaba mediante llamadas manuales a mutex.lock() y mutex.unlock().

Reflexión: En C++ moderno, la gestión manual de recursos es una práctica riesgosa. Si ocurre una excepción o un retorno anticipado antes de alcanzar el unlock(), el mutex permanece bloqueado indefinidamente, provocando un interbloqueo total (Deadlock). El uso de std::lock_guard garantiza la liberación automática al salir del ámbito (scope).

D. Riesgo de Contención y Desorden en Salida Estándar

Hallazgo: Los hilos imprimían mensajes directamente en std::cout sin sincronización, y el tiempo de espera no reflejaba una acción real de consumo.

Reflexión: Aunque std::cout es seguro a nivel de caracteres individuales en C++11, la intercalación de cadenas completas desde múltiples hilos degrada la legibilidad de la consola. Asimismo, simular el tiempo de ingesta con relojes de alta precisión (std::chrono::steady_clock) aporta realismo a la concurrencia.

2. Evidencias de Ejecución (Comparativa)

A continuación, se presentan las capturas de pantalla que evidencian el comportamiento antes y después de aplicar las mejoras de sincronización y refactorización.

Ejecución del Código Original

Nota: En esta captura se observa la ejecución inicial antes de corregir las condiciones de carrera y el cálculo real de tiempos.


/home/jhh/Descargas/SO/filosofosEjercicios/capturas/filosofs.png

Ejecución del Código Mejorado

Nota: En esta captura se evidencia la ejecución estable bajo std::lock_guard, la ausencia de condiciones de carrera y el cálculo preciso de porcentajes de tiempo real.


/home/jhh/Descargas/SO/filosofosEjercicios/capturas/filosofMejora.png

3. Guía de Compilación y Ejecución (Linux )

Para compilar y ejecutar la versión mejorada en un entorno Linux basado en Debian/Ubuntu (como Pop!_OS), asegúrate de tener instalado el compilador g++.

Paso 1: Compilar el código fuente

Abre la terminal en el directorio raíz del proyecto y ejecuta:

g++ -std=c++11 -pthread filosofos.cpp -o filosofos


-std=c++11: Habilita el estándar C++11 necesario para std::thread, std::mutex y std::chrono.

-pthread: Enlaza la biblioteca nativa de hilos POSIX requerida en sistemas Linux.

Paso 2: Ejecutar el binario

Inicia la simulación ejecutando:

./filosofos


4. Conclusiones

Seguridad en Concurrencia: La encapsulación de todas las lecturas y escrituras de la variable global fideos dentro de bloques protegidos por std::lock_guard elimina por completo las condiciones de carrera.

Integridad de Datos: El uso de std::chrono::steady_clock permite medir con exactitud el tiempo real invertido por cada hilo en la tarea de comer, garantizando estadísticas coherentes.



## Evolución hacia el Modelo Clásico con Tenedores (Fase 2)

En esta segunda etapa del proyecto, el sistema evoluciona desde un bloqueo centralizado (un único mutex para todo el plato de fideos) hacia el **problema clásico de los Filósofos Comensales de Dijkstra**, donde cada filósofo debe competir por adquirir dos recursos independientes y contiguos: su tenedor izquierdo y su tenedor derecho.

---

###Análisis 

Al analizar la primera iteración de esta implementación basada en tenedores individuales, se identificaron fallas críticas de concurrencia y diseño:

1. **Destrucción Prematura de Bloqueos (RAII vs. `unlock()` Manual):**
   * *Problema:* En la función original `tomarTenedores()`, se declaraban objetos `unique_lock` locales. Al salir de la función, estos se destruían liberando los mutex de inmediato, lo que provocaba que llamadas posteriores a `dejarTenedores()` ejecutaran `.unlock()` sobre mutex ya liberados, generando **Comportamiento Indefinido (Undefined Behavior)** y excepciones de sistema.
2. **Escalabilidad Estática y `Segmentation Fault` (`numFilos != 5`):**
   * *Problema:* El vector de tenedores estaba hardcodeado a un tamaño fijo de 5 (`vector<mutex> Filo::tenedores(5);`) usando aritmética modular fija (`id % 5`). Si el usuario ingresaba una cantidad de filósofos distinta de 5, el sistema sufría un fallo de segmentación o rompía la topología circular de la mesa.
3. **Falsa Validación de Límites (`LimitFideMax`):**
   * *Problema:* Se intentaba validar un tope entero de `2,147,483,647` mediante una función, pero al ser `fide` un tipo `int` estándar, el desbordamiento (*overflow*) ocurría directamente durante la lectura por consola (`cin >> fide`) antes de llegar a la validación.
4. **Condiciones de Carrera en Consola:**
   * *Problema:* Múltiples hilos imprimían concurrentemente mediante `std::cout` sin sincronización, provocando el entrelazamiento indeseado de los mensajes en pantalla.
/home/jhh/Descargas/SO/filosofosEjercicios/capturas/tenedores.png

---

###  Soluciones y Mejoras Aplicadas

* **Uso de `std::scoped_lock` (C++17):** Se reemplazó la gestión manual por el bloqueo seguro de múltiples mutex de forma simultánea, aplicando además una **jerarquía de recursos** (ordenar por índice menor y mayor) para prevenir interbloqueos (*deadlocks*).
* **Dinamismo Estructural:** Los tenedores se inicializan dinámicamente en función del número de filósofos ingresados por el usuario (`vector<mutex> tenedores(numFilos)`), permitiendo una mesa flexible de `N` comensales.
* **Sincronización de Salida (`mutexConsola`):** Se blindaron todas las impresiones en consola para mantener un registro limpio y ordenado de los estados de cada hilo.
/home/jhh/Descargas/SO/filosofosEjercicios/capturas/filosodostendrsMJ.png

---

### ⚙Instrucciones de Compilación y Ejecución (C++17)

Dado que se utilizan características modernas de concurrencia de C++17 (`std::scoped_lock`), asegúrate de compilar con el estándar correcto en **Pop!_OS Linux**:

```bash
# 1. Compilar especificando el estándar C++17 y el soporte para hilos POSIX (-pthread)
g++ -std=c++17 -pthread filosofsTenedoresMJ.cpp -o filosofosTenedrsmj

# 2. Ejecutar el binario resultante
./filosofosTenedrsmj