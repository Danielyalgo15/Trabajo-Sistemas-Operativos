# Trabajo-Sistemas-Operativos
## Threads (Hilos)

### 1. Introducción

Los hilos (Threads) son unidades de ejecución que permiten que un proceso realice varias tareas de manera concurrente. Comparten los recursos del mismo proceso, pero cada hilo puede ejecutar sus propias instrucciones de forma independiente.

En los sistemas operativos, los hilos permiten aprovechar mejor el procesador y realizar varias actividades sin necesidad de crear procesos independientes.

### 2. Descripción del código original

El programa proporcionado utiliza la biblioteca `pthread.h` para crear y administrar hilos en C++.

Su funcionamiento consiste en crear cinco hilos. Cada uno recibe un identificador y calcula un tiempo de espera aleatorio entre uno y cinco segundos. Después, muestra un mensaje indicando cuándo comienza, cuánto tiempo esperará y cuándo termina.

La función `pthread_create()` permite crear los hilos, mientras que `pthread_join()` hace que el programa principal espere a que cada uno finalice antes de terminar la ejecución.

### 3. Análisis crítico del código original

Al analizar el programa, se identificaron algunos aspectos que podían mejorarse:

- No se inicializa el generador de números aleatorios, por lo que la secuencia puede repetirse entre ejecuciones.
- Se utiliza `assert()` para comprobar errores, pero no proporciona mensajes descriptivos ni permite gestionar adecuadamente los fallos.
- Los datos de cada hilo se pasan mediante un arreglo de enteros, lo que limita la información que puede recibir cada uno.
- No existe una gestión específica cuando falla la creación de un hilo.

### 4. Mejoras implementadas

Se realizaron modificaciones para mejorar la organización y el control del programa:

- Se agregó `srand()` junto con `time(NULL)` para inicializar el generador de números aleatorios.
- Se implementó una estructura llamada `ThreadData` para almacenar el identificador y el tiempo de espera de cada hilo.
- Se reemplazó el uso de `assert()` por condiciones que comprueban los errores y muestran mensajes descriptivos.
- Se agregó un contador para registrar cuántos hilos se crearon correctamente y esperar únicamente a esos hilos mediante `pthread_join()`.

### 5. Comparación entre ambas versiones

El programa original cumple con la función de crear cinco hilos y esperar a que finalicen. Sin embargo, su gestión de errores es limitada y la información de cada hilo se transmite de una manera sencilla.

La versión mejorada conserva el funcionamiento principal, pero incorpora una mejor organización de los datos, inicialización de los números aleatorios y comprobación de errores. Esto permite que el programa sea más claro y tenga un mejor control durante su ejecución.

### 6. Evidencias

**Código original**

![Código original](evidencias/Captura%20de%20pantalla%202026-11-15%20183518.png)

**Resultado de la ejecución original**

![Resultado original](evidencias/Captura%20de%20pantalla%202026-11-15%20183529.png)

**Código mejorado**

![Código mejorado](evidencias/Captura%20de%20pantalla%202026-11-15%20184407.png)
![Resultado mejorado](evidencias/Captura%20de%20pantalla%202026-11-15%20184421.png)

**Resultado de la ejecución mejorada**

![Evidencia adicional](evidencias/Captura%20de%20pantalla%202026-11-15%20184500.png)

### 7. Conclusión

El desarrollo de esta actividad permitió comprender cómo funcionan los hilos y cómo pueden ejecutarse varias tareas de manera concurrente dentro de un mismo proceso. También permitió conocer la importancia de utilizar correctamente funciones como `pthread_create()` y `pthread_join()`, así como implementar mecanismos para controlar errores y organizar mejor la información compartida con cada hilo.

# Exclusión mutua (Mutual Exclusion)

## 8. Introducción

La exclusión mutua es un mecanismo utilizado en los sistemas operativos para evitar que varios hilos accedan y modifiquen al mismo tiempo un recurso compartido.

