#include <stdio.h>
#include <string.h>
#include <unistd.h> // Para sbrk
#include "mm_malloc.h"

// Colores ANSI para la consola
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"

// Macro para aserciones simples
#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            printf(COLOR_RED "[FAIL] %s\n" COLOR_RESET, message); \
            printf("       En archivo: %s, Linea: %d\n", __FILE__, __LINE__); \
            return 0; \
        } \
    } while (0)

#define PASS(msg) printf(COLOR_GREEN "[PASS] %s\n" COLOR_RESET, msg)
#define INFO(msg) printf(COLOR_BLUE "[INFO] %s\n" COLOR_RESET, msg)

// --------------------------------------------------------------------------
// TEST 1: Funcionalidad Básica (Malloc y Free)
// --------------------------------------------------------------------------
int test_basic_allocation() {
    printf(COLOR_YELLOW "\n=== TEST 1: Asignación Básica ===\n" COLOR_RESET);
    
    int *ptr1 = (int*)my_malloc(sizeof(int));
    ASSERT(ptr1 != NULL, "my_malloc devolvió NULL en la primera llamada");
    
    *ptr1 = 42;
    ASSERT(*ptr1 == 42, "No se pudo escribir en la memoria asignada");
    
    printf("    Puntero 1 asignado en: %p\n", (void*)ptr1);
    
    my_free(ptr1);
    PASS("Malloc y Free básicos ejecutados sin Segfaults");
    return 1;
}

// --------------------------------------------------------------------------
// TEST 2: Reutilización de Bloques (First-Fit / Best-Fit)
// CRÍTICO: Si este test falla, pierden el 30% de la nota.
// --------------------------------------------------------------------------
int test_reuse() {
    printf(COLOR_YELLOW "\n=== TEST 2: Reutilización de Memoria ===\n" COLOR_RESET);
    
   // 1. Crear un bloque "Guardian" para evitar coalescing con el Test 1
    void *guard = my_malloc(10); 
    
    INFO("Asignando bloque Objetivo A (128 bytes)...");
    void *ptr1 = my_malloc(128);
    ASSERT(ptr1 != NULL, "Fallo malloc inicial");
    
    // 2. Crear otro "Guardian" para evitar coalescing hacia adelante (futuro)
    void *guard2 = my_malloc(10);

    INFO("Liberando bloque Objetivo A...");
    my_free(ptr1); // Ahora ptr1 queda libre entre dos bloques ocupados (guards)
    
    INFO("Solicitando bloque B (128 bytes)...");
    void *ptr2 = my_malloc(128);
    ASSERT(ptr2 != NULL, "Fallo segundo malloc");
    
    printf("    Ptr1 (Original): %p\n    Ptr2 (Nuevo):    %p\n", ptr1, ptr2);
    
    // Si ptr1 == ptr2, significa que el allocator reusó el espacio.
    // Si ptr1 != ptr2, significa que pidió más RAM al OS innecesariamente.
    ASSERT(ptr1 == ptr2, "El allocator NO reutilizó el bloque libre. Se desperdicia memoria.");
    
    // Limpieza
    my_free(ptr2);
    my_free(guard);
    my_free(guard2);
    
    PASS("El bloque fue reutilizado correctamente en su lugar exacto");    
    return 1;
}

// --------------------------------------------------------------------------
// TEST 3: Calloc (Inicialización a Cero)
// --------------------------------------------------------------------------
int test_calloc() {
    printf(COLOR_YELLOW "\n=== TEST 3: Calloc (Zero Init) ===\n" COLOR_RESET);
    
    size_t num_elements = 10;
    int *array = (int*)my_calloc(num_elements, sizeof(int));
    
    ASSERT(array != NULL, "my_calloc devolvió NULL");
    
    // Verificar que todo sea 0
    for (size_t i = 0; i < num_elements; i++) {
        if (array[i] != 0) {
            printf(COLOR_RED "[FAIL] Posición %zu no es cero (valor: %d)\n" COLOR_RESET, i, array[i]);
            return 0;
        }
    }
    
    my_free(array);
    PASS("Calloc inicializó la memoria en 0 correctamente");
    return 1;
}

// --------------------------------------------------------------------------
// TEST 4: Realloc (Redimensionamiento)
// --------------------------------------------------------------------------
int test_realloc() {
    printf(COLOR_YELLOW "\n=== TEST 4: Realloc ===\n" COLOR_RESET);
    
    // 1. Asignar pequeño
    char *ptr = (char*)my_malloc(10);
    ASSERT(ptr != NULL, "Malloc inicial falló");
    strcpy(ptr, "Hola");
    
    // 2. Agrandar
    INFO("Expandiendo bloque...");
    ptr = (char*)my_realloc(ptr, 20);
    ASSERT(ptr != NULL, "Realloc devolvió NULL");
    ASSERT(strcmp(ptr, "Hola") == 0, "Realloc corrompió los datos originales");
    
    // 3. Concatenar para probar escritura en nueva zona
    strcat(ptr, " Mundo");
    ASSERT(strcmp(ptr, "Hola Mundo") == 0, "Error escribiendo en área expandida");
    
    my_free(ptr);
    PASS("Realloc conservó los datos y expandió la memoria");
    return 1;
}

