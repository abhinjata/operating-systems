#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define WIDTH 1000
#define HEIGHT 1000
#define NPIXELS (WIDTH * HEIGHT)

typedef struct {
    unsigned char r, g, b;
} Pixel;

Pixel image[NPIXELS];   // global image

typedef struct ThreadArg{

    int start;
    int end;
    int effect;  // which effect to apply (1..4)

} ThreadArg;

//actual worker function
void* process_pixels(void* arg) {

    ThreadArg* t = (ThreadArg*)arg;

    for (int i = t->start; i < t->end; i++) {

        Pixel* p = &image[i];

        if (t->effect == 1) {
            // Red Rage: invert red
            p->r = 255 - p->r;

        } 
        
        else if (t->effect == 2) {
            // Green Greed: double green, cap at 255
            p->g = (p->g * 2 > 255) ? 255 : p->g * 2;

        } 
        
        else if (t->effect == 3) {
            // Blue Blur: halve blue
            p->b /= 2;

        } 
        
        else if (t->effect == 4) {
            // Gray Scale Crush
            unsigned char gray = (p->r + p->g + p->b) / 3;
            p->r = p->g = p->b = gray;
        }

    }
    return NULL;
}

//helper to calculate elapsed time
double elapsed(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(void) {

    // Initialize image with random pixels
    srand(42);
    for (int i = 0; i < NPIXELS; i++) {
        image[i].r = rand() % 256;
        image[i].g = rand() % 256;
        image[i].b = rand() % 256;
    }

    //MULTITHREADED VERSION
    pthread_t threads[4];
    ThreadArg args[4];
    int quarter = NPIXELS / 4;

    struct timespec t1, t2;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    for (int i = 0; i < 4; i++) {
        args[i].start = i * quarter;
        args[i].end = (i == 3) ? NPIXELS : (i + 1) * quarter;
        args[i].effect = i + 1;
        pthread_create(&threads[i], NULL, process_pixels, &args[i]);
    }
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("Multithreaded time: %.6f sec\n", elapsed(t1, t2));

    // SINGLE-THREADED VERSION - sequentially
    // Reinitialize image to same random pixels
    srand(42);
    for (int i = 0; i < NPIXELS; i++) {
        image[i].r = rand() % 256;
        image[i].g = rand() % 256;
        image[i].b = rand() % 256;
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    int quarter_size = NPIXELS / 4;
    for (int eff = 1; eff <= 4; eff++) {
        int start = (eff - 1) * quarter_size;
        int end = (eff == 4) ? NPIXELS : eff * quarter_size;
        ThreadArg tmp = {start, end, eff};
        process_pixels(&tmp);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    printf("Single-threaded time: %.6f sec\n", elapsed(t1, t2));

    return 0;
}
