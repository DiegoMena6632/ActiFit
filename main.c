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
    char descripcion[500];
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

//---------------------------------------------------------------------
// Esta función verifica si un equipamiento ya está en la lista Equipamiento_Total.
// Devuelve true si el equipamiento no está en la lista, false si ya está.
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

void LeerEjercicios(Map* Ejercicios_PorEquipamiento, Map* Ejercicios_PorTipo, List* Equipamiento_Total)
{
    FILE *archivo = fopen("ejercicios_formato_mostrar.csv", "r"); // Abre el archivo CSV en modo lectura
    if (archivo == NULL) {
        perror("Error al abrir el archivo ejercicios_formato_mostrar.csv");
        return;
    }

    char **campos; // Array para almacenar los campos de cada línea del CSV
    campos = leer_linea_csv(archivo, ','); // Lee la primera línea del CSV (encabezados)

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) 
    { // Lee cada línea del CSV
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

        for (char *item = list_first(equipamiento_Necesario); item != NULL; item = list_next(equipamiento_Necesario)) 
        {
            char *equipamiento = strdup(item); // Duplica el string del equipamiento
            if (equipamiento == NULL)
            {
                perror("Error al asignar memoria para un equipamiento");
                continue;
            }
            list_pushBack(ejercicio->Equipamiento_Necesario, equipamiento); // Agrega el equipamiento a la lista del ejercicio
        }

        // Inserta el ejercicio en el mapa por tipo
        List* lista_tipo = NULL;
        MapPair* tipo_pair = map_search(Ejercicios_PorTipo, ejercicio->tipo);
        if (tipo_pair) 
        {
            lista_tipo = (List*)tipo_pair->value;
        } 
        else 
        {
            lista_tipo = list_create();
            map_insert(Ejercicios_PorTipo, ejercicio->tipo, lista_tipo);
        }   
        list_pushBack(lista_tipo, ejercicio);

        // Inserta el ejercicio en el mapa por equipamiento y actualiza Equipamiento_Total
        for (char *equip = list_first(ejercicio->Equipamiento_Necesario); equip != NULL; equip = list_next(ejercicio->Equipamiento_Necesario))
        {
            // Buscar si ya hay una lista de ejercicios para ese equipamiento
            List* lista_equip = NULL;
            MapPair* equip_pair = map_search(Ejercicios_PorEquipamiento, equip);
            if (equip_pair) 
            {
                lista_equip = (List*)equip_pair->value;
            } else 
            {
                lista_equip = list_create();
                map_insert(Ejercicios_PorEquipamiento, equip, lista_equip);
            }
            list_pushBack(lista_equip, ejercicio);

            if (not_in(equip, Equipamiento_Total)) 
            {
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
    scanf(" %[^\n]", usuario->username);

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

//---------------------------------------------------------------------
// Esta función verifica si el usuario tiene el equipamiento necesario para realizar un ejercicio.
// Devuelve 1 si tiene todo el equipamiento necesario, 0 si no lo tiene
//---------------------------------------------------------------------
int usuario_tiene_equipamiento(List* Equipamiento_Usuario, List* Equipamiento_Necesario) {
    // Si el ejercicio no requiere equipamiento, siempre puede hacerlo
    if (list_first(Equipamiento_Necesario) == NULL) return 1;

    int tiene_todo = 1;
    for (char* equip = list_first(Equipamiento_Necesario); equip != NULL; equip = list_next(Equipamiento_Necesario)) {
        int encontrado = 0;
        for (char* user_equip = list_first(Equipamiento_Usuario); user_equip != NULL; user_equip = list_next(Equipamiento_Usuario)) {
            if (strcmp(equip, "Ninguno") == 0 || strcmp(equip, user_equip) == 0)//Busca si el usuario tiene el eqipamiento necesario
            {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) // Si no se encontró el equipamiento necesario
        {
            tiene_todo = 0;
            break;
        }
    }
    return tiene_todo;
}

//---------------------------------------------------------------------
// Esta función verifica si un ejercicio ya está en la lista Ejercicios_asignados.
// Devuelve true si el ejercicio ya está en la lista, false si no lo está.
//---------------------------------------------------------------------
bool list_contains_Ejercicio(List* Ejercicios_asignados, Ejercicio* ejercicio) {
    for (Ejercicio* e = list_first(Ejercicios_asignados); e != NULL; e = list_next(Ejercicios_asignados)) {
        if (strcmp(e->nombre, ejercicio->nombre) == 0) {
            return true; // El ejercicio ya está en la lista
        }
    }
    return false; // El ejercicio no está en la lista
}

//---------------------------------------------------------------------
// Esta función genera una rutina semanal personalizada para el usuario.
// Toma en cuenta las preferencias del usuario, su IMC y el equipamiento disponible.
// Para generar la rutina, se priorizan los ejercicios del tipo preferido del usuario.
// Si no hay suficientes ejercicios del tipo preferido, se buscan en otros tipos.
//---------------------------------------------------------------------
void GenerarRutina(Usuario* usuario, Map* Ejercicios_PorTipo, List* Equipamiento_Usuario, List* Ejercicios_asignados) {
    char preferencia[32];

    printf("¿Que tipo de ejercicios prefieres para tu rutina? (Cardio/Fuerza/Core): ");
    puts("Recuerda que tienes que escribir exactamente el tipo de ejercicio preferido, como aparece en la pantalla.");
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

    const char* dias_semana[] = {"Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"};
    usuario->Rutina_Usuario = map_create(is_equal_str);

    for (int i = 0; i < 7; i++) {
        RutinaDia* Dia = malloc(sizeof(RutinaDia));
        strcpy(Dia->dia, dias_semana[i]);
        Dia->ejercicios = list_create();

        int count = 0;
        // 1. Prioriza ejercicios del tipo preferido
        MapPair* pair = map_search(Ejercicios_PorTipo, preferencia);
        if (pair) {
            for (Ejercicio* ejercicio = list_first((List*)pair->value); ejercicio != NULL && count < ejercicios_diarios; ejercicio = list_next((List*)pair->value)) {
                if (usuario_tiene_equipamiento(Equipamiento_Usuario, ejercicio->Equipamiento_Necesario)) 
                {
                    if (!list_contains_Ejercicio(Ejercicios_asignados, ejercicio))
                    {
                        list_pushBack(Dia->ejercicios, ejercicio);
                        list_pushBack(Ejercicios_asignados, ejercicio);
                        count++;
                    }
                }
            }
        }
        // 2. Si faltan ejercicios, busca en otros tipos
        if (count < ejercicios_diarios) {
            for (MapPair* p = map_first(Ejercicios_PorTipo); p != NULL && count < ejercicios_diarios; p = map_next(Ejercicios_PorTipo)) {
                if (strcmp((char*)p->key, preferencia) == 0) continue;
                for (Ejercicio* ejercicio = list_first((List*)p->value); ejercicio != NULL && count < ejercicios_diarios; ejercicio = list_next((List*)p->value)) {
                    if (usuario_tiene_equipamiento(Equipamiento_Usuario, ejercicio->Equipamiento_Necesario)) {
                        if (!list_contains_Ejercicio(Ejercicios_asignados, ejercicio))
                        {
                            list_pushBack(Dia->ejercicios, ejercicio);
                            list_pushBack(Ejercicios_asignados, ejercicio);
                            count++;
                        }
                    }
                }
            }
        }
        map_insert(usuario->Rutina_Usuario, Dia->dia, Dia);
    }
    puts("¡Rutina semanal generada exitosamente!");

}

//---------------------------------------------------------------------
// Esta función muestra la rutina semanal del usuario.
// Recorre los días de la semana y muestra los ejercicios asignados a cada día.
//---------------------------------------------------------------------
void MostrarRutina(Usuario* usuario) {
    if (usuario->Rutina_Usuario == NULL) {
        puts("No tienes una rutina generada aún.");
        return;
    }

    puts("===== Rutina semanal =====");
    const char* dias_semana[] = {"Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"};
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

//---------------------------------------------------------------------
// Esta función gestiona el equipamiento del usuario.
// Permite agregar o eliminar equipamiento de la lista del usuario.
// Si el usuario intenta agregar un equipamiento que ya posee, se le informa.
//---------------------------------------------------------------------
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
                puts("AVISO : NO ELIMINAR EL EQUIPAMIENTO 'Ninguno'!");
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
// Esta función permite al usuario modificar un ejercicio en su rutina semanal.
// Permite seleccionar un día de la semana y un ejercicio específico para reemplazarlo por otro compatible.
//---------------------------------------------------------------------
void ModificarRutina(Usuario* usuario, Map* Ejercicios_PorTipo, List* Equipamiento_Usuario) {
    if (usuario->Rutina_Usuario == NULL) {
        puts("No tienes una rutina generada aún.");
        return;
    }

    const char* dias_semana[] = {"Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"};
    puts("¿De qué día deseas modificar un ejercicio?");
    for (int i = 0; i < 7; i++) {
        printf("%d. %s\n", i + 1, dias_semana[i]);
    }
    int dia_opcion;
    printf("Seleccione el número del día: ");
    scanf("%d", &dia_opcion);
    if (dia_opcion < 1 || dia_opcion > 7) {
        puts("Opción de día no válida.");
        return;
    }

    MapPair* pair = map_search(usuario->Rutina_Usuario, (void*)dias_semana[dia_opcion - 1]);
    if (!pair || !pair->value) {
        puts("No hay ejercicios asignados para ese día.");
        return;
    }
    RutinaDia* dia = (RutinaDia*)pair->value;

    int total_ejercicios = list_size(dia->ejercicios);
    if (total_ejercicios == 0) {
        puts("No hay ejercicios para modificar en este día.");
        return;
    }

    puts("Ejercicios asignados:");
    int idx = 1;
    for (Ejercicio* ejercicio = list_first(dia->ejercicios); ejercicio != NULL; ejercicio = list_next(dia->ejercicios)) {
        printf("%d. %s (%s)\n", idx++, ejercicio->nombre, ejercicio->tipo);
    }
    int ejercicio_opcion;
    printf("Seleccione el número del ejercicio a reemplazar: ");
    scanf("%d", &ejercicio_opcion);
    if (ejercicio_opcion < 1 || ejercicio_opcion > total_ejercicios) {
        puts("Opción de ejercicio no válida.");
        return;
    }

    // Obtener preferencia del usuario
    char preferencia[32];
    printf("¿Qué tipo de ejercicio prefieres para el reemplazo? (Cardio/Fuerza/Core): ");
    scanf("%s", preferencia);

    // Mostrar ejercicios compatibles
    MapPair* tipo_pair = map_search(Ejercicios_PorTipo, preferencia);
    if (!tipo_pair) {
        puts("No hay ejercicios de ese tipo.");
        return;
    }

    List* lista_tipo = (List*)tipo_pair->value;
    int compatibles = 0, comp_idx = 1;
    Ejercicio* compatibles_arr[100]; // Máximo 100 compatibles para mostrar

    puts("Ejercicios compatibles disponibles:");
    for (Ejercicio* ejercicio = list_first(lista_tipo); ejercicio != NULL; ejercicio = list_next(lista_tipo)) {
        // Verifica equipamiento
        if (usuario_tiene_equipamiento(Equipamiento_Usuario, ejercicio->Equipamiento_Necesario)) {
            // Evita duplicados en el mismo día
            int ya_en_dia = 0;
            for (Ejercicio* e = list_first(dia->ejercicios); e != NULL; e = list_next(dia->ejercicios)) {
                if (strcmp(e->nombre, ejercicio->nombre) == 0) {
                    ya_en_dia = 1;
                    break;
                }
            }
            if (!ya_en_dia) {
                printf("%d. %s (%d min)\n", comp_idx, ejercicio->nombre, ejercicio->duracion);
                compatibles_arr[comp_idx - 1] = ejercicio;
                compatibles++;
                comp_idx++;
            }
        }
    }
    if (compatibles == 0) {
        puts("No hay ejercicios compatibles disponibles para reemplazar.");
        return;
    }

    int nuevo_opcion;
    printf("Seleccione el número del nuevo ejercicio: ");
    scanf("%d", &nuevo_opcion);
    if (nuevo_opcion < 1 || nuevo_opcion > compatibles) {
        puts("Opción no válida.");
        return;
    }

    // Reemplazar el ejercicio
    int curr_idx = 1;
    for (Ejercicio* ejercicio = list_first(dia->ejercicios); ejercicio != NULL; ejercicio = list_next(dia->ejercicios)) {
        if (curr_idx == ejercicio_opcion) {
            list_popCurrent(dia->ejercicios); // Elimina el ejercicio actual
            list_pushBack(dia->ejercicios, compatibles_arr[nuevo_opcion - 1]); // Agrega el nuevo ejercicio
            puts("Ejercicio reemplazado exitosamente.");
            return;
        }
        curr_idx++;
    }
}

//---------------------------------------------------------------------
// Esta función muestra un resumen semanal de la rutina del usuario.
//---------------------------------------------------------------------
void MostrarResumenSemanal(Usuario* usuario) {
    if (usuario->Rutina_Usuario == NULL) {
        puts("No tienes una rutina generada aún.");
        return;
    }

    const char* dias_semana[] = {"Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"};
    int total_ejercicios = 0;
    int total_tiempo = 0;
    int total_calorias = 0;

    for (int i = 0; i < 7; i++) {
        MapPair* pair = map_search(usuario->Rutina_Usuario, (void*)dias_semana[i]);
        if (!pair || !pair->value) continue;
        RutinaDia* dia = (RutinaDia*)pair->value;
        for (Ejercicio* ejercicio = list_first(dia->ejercicios); ejercicio != NULL; ejercicio = list_next(dia->ejercicios)) {
            total_ejercicios++;
            total_tiempo += ejercicio->duracion;
            total_calorias += ejercicio->calorias;
        }
    }

    puts("===== Resumen semanal de tu rutina =====");
    printf("Total de ejercicios realizados: %d\n", total_ejercicios);
    printf("Tiempo total realizando ejercicios: %d minutos\n", total_tiempo);
    printf("Calorías quemadas aproximadas: %d kcal\n", total_calorias);
    puts("========================================");
}

//---------------------------------------------------------------------
// Esta funcion muestra un menú de opciones al usuario.
// El menú incluye opciones para generar una rutina de entrenamiento,
// mostrar la rutina, modificar la rutina y ver un resumen de la rutina semanal.
//---------------------------------------------------------------------
void mostrarmenu()
{
    puts("=============================================");
    puts("1. Gestionar equipamiento");
    puts("2. Generar rutina de entrenamiento");
    puts("3. Mostrar Rutina");
    puts("4. Modificar Rutina");
    puts("5. Ver resumen de rutina semanal");
    puts("6. Salir");
    puts("=============================================");
}

//---------------------------------------------------------------------
// Esta función libera toda la memoria utilizada por el programa al finalizar.
//---------------------------------------------------------------------
void LiberarMemoria(Usuario* usuario, Map* Ejercicios_PorTipo, Map* Ejercicios_PorEquipamiento,List* Equipamiento_Total, List* Equipamiento_Usuario, List* Ejercicios_asignados)
{
    // --------------------------------------------------------------
    // 1. Liberar los ejercicios en Ejercicios_PorTipo (una sola vez)
    // --------------------------------------------------------------
    // Cada ejercicio puede estar en múltiples listas, pero solo debe liberarse una vez.
    MapPair* tipo_pair = map_first(Ejercicios_PorTipo);
    while (tipo_pair != NULL) {
        List* lista = tipo_pair->value;

        Ejercicio* e = list_first(lista);
        while (e != NULL) {
            // Liberar todos los strings del equipamiento necesario
            for (char* equip = list_first(e->Equipamiento_Necesario); equip != NULL; equip = list_next(e->Equipamiento_Necesario)) {
                free(equip); // strdup de cada item
            }

            // Limpiar y liberar la lista de equipamiento del ejercicio
            list_clean(e->Equipamiento_Necesario);
            free(e->Equipamiento_Necesario);

            // Finalmente, liberar el ejercicio completo
            free(e);
            e = list_next(lista);
        }

        // Limpiar y liberar la lista del tipo
        list_clean(lista);
        free(lista);

        tipo_pair = map_next(Ejercicios_PorTipo);
    }

    // Limpiar y liberar el mapa completo de ejercicios por tipo
    map_clean(Ejercicios_PorTipo);
    free(Ejercicios_PorTipo);

    // ---------------------------------------------------------------------
    // 2. Liberar las listas en Ejercicios_PorEquipamiento (sin liberar ejercicios)
    // ---------------------------------------------------------------------
    // Ya que los ejercicios fueron liberados en el paso anterior, aquí solo liberamos las listas.
    MapPair* equip_pair = map_first(Ejercicios_PorEquipamiento);
    while (equip_pair != NULL) {
        List* lista = equip_pair->value;
        list_clean(lista);
        free(lista);
        equip_pair = map_next(Ejercicios_PorEquipamiento);
    }

    map_clean(Ejercicios_PorEquipamiento);
    free(Ejercicios_PorEquipamiento);

    // --------------------------------------------------------------
    // 3. Liberar Equipamiento_Total (strings creados con strdup)
    // --------------------------------------------------------------
    for (char* equip = list_first(Equipamiento_Total); equip != NULL; equip = list_next(Equipamiento_Total)) {
        free(equip);
    }
    list_clean(Equipamiento_Total);
    free(Equipamiento_Total);

    // --------------------------------------------------------------
    // 4. Liberar Equipamiento_Usuario (también con strdup)
    // --------------------------------------------------------------
    for (char* equip = list_first(Equipamiento_Usuario); equip != NULL; equip = list_next(Equipamiento_Usuario)) {
        free(equip);
    }
    list_clean(Equipamiento_Usuario);
    free(Equipamiento_Usuario);

    // --------------------------------------------------------------
    // 5. Liberar lista de Ejercicios_asignados (sin liberar los ejercicios)
    // --------------------------------------------------------------
    // Solo liberamos la lista porque los ejercicios ya fueron liberados anteriormente.
    list_clean(Ejercicios_asignados);
    free(Ejercicios_asignados);

    // --------------------------------------------------------------
    // 6. Liberar la rutina generada por el usuario (Rutina_Usuario)
    // --------------------------------------------------------------
    if (usuario->Rutina_Usuario != NULL) {
        MapPair* rutina_pair = map_first(usuario->Rutina_Usuario);
        while (rutina_pair != NULL) {
            RutinaDia* dia = rutina_pair->value;

            // Limpiar la lista de ejercicios del día (sin liberar los ejercicios en sí)
            list_clean(dia->ejercicios);
            free(dia->ejercicios);

            // Liberar la estructura del día
            free(dia);

            rutina_pair = map_next(usuario->Rutina_Usuario);
        }

        // Limpiar y liberar el mapa de rutina
        map_clean(usuario->Rutina_Usuario);
        free(usuario->Rutina_Usuario);
    }
}

//=========================================================================
// Main
//==========================================================================

int main() {
    int bandera = 0; // Bandera para controlar el flujo del programa
    Usuario usuario;
    Map *Ejercicios_PorEquipamiento = map_create(is_equal_str);
    Map *Ejercicios_PorTipo = map_create(is_equal_str);
    List *Ejercicios_asignados = list_create(); // Lista para almacenar los ejercicios asignados a la rutina para evitar duplicados en la semana
    List *Equipamiento_Total = list_create(); // Lista para almacenar el equipamiento total disponible
    List *Equipamiento_Usuario = list_create(); // Lista para almacenar el equipamiento del usuario
    list_pushBack(Equipamiento_Usuario, "Ninguno"); // Agrega un equipamiento por defecto

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
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                gestionarEquipamiento(&usuario, Equipamiento_Usuario); // Gestionar el equipamiento del usuario
                puts("Equipamiento del usuario:");
                for (char *equip = list_first(Equipamiento_Usuario); equip != NULL; equip = list_next(Equipamiento_Usuario)) {
                    printf("- %s\n", equip);
                }
                break;
            case 2:
                if (bandera == 0) 
                {
                    puts("¡Genial! Vamos a generar tu rutina de entrenamiento.");
                    list_clean(Ejercicios_asignados); // Limpia la lista de ejercicios asignados
                    GenerarRutina(&usuario, Ejercicios_PorTipo, Equipamiento_Usuario, Ejercicios_asignados); // Generar rutina de entrenamiento
                    puts("Rutina generada. Puedes ver o modificar tu rutina en las opciones.");
                    bandera = 1; // Cambia la bandera para indicar que ya se ha generado una rutina
                    break;
                } 
                else 
                {
                    puts("Ya has generado una rutina. Si deseas cambiarla, por favor, modifícala en las opciones.");
                    break;
                }
            case 3:
                MostrarRutina(&usuario); // Mostrar la rutina generada
                break;
            case 4:
                puts("Vamos a modificar tu rutina de entrenamiento.");
                ModificarRutina(&usuario, Ejercicios_PorTipo, Equipamiento_Usuario); // Modificar la rutina generada
                puts("Rutina modificada. Puedes volver a mostrarla o modificarla nuevamente.");
                break;
            case 5:
                MostrarResumenSemanal(&usuario); // Mostrar un resumen de la rutina semanal
                puts("Este resumen te ayudará a ver tu progreso y ajustar tu rutina si es necesario.");
                break;
            case 6:
                puts("Saliendo de la aplicacion. ¡Hasta luego!");
                break;
            default:
                puts("Opción no valida. Por favor, intente de nuevo.");
        }
    } while (opcion != 6);

    // Liberar memoria
    LiberarMemoria(&usuario, Ejercicios_PorTipo, Ejercicios_PorEquipamiento, Equipamiento_Total, Equipamiento_Usuario, Ejercicios_asignados);
    return 0;
}