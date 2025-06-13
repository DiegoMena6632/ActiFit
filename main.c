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
void LeerCVS(Map* Ejercicios_PorEquipamiento, Map* Ejercicios_PorTipo, List* Equipamiento_Total) {
    
    FILE *archivo = fopen("Ejercicios_formato_mostrar.csv", "r");// Abre el archivo CSV en modo lectura
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }
    char **campos;// Array para almacenar los campos de cada línea del CSV
    campos = leer_linea_csv(archivo, ',');// Lee la primera línea del CSV (encabezados)

    while((campos = leer_linea_csv(archivo, ',')) != NULL)//Lee cada linea del CVS
    {
        Ejercicio *ejercicio = ( Ejercicio*)malloc(sizeof(Ejercicio));// Reserva memoria para un nuevo ejercicio
        strncpy(ejercicio->nombre, campos[0], sizeof(ejercicio->nombre));// Asigna el nombre del ejercicio
        strncpy(ejercicio->tipo, campos[1], sizeof(ejercicio->tipo));// Asigna el tipo del ejercicio

        List *equipamiento_Necesario = split_string(campos[2], ";");// Divide los equipamientos necesarios por el delimitador ";"
        for (char *item = list_first(equipamiento_Necesario); item != NULL; item = list_next(equipamiento_Necesario))// Itera sobre los equipamientos necesarios
        {
            char *equipamiento = (char*)malloc(sizeof(char));// Reserva memoria para un nuevo equipamiento
            if (equipamiento == NULL) 
            {
                perror("Error al asignar memoria para nuevo item");// Informa si no se puede reservar memoria
                continue;
            }
            strncpy(equipamiento, item, sizeof(char));// Asigna el nombre del equipamiento
            list_pushBack(ejercicio->Equipamiento_Necesario, equipamiento);// Agrega el equipamiento a la lista de equipamiento necesario del ejercicio
        }
        list_clean(equipamiento_Necesario);// Limpia la lista de equipamiento necesario
        free(equipamiento_Necesario);// Libera la memoria de la lista de equipamiento necesario

        ejercicio->duracion = atoi(campos[3]);// Asigna la duración del ejercicio en minutos
        ejercicio->calorias = atoi(campos[4]);// Asigna las calorías quemadas por el ejercicio
        strncpy(ejercicio->descripcion, campos[5], sizeof(ejercicio->descripcion));// Asigna la descripción del ejercicio

        map_insert(Ejercicios_PorTipo, ejercicio->tipo, ejercicio);// Inserta el ejercicio en el mapa de ejercicios por tipo, con la clave siendo el tipo del ejercicio
        for(char *equip = list_first(ejercicio->Equipamiento_Necesario); equip != NULL; equip = list_next(ejercicio->Equipamiento_Necesario))// Itera sobre los equipamientos necesarios del ejercicio
        {
            map_insert(Ejercicios_PorEquipamiento, equip, ejercicio);// Inserta el ejercicio en el mapa de ejercicios por equipamiento, con la clave siendo el equipamiento necesario
            if(not_in(equip, Equipamiento_Total)) // Verifica si el equipamiento no está en la lista de equipamiento total
            {
                list_pushBack(Equipamiento_Total, equip);// Agrega el equipamiento a la lista de equipamiento total
            }
        }
        free(campos);
    }
    fclose(archivo);// Cierra el archivo después de leer todas las líneas
    puts("Ejercicios cargados correctamente desde el archivo CSV.");
}

//---------------------------------------------------------------------
// Esta función le pide al usuario sus datos personales y calcula su IMC.
// Se espera que el usuario ingrese su nombre de usuario, género, peso y altura.
//---------------------------------------------------------------------
void LeerDatosUsuario(Usuario* usuario) 
{
    printf("Ingrese su nombre de usuario: ");
    scanf("%s", usuario->username);
    printf("Ingrese su género (Masculino/Femenino): ");
    scanf("%s", usuario->genero);
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

    LeerCVS(Ejercicios_PorEquipamiento, Ejercicios_PorTipo, Equipamiento_Total); // Cargar ejercicios desde el archivo CSV

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
                puts("Saliendo del programa. ¡Hasta luego!");
                break;
            default:
                puts("Opción no válida. Por favor, intente de nuevo.");
        }
    } while (opcion != 5);
    return 0;
}