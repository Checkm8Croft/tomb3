// --- init.cpp fix ---

#include "../tomb3/pch.h"
#include "init.h"
#include "game.h"

#define MALLOC_SIZE (32*1024*1024) // 32 MB

char* malloc_ptr = nullptr;
long malloc_free = 0;
long malloc_used = 0;
GLVERTEX* CurrentGLVertex = nullptr;
GLVERTEX* VertexBuffer = nullptr;
GLVERTEX* UnRollBuffer = nullptr;
WATERTAB WaterTable[22][64];  // <-- definito qui
float wibble_table[32];        // <-- definito qui
extern void InitZTable();
extern void InitUVTable();

// TLVertex/UnRoll buffer
GLVERTEX* TLVertexBuffer = nullptr;
GLVERTEX* TLUnRollBuffer = nullptr;
void init_water_table() { 
    // il codice che già hai per inizializzare WaterTable
}

void CalculateWibbleTable() { 
    // il codice che già hai per inizializzare wibble_table
}


// Puntatori originali per free corretto
void* TLVertexBuffer_orig = nullptr;
void* TLUnRollBuffer_orig = nullptr;

void init_game_malloc() {
    if (!malloc_buffer) {
        malloc_buffer = (char*)malloc(MALLOC_SIZE);
        if (!malloc_buffer) {
            fprintf(stderr, "FATAL: cannot allocate malloc_buffer\n");
            exit(1);
        }
    }
    malloc_ptr = malloc_buffer;
    malloc_free = MALLOC_SIZE;
    malloc_used = 0;
}

void* game_malloc(long size) {
    size = (size + 3) & ~3;
    if (size > malloc_free) {
        fprintf(stderr, "game_malloc(): OUT OF MEMORY. Needed=%ld Free=%ld\n", size, malloc_free);
        exit(1);
    }
    void* ptr = malloc_ptr;
    malloc_ptr += size;
    malloc_free -= size;
    malloc_used += size;
    return ptr;
}

void game_free(long size) {
    size = (size + 3) & ~3;
    malloc_ptr -= size;
    malloc_free += size;
    malloc_used -= size;
}

void GlobalFree(void* ptr) {
 if (!ptr) return;
    uintptr_t p = (uintptr_t)ptr;
    uintptr_t pool_start = (uintptr_t)malloc_buffer;
    uintptr_t pool_end = pool_start + (uintptr_t)POOL_SIZE;
    if (p >= pool_start && p < pool_end) return;
    free(ptr);
}

long S_InitialiseSystem() {
    init_game_malloc();

    // Allineamento a 32-byte
    size_t vb_size = MAX_TLVERTICES * sizeof(GLVERTEX) + 32;
    TLVertexBuffer_orig = malloc(vb_size);
    TLUnRollBuffer_orig = malloc(vb_size);

    if (!TLVertexBuffer_orig || !TLUnRollBuffer_orig) {
        fprintf(stderr, "FATAL: cannot allocate TLVertex/UnRoll buffers\n");
        return 0;
    }

    uintptr_t aligned_tb = (((uintptr_t)TLVertexBuffer_orig) + 31) & ~((uintptr_t)31);
    uintptr_t aligned_ub = (((uintptr_t)TLUnRollBuffer_orig) + 31) & ~((uintptr_t)31);

    TLVertexBuffer = reinterpret_cast<GLVERTEX*>(aligned_tb);
    VertexBuffer = TLVertexBuffer;

    TLUnRollBuffer = reinterpret_cast<GLVERTEX*>(aligned_ub);
    UnRollBuffer = TLUnRollBuffer;

    InitZTable();
    InitUVTable();
    CalculateWibbleTable();
    init_water_table();

    return 1;
}

void ShutdownGame() {
    free(TLVertexBuffer_orig);
    free(TLUnRollBuffer_orig);
    free(malloc_buffer);

    TLVertexBuffer = VertexBuffer = nullptr;
    TLUnRollBuffer = UnRollBuffer = nullptr;
    TLVertexBuffer_orig = TLUnRollBuffer_orig = nullptr;
    malloc_buffer = malloc_ptr = nullptr;
    malloc_free = malloc_used = 0;
}
