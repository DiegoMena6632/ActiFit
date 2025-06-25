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

int usuario_tiene_equipamiento(List* Equipamiento_Usuario, List* Equipamiento_Necesario) {
    // Si el ejercicio no requiere equipamiento, siempre puede hacerlo
    if (list_first(Equipamiento_Necesario) == NULL) return 1;

    int tiene_todo = 1;
    for (char* equip = list_first(Equipamiento_Necesario); equip != NULL; equip = list_next(Equipamiento_Necesario)) {
        int encontrado = 0;
        for (char* user_equip = list_first(Equipamiento_Usuario); user_equip != NULL; user_equip = list_next(Equipamiento_Usuario)) {
            if (strcmp(equip, "Ninguno") == 0 || strcmp(equip, user_equip) == 0) {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            tiene_todo = 0;
            break;
        }
    }
    return tiene_todo;
}

void GenerarRutina(Usuario* usuario, Map* Ejercicios_PorTipo, List* Equipamiento_Usuario) {
    char preferencia[32];
    printf("¿Qué tipo de ejercicios prefieres para tu rutina? (Cardio/Fuerza/Core): ");
    scanf("%s", preferencia);

    int ejercicios_diarios;
    if (usuario->IMC < 18.5)
        ejercicios_diarios = 2;
    else if (usuario->IMC < 25)
        ejercicios_diarios = 3;
    else if (usuario->IMC < 30)
        ejercicios_diarios = 4;
    else
        ejercicios_diarios = 5;

    const char* dias_semana[] = {"Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado", "Domingo"};
    usuario->Rutina_Usuario = map_create(is_equal_str);

    for (int i = 0; i < 7; i++) {
        RutinaDia* dia = malloc(sizeof(RutinaDia));
        strcpy(dia->dia, dias_semana[i]);
        dia->ejercicios = list_create();

        int count = 0;
        // 1. Prioriza ejercicios del tipo preferido
        MapPair* pair = map_search(Ejercicios_PorTipo, preferencia);
        if (pair) {
            for (Ejercicio* ejercicio = list_first((List*)pair->value); ejercicio != NULL && count < ejercicios_diarios; ejercicio = list_next((List*)pair->value)) {
                if (usuario_tiene_equipamiento(Equipamiento_Usuario, ejercicio->Equipamiento_Necesario)) {
                    list_pushBack(dia->ejercicios, ejercicio);
                    count++;
                }
            }
        }
        // 2. Si faltan ejercicios, busca en otros tipos
        if (count < ejercicios_diarios) {
            for (MapPair* p = map_first(Ejercicios_PorTipo); p != NULL && count < ejercicios_diarios; p = map_next(Ejercicios_PorTipo)) {
                if (strcmp((char*)p->key, preferencia) == 0) continue;
                for (Ejercicio* ejercicio = list_first((List*)p->value); ejercicio != NULL && count < ejercicios_diarios; ejercicio = list_next((List*)p->value)) {
                    if (usuario_tiene_equipamiento(Equipamiento_Usuario, ejercicio->Equipamiento_Necesario)) {
                        list_pushBack(dia->ejercicios, ejercicio);
                        count++;
                    }
                }
            }
        }
        map_insert(usuario->Rutina_Usuario, dia->dia, dia);
    }
    puts("¡Rutina semanal generada exitosamente!");
}

void MostrarRutina(Usuario* usuario) {
    if (usuario->Rutina_Usuario == NULL) {
        puts("No tienes una rutina generada aún.");
        return;
    }

    puts("===== Rutina semanal =====");
    const char* dias_semana[] = {"Lunes", "Martes", "Miércoles", "Jueves", "Viernes", "Sábado", "Domingo"};
    for (int i = 0; i < 7; i++) {
        MapPair* pair = map_search(usuario->Rutina_Usuario, (void*)dias_semana[i]);
        if (!pair || !pair->value) continue;
        RutinaDia* dia = (RutinaDia*)pair->value;
        printf("\n%s:\n", dias_semana[i]);
        int num = 1;
        for (Ejercicio* ejercicio = list_first(dia->ejercicios); ejercicio != NULL; ejercicio = list_next(dia->ejercicios)) {
            printf("  %d. %s (%d min)\n     %s\n", num++, ejercicio->nombre, ejercicio->duracion, ejercicio->descripcion);
        }
        if (num == 1) printf("  (Sin ejercicios asignados)\n");
    }
    puts("==========================");
}

void gestionarEquipamiento(Usuario* usuario, List* Equipamiento_Usuario) {
    if (Equipamiento_Usuario == NULL) {
        puts("Error: La lista de equipamiento no está inicializada.");
        return;
    }

    int opcion;
    do {
        puts("Seleccione una opción:");
        puts("1. Agregar equipamiento");
        puts("2. Eliminar equipamiento");
        puts("3. Volver al menú principal");
        printf("Ingrese su opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: {
                // Agregar equipamiento
                puts("Seleccione el equipamiento que desea agregar:");
                puts("1. Mancuernas");
                puts("2. Banda elástica");
                puts("3. Banco");
                int opcion_equipamiento;
                printf("Ingrese su opción: ");
                scanf("%d", &opcion_equipamiento);

                char* equipamiento = NULL;
                switch (opcion_equipamiento) {
                    case 1:
                        equipamiento = "Mancuernas";
                        break;
                    case 2:
                        equipamiento = "Banda elástica";
                        break;
                    case 3:
                        equipamiento = "Banco";
                        break;
                    default:
                        puts("Opción no válida.");
                        continue;
                }

                // Verificar si ya posee el equipamiento
                int ya_existe = 0;
                for (char* equip = list_first(Equipamiento_Usuario); equip != NULL; equip = list_next(Equipamiento_Usuario)) {
                    if (strcmp(equip, equipamiento) == 0) {
                        ya_existe = 1;
                        break;
                    }
                }

                if (!ya_existe) {
                    list_pushBack(Equipamiento_Usuario, strdup(equipamiento));
                    printf("Equipamiento '%s' agregado correctamente.\n", equipamiento);
                } else {
                    printf("Ya posee el equipamiento '%s'.\n", equipamiento);
                }
                break;
            }
            case 2: {
                // Eliminar equipamiento
                if (list_size(Equipamiento_Usuario) == 0) {
                    puts("No tiene equipamiento para eliminar.");
                    break;
                }

                puts("Seleccione el equipamiento que desea eliminar:");
                int index = 1;
                for (char* equip = list_first(Equipamiento_Usuario); equip != NULL; equip = list_next(Equipamiento_Usuario)) {
                    printf("%d. %s\n", index++, equip);
                }

                int opcion_eliminar;
                printf("Ingrese el número del equipamiento a eliminar: ");
                scanf("%d", &opcion_eliminar);

                if (opcion_eliminar < 1 || opcion_eliminar > list_size(Equipamiento_Usuario)) {
                    puts("Opción no válida.");
                    break;
                }

                // Eliminar el equipamiento seleccionado
                int current_index = 1;
                for (char* equip = list_first(Equipamiento_Usuario); equip != NULL; equip = list_next(Equipamiento_Usuario)) {
                    if (current_index == opcion_eliminar) {
                        printf("Equipamiento '%s' eliminado correctamente.\n", equip);
                        free(list_popCurrent(Equipamiento_Usuario)); // Eliminar y liberar memoria
                        break;
                    }
                    current_index++;
                }
                break;
            }
            case 3:
                puts("Volviendo al menú principal...");
                break;
            default:
                puts("Opción no válida. Intente nuevamente.");
        }
    } while (opcion != 3);
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
    puts("5. Gestionar equipamiento");
    puts("6. Salir");
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
    List *Equipamiento_Usuario = list_create(); // Lista para almacenar el equipamiento del usuario

    LeerEjercicios(Ejercicios_PorEquipamiento, Ejercicios_PorTipo, Equipamiento_Total); // Carga los ejercicios desde el archivo CSV

    puts("=============================================");
    puts("Bienvenido a ActiFit, el programa de entrenamiento personalizado.");
    puts("=============================================");
    puts("Por favor, ingresa tus datos personales para comenzar.");

    LeerDatosUsuario(&usuario);
    puts("Datos ingresados correctamente.");
    puts("=============================================");

    int opcion;
    do {
        mostrarmenu();
        printf("Seleccione una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                GenerarRutina(&usuario, Ejercicios_PorTipo, Equipamiento_Usuario); // Generar rutina de entrenamiento
                puts("Rutina generada. Puedes ver o modificar tu rutina en las opciones.");

                break;
            case 2:
                MostrarRutina(&usuario); // Mostrar la rutina generada
                break;
            case 3:
                // ModificarRutina(&usuario, Ejercicios_PorEquipamiento, Ejercicios_PorTipo); // Esta funcion deberia permitir modificar la rutina del usuario
                break;
            case 4:
                // VerResumenRutina(&usuario); // Esta funcion deberia mostrar un resumen de la rutina semanal del usuario
                break;
            case 5:
                gestionarEquipamiento(&usuario, Equipamiento_Usuario); // Gestionar el equipamiento del usuario
                puts("Equipamiento del usuario:");
                for (char *equip = list_first(Equipamiento_Usuario); equip != NULL; equip = list_next(Equipamiento_Usuario)) {
                    printf("- %s\n", equip);
                }
                break;
            case 6:
                puts("Saliendo de la aplicacion. ¡Hasta luego!");
                break;
            default:
                puts("Opción no valida. Por favor, intente de nuevo.");
        }
    } while (opcion != 6);
    
    return 0;
}