// --------------------------------------------------------------------------
// TEST 5: Stress Test (Fragmentación)
// --------------------------------------------------------------------------
int test_stress() {
    printf(COLOR_YELLOW "\n=== TEST 5: Stress Test (Fragmentación) ===\n" COLOR_RESET);
    
    #define NUM_ALLOCS 50
    void *ptrs[NUM_ALLOCS];
    
    INFO("Realizando 50 asignaciones aleatorias...");
    for(int i=0; i<NUM_ALLOCS; i++) {
        ptrs[i] = my_malloc((i + 1) * 10); // Tamaños variables
    }
    
    INFO("Liberando pares...");
    for(int i=0; i<NUM_ALLOCS; i+=2) {
        my_free(ptrs[i]);
    }
    
    INFO("Reasignando pares (deben caber en los huecos)...");
    for(int i=0; i<NUM_ALLOCS; i+=2) {
        ptrs[i] = my_malloc((i + 1) * 10);
        ASSERT(ptrs[i] != NULL, "Fallo reasignación en stress test");
    }
    
    // Limpieza final
    for(int i=0; i<NUM_ALLOCS; i++) {
        my_free(ptrs[i]);
    }
    
    PASS("Stress test superado sin fugas masivas ni segfaults");
    return 1;
}

// --------------------------------------------------------------------------
// TEST 6: Coalescing Obligatorio (Fusión de Bloques)
// REQUISITO: Si liberamos dos bloques adyacentes, deben unirse en uno grande.
// --------------------------------------------------------------------------
int test_coalescing() {
    printf(COLOR_YELLOW "\n=== TEST 6: Coalescing (Fusión Obligatoria) ===\n" COLOR_RESET);

    // 1. Asignamos 3 bloques contiguos
    INFO("Asignando bloques A, B y C (64 bytes c/u)...");
    void *ptrA = my_malloc(64);
    void *ptrB = my_malloc(64);
    void *ptrC = my_malloc(64);

    ASSERT(ptrA != NULL && ptrB != NULL && ptrC != NULL, "Fallo en malloc inicial");

    // 2. Liberamos A y B. 
    // Si hay coalescing, ahora debería haber un hueco de ~128 bytes + header en ptrA.
    // Si NO hay coalescing, hay dos huecos separados de 64 bytes.
    INFO("Liberando bloque A y bloque B...");
    my_free(ptrA);
    my_free(ptrB);

    // 3. Pedimos un bloque de 100 bytes.
    // - Con Coalescing: Cabe perfecto en el espacio fusionado de A+B.
    // - Sin Coalescing: No cabe en A (64) ni en B (64). El allocator pedirá más RAM (sbrk).
    INFO("Solicitando bloque D (100 bytes)...");
    void *ptrD = my_malloc(100);
    
    printf("    PtrA (Original): %p\n", ptrA);
    printf("    PtrD (Nuevo):    %p\n", ptrD);

    // Verificación estricta
    if (ptrD == ptrA) {
        PASS("¡Coalescing exitoso! Los bloques A y B se fusionaron.");
        
        // Limpieza final
        my_free(ptrD);
        my_free(ptrC);
        return 1;
    } else {
        printf(COLOR_RED "[FAIL] No hubo fusión. El bloque D se asignó en una nueva dirección.\n" COLOR_RESET);
        printf(COLOR_RED "       Se esperaba reutilizar %p, pero se obtuvo %p\n" COLOR_RESET, ptrA, ptrD);
        return 0;
    }
}

// --------------------------------------------------------------------------
// MAIN
// --------------------------------------------------------------------------
int main() {
    printf(COLOR_BLUE "========================================\n");
    printf("    OS PROJECT 1: ULA-MALLOC TESTER    \n");
    printf("========================================\n" COLOR_RESET);
    
    int score = 0;
    int total = 6;
    
    if (test_basic_allocation()) score++;
    if (test_reuse()) score++;
    if (test_calloc()) score++;
    if (test_realloc()) score++;
    if (test_stress()) score++;
    if (test_coalescing()) score++;
    
    printf(COLOR_BLUE "\n========================================\n");
    printf("RESULTADO FINAL: %d/%d TESTS PASADOS\n", score, total);
    printf("========================================\n" COLOR_RESET);
    
    if (score == total) {
        printf(COLOR_GREEN "¡FELICIDADES! Tu malloc parece funcionar correctamente.\n" COLOR_RESET);
        printf("No olvides documentar tu código antes del Pull Request.\n");
        return 0;
    } else {
        printf(COLOR_RED "Aún tienes errores. Revisa los mensajes de fallo.\n" COLOR_RESET);
        return 1;
    }
}
