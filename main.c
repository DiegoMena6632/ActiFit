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

int is_equal_str(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
}

void LeerCVS(Map* Ejercicios_PorEquipamiento, Map* Ejercicios_PorTipo) {
    
    FILE *archivo = fopen("Ejercicios_formato_mostrar.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }
    char **campos;
    campos = leer_linea_csv(archivo, ',');

    while((campos = leer_linea_csv(archivo, ',')) != NULL) 
    {
        Ejercicio *ejercicio = ( Ejercicio*)malloc(sizeof(Ejercicio));
        strncpy(ejercicio->nombre, campos[0], sizeof(ejercicio->nombre));
        strncpy(ejercicio->tipo, campos[1], sizeof(ejercicio->tipo));

        List *equipamiento_Necesario = split_string(campos[2], ";");
        for (char *item = list_first(equipamiento_Necesario); item != NULL; item = list_next(equipamiento_Necesario))// Itera sobre los items
        {
                char *equipamiento = (char*)malloc(sizeof(char));
                if (equipamiento == NULL) 
                {
                    perror("Error al asignar memoria para nuevo item");// Informa si no se puede reservar memoria
                    continue;
                }
                strncpy(equipamiento, item, sizeof(char));
                list_pushBack(ejercicio->Equipamiento_Necesario, equipamiento);
            }
        list_clean(equipamiento_Necesario);
        free(equipamiento_Necesario);

        ejercicio->duracion = atoi(campos[3]);
        ejercicio->calorias = atoi(campos[4]);
        strncpy(ejercicio->descripcion, campos[5], sizeof(ejercicio->descripcion));

        map_insert(Ejercicios_PorTipo, ejercicio->tipo, ejercicio);
        for(char *equip = list_first(ejercicio->Equipamiento_Necesario); equip != NULL; equip = list_next(ejercicio->Equipamiento_Necesario))
        {
            map_insert(Ejercicios_PorEquipamiento, equip, ejercicio);
        }
        free(campos);
    }
}
void LeerDatosUsuario(Usuario* usuario) {
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

int main() {
    Usuario usuario;
    Map *Ejercicios_PorEquipamiento = map_create(is_equal_str);
    Map *Ejercicios_PorTipo = map_create(is_equal_str);

    LeerCVS(Ejercicios_PorEquipamiento, Ejercicios_PorTipo);

    puts("=============================================");
    puts("Bienvenido a ActiFit, el programa de entrenamiento personalizado.");
    puts("=============================================");
    puts("Por favor, ingresa tus datos personales para comenzar.");

    //LeerDatosUsuario(&usuario);
    //indicarEquipamiento(&usuario, Ejercicios_PorEquipamiento);// Esta falta hacerla
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