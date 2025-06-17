//====================================================================
// Programa ActiFit
//====================================================================


//Incluimos las librerías necesarias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "TDAS/list.h"
#include "TDAS/map.h"
#include "TDAS/extra.h"

//=====================================================================
// Definicion estructuras
//=====================================================================
typedef struct {
    char username[50];
    char genero[10];
    int peso;
    int altura;
    int IMC;
    List* Equipamiento_Disponible;
    Map* Rutina_Usuario;
} Usuario;

typedef struct {
    char nombre[50];
    char tipo[20]; // Ejemplo: "Cardio", "Fuerza", etc.
    int duracion; // Duración en minutos
    char descripcion[200];
    int calorias;
    List* Equipamiento_Necesario; // Lista de equipamiento necesario para el ejercicio
} Ejercicio;

typedef struct {
    char dia[10]; // Ejemplo: "Lunes", "Martes", etc.
    List* ejercicios; // Lista de ejercicios que componen la rutina
} RutinaDia;

typedef struct {
    List* dias; // Lista de RutinaDia que componen la rutina semanal
} RutinaSemanal;

//=====================================================================
// Definicion de funciones
//=====================================================================

//---------------------------------------------------------------------
// Esta Funcion es para que el mapa pueda comparar las claves de tipo cadena
int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}
//---------------------------------------------------------------------

bool not_in(char *equip, List* Equipamiento_Total)
{
    for(char *item = list_first(Equipamiento_Total); item != NULL; item = list_next(Equipamiento_Total)) {
        if (strcmp(item, equip) == 0) {
            return false; // El equipamiento ya está en la lista
        }
    }
    return true; // El equipamiento no está en la lista
}

//----------------------------------------------------------------------
// Esta Funcion es para leer el archivo CSV y cargar los ejercicios en
// los mapas Ejercicios_PorEquipamiento y Ejercicios_PorTipo.
// Se espera que el archivo CSV tenga el formato:
// Nombre,Tipo,Equipamiento_Necesario,Duracion,Calorias,Descripcion
// Equipamiento_Necesario es una lista separada por punto y coma (;)
//----------------------------------------------------------------------

void LeerEjercicios(Map* Ejercicios_PorEquipamiento, Map* Ejercicios_PorTipo, List* Equipamiento_Total) {
    FILE *archivo = fopen("ejercicios_formato_mostrar.csv", "r"); // Abre el archivo CSV en modo lectura
    if (archivo == NULL) {
        perror("Error al abrir el archivo ejercicios_formato_mostrar.csv");
        return;
    }

    char **campos; // Array para almacenar los campos de cada línea del CSV
    campos = leer_linea_csv(archivo, ','); // Lee la primera línea del CSV (encabezados)

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) { // Lee cada línea del CSV
        Ejercicio *ejercicio = (Ejercicio *)malloc(sizeof(Ejercicio)); // Reserva memoria para un nuevo ejercicio
        if (ejercicio == NULL) {
            perror("Error al asignar memoria para un ejercicio");
            continue;
        }

        // Asigna los valores a la estructura Ejercicio
        strncpy(ejercicio->nombre, campos[0], sizeof(ejercicio->nombre));
        strncpy(ejercicio->tipo, campos[1], sizeof(ejercicio->tipo));
        ejercicio->duracion = atoi(campos[3]);
        ejercicio->calorias = atoi(campos[4]);
        strncpy(ejercicio->descripcion, campos[5], sizeof(ejercicio->descripcion));

        // Divide los equipamientos necesarios por el delimitador ";"
        List *equipamiento_Necesario = split_string(campos[2], ";");
        ejercicio->Equipamiento_Necesario = list_create();

        for (char *item = list_first(equipamiento_Necesario); item != NULL; item = list_next(equipamiento_Necesario)) {
            char *equipamiento = strdup(item); // Duplica el string del equipamiento
            if (equipamiento == NULL) {
                perror("Error al asignar memoria para un equipamiento");
                continue;
            }
            list_pushBack(ejercicio->Equipamiento_Necesario, equipamiento); // Agrega el equipamiento a la lista del ejercicio
        }

        // Inserta el ejercicio en el mapa por tipo
        map_insert(Ejercicios_PorTipo, ejercicio->tipo, ejercicio);

        // Inserta el ejercicio en el mapa por equipamiento y actualiza Equipamiento_Total
        for (char *equip = list_first(ejercicio->Equipamiento_Necesario); equip != NULL; equip = list_next(ejercicio->Equipamiento_Necesario)) {
            map_insert(Ejercicios_PorEquipamiento, equip, ejercicio);
            if (not_in(equip, Equipamiento_Total)) {
                list_pushBack(Equipamiento_Total, strdup(equip)); // Agrega el equipamiento a la lista total
            }
        }

        list_clean(equipamiento_Necesario); // Limpia la lista temporal de equipamiento necesario
        free(equipamiento_Necesario); // Libera la memoria de la lista temporal
        free(campos); // Libera la memoria de los campos
    }

    fclose(archivo); // Cierra el archivo después de leer todas las líneas
    puts("Ejercicios cargados correctamente desde el archivo ejercicios_formato_mostrar.csv.");
}