Su objetivo es proteger la sección crítica de un programa, evitando condiciones de carrera y garantizando que las operaciones sobre los datos compartidos se realicen de manera ordenada.

## 9. Descripción de los códigos originales

Se proporcionaron tres programas que muestran diferentes formas de trabajar con una variable compartida utilizando cinco hilos.

**Código 00:**

Crea cinco hilos que incrementan una variable global llamada `variable`. Cada hilo realiza cinco incrementos. Sin embargo, no utiliza ningún mecanismo de protección, por lo que pueden producirse condiciones de carrera.

**Código 01:**

Mantiene el funcionamiento del primer programa, pero permite al usuario ingresar la cantidad de incrementos que realizará cada hilo. A pesar de esta modificación, continúa sin utilizar mecanismos de exclusión mutua.

**Código 02:**

Incorpora un mutex para proteger la variable compartida. Utiliza `pthread_mutex_lock()` antes de incrementar la variable y `pthread_mutex_unlock()` después de realizar la operación, evitando que varios hilos modifiquen simultáneamente el mismo recurso.

## 10. Análisis crítico

Al revisar los programas, se identificaron los siguientes aspectos:

- Los dos primeros códigos no protegen la variable compartida, lo que puede provocar condiciones de carrera.
- El segundo y tercer programa utilizan `assert()` para comprobar errores, pero no muestran mensajes descriptivos.
- No se comprueba adecuadamente si la inicialización y destrucción del mutex se realizan correctamente.
- La información de los hilos se transmite mediante arreglos de identificadores, lo que puede organizarse mejor utilizando estructuras.

## 11. Mejoras implementadas

Se desarrolló una nueva versión tomando como base el tercer programa, que ya utiliza exclusión mutua.

Las mejoras realizadas fueron:

- Se implementó una estructura llamada `DatosHilo` para organizar el identificador y la cantidad de incrementos de cada hilo.
- Se agregó una validación para evitar que el usuario ingrese valores incorrectos o negativos.
- Se incorporó la comprobación de errores al crear y esperar los hilos.
- Se agregó el control de errores al bloquear y desbloquear el mutex.
- Se verificó la inicialización y destrucción del mutex.
- Se añadieron mensajes para informar cuándo finaliza cada hilo y cuál es el resultado final.

## 12. Comparación de resultados

Los primeros dos programas realizan operaciones sobre una variable compartida sin protección, por lo que existe el riesgo de obtener resultados incorrectos debido a las condiciones de carrera.

El tercer programa soluciona este problema mediante el uso de un mutex. La versión mejorada conserva esta protección e incorpora validaciones, mensajes de error y una mejor organización de los datos.

En la versión mejorada, cuando los cinco hilos terminan correctamente, el resultado esperado corresponde a cinco veces la cantidad de incrementos ingresada por el usuario.

## 13. Evidencias

En esta sección se presentan las capturas de los tres códigos originales, sus ejecuciones y la nueva versión mejorada.

### Código original 00
[Insertar captura del primer código]

### Ejecución del código 00
[Insertar captura del resultado]

### Código original 01
[Insertar captura del segundo código]

### Ejecución del código 01
[Insertar captura del resultado]

### Código original 02
[Insertar captura del tercer código]

### Ejecución del código 02
[Insertar captura del resultado]

### Código mejorado
[Insertar captura del nuevo código]

### Ejecución del código mejorado
[Insertar captura del resultado]

## 14. Conclusión

El desarrollo de esta actividad permitió comprender la importancia de la exclusión mutua en los sistemas concurrentes. Mediante los programas originales se observó el problema que puede producirse cuando varios hilos modifican una misma variable sin protección.

También se comprendió cómo el uso de mutex permite proteger una sección crítica y evitar condiciones de carrera. Finalmente, las mejoras implementadas permitieron desarrollar un programa con una mejor organización y un control más adecuado de los posibles errores.
