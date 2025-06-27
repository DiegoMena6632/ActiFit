# ActiFit

## Descripción de la aplicación
**ActiFit** es una aplicación de consola que permite a los usuarios generar, visualizar y modificar rutinas de entrenamiento personalizadas según sus preferencias, equipamiento disponible y datos personales. El sistema carga ejercicios desde un archivo CSV y ofrece un menú interactivo para gestionar el equipamiento, crear rutinas, modificarlas y ver un resumen semanal del progreso.

## Funciones implementadas
1. Cargar ejercicios desde un archivo CSV.
2. Registrar datos personales del usuario (nombre, género, peso, altura).
3. Calcular el IMC del usuario y sugerir la cantidad de ejercicios diaria.
4. Gestionar el equipamiento disponible del usuario.
5. Generar una rutina semanal personalizada según preferencias y equipamiento.
6. Visualizar la rutina semanal.
7. Modificar ejercicios de la rutina por otros compatibles.
8. Mostrar un resumen semanal: cantidad de ejercicios, tiempo total y calorías quemadas.

## Compilar el código 

1. Instala [Visual Studio Code](https://code.visualstudio.com/) y la extensión de C/C++.
2. Asegúrate de tener un compilador de C instalado por ejemplo, MinGW en Windows.
3. Clona o descarga este repositorio en tu equipo.
4. Abre una terminal en la carpeta del proyecto y ejecuta el siguiente comando:

    ```
    gcc main.c TDAS/list.c TDAS/hashmap.c TDAS/extra.c -o ActiFit.exe
    ```

5. Para ejecutar el programa, usa:

    ```
    ./ActiFit.exe
    ```

## Estructura del código

- `main.c`       — Programa principal y lógica de la aplicación.
- `list.h` / `list.c` — Implementación de listas.
- `map.h` / `map.c` — Implementación de mapas.
- `extra.h` / `extra.c` — Funciones auxiliares y menú.
- `README.md`    — Este archivo.

## Ejemplo de uso

1. **Registrar datos personales**  
   El programa solicita nombre, género, peso y altura, y calcula el IMC.

2. **Gestionar equipamiento**  
   Permite agregar o eliminar equipamiento disponible.

3. **Generar rutina**  
   El usuario elige el tipo de ejercicio preferido y se genera una rutina semanal personalizada.

4. **Mostrar rutina**  
   Visualiza los ejercicios asignados para cada día de la semana.

5. **Modificar rutina**  
   Permite reemplazar ejercicios por otros compatibles según preferencia y equipamiento.

6. **Ver resumen semanal**  
   Muestra la cantidad total de ejercicios realizados, tiempo total y calorías quemadas en la semana.

## Contribuciones

### Ignacio Muñoz
- Implementación de la lógica principal de rutinas y gestión de equipamiento.
- Desarrollo de la función de cálculo de IMC y sugerencia de ejercicios.
- Corrección de bugs en la generación y modificación de rutinas.
- Mejoras en la interfaz de usuario y validaciones.

### Diego Mena
- Implementación del sistema de carga de ejercicios desde archivo CSV.
- Desarrollo y mantenimiento de las estructuras de datos (listas y mapas).
- Optimización del manejo de archivos y memoria.
- Apoyo en la documentación y pruebas del sistema.

## Aspectos a mejorar

1. Crear una interfaz de usuario mas llamativa para la vista.
2. Añadir validaciones adicionales para entradas del usuario.
3. Permitir agregar nuevos ejercicios desde la aplicación.
4. Poder guardar información de semanas anteriores para que el usuario pueda ver progreso.

---

¡Gracias por usar ActiFit!