//---------------------------------------------------------------------
// Esta función le pide al usuario sus datos personales y calcula su IMC.
// Se espera que el usuario ingrese su nombre de usuario, género, peso y altura.
//---------------------------------------------------------------------
void LeerDatosUsuario(Usuario* usuario) {
    printf("Ingrese su nombre de usuario: ");
    scanf("%s", usuario->username);

    int opcion_genero;
    do {
        puts("Ingrese su género:");
        puts("1. Masculino");
        puts("2. Femenino");
        printf("Seleccione una opción: ");
        scanf("%d", &opcion_genero);

        switch (opcion_genero) {
            case 1:
                strcpy(usuario->genero, "Masculino");
                break;
            case 2:
                strcpy(usuario->genero, "Femenino");
                break;
            default:
                puts("Opción no válida. Por favor, intente de nuevo.");
        }
    } while (opcion_genero < 1 || opcion_genero > 2);

    printf("Ingrese su peso en kg: ");
    scanf("%d", &usuario->peso);

    printf("Ingrese su altura en cm: ");
    scanf("%d", &usuario->altura);

    // Calcular IMC
    usuario->IMC = usuario->peso / ((usuario->altura / 100.0) * (usuario->altura / 100.0));

    printf("Tu IMC es: %d\n", usuario->IMC);
}

void indicarEquipamiento(Usuario* usuario, List* Equipamiento) 
{
    //Falta terminarla
    return;
}

//---------------------------------------------------------------------
// Esta funcion muestra un menú de opciones al usuario.
// El menú incluye opciones para generar una rutina de entrenamiento,
// mostrar la rutina, modificar la rutina y ver un resumen de la rutina semanal.
//---------------------------------------------------------------------
void mostrarmenu()
{
    puts("=============================================");
    puts("1. Generar rutina de entrenamiento");
    puts("2. Mostrar Rutina");
    puts("3. Modificar Rutina");
    puts("4. Ver resumen de rutina semanal");
    puts("5. Salir");
    puts("=============================================");
}

//=========================================================================
// Main
//==========================================================================

int main() {
    Usuario usuario;
    Map *Ejercicios_PorEquipamiento = map_create(is_equal_str);
    Map *Ejercicios_PorTipo = map_create(is_equal_str);
    List *Equipamiento_Total = list_create(); // Lista para almacenar el equipamiento total disponible

    LeerEjercicios(Ejercicios_PorEquipamiento, Ejercicios_PorTipo, Equipamiento_Total); // Carga los ejercicios desde el archivo CSV

    puts("=============================================");
    puts("Bienvenido a ActiFit, el programa de entrenamiento personalizado.");
    puts("=============================================");
    puts("Por favor, ingresa tus datos personales para comenzar.");

    LeerDatosUsuario(&usuario);
    indicarEquipamiento(&usuario, Equipamiento_Total);// Esta falta hacerla
    puts("Datos ingresados correctamente.");
    puts("=============================================");

    int opcion;
    do {
        mostrarmenu();
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                //GenerarRutina(); en eta misma deveria ir la opcion de elegir el tipo
                break;
            case 2:
                //VerRutina(&usuario); // Esta funcion deberia mostrar la rutina del usuario
                break;
            case 3:
                // ModificarRutina(&usuario, Ejercicios_PorEquipamiento, Ejercicios_PorTipo); // Esta funcion deberia permitir modificar la rutina del usuario
                break;
            case 4:
                // VerResumenRutina(&usuario); // Esta funcion deberia mostrar un resumen de la rutina semanal del usuario
                break;
            case 5:
                puts("Saliendo de la aplicacion. ¡Hasta luego!");
                break;
            default:
                puts("Opción no valida. Por favor, intente de nuevo.");
        }
    } while (opcion != 5);
    return 0;
}