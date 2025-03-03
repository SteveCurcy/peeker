#include <stdio.h>
#include "random_forest.h"

int main(int argc, char *argv[]) {
    sample_t samples[200] = {
        {{0, 3, 2, 2}, 1},
        {{1, 0, 0, 1}, 0},
        {{0, 0, 2, 1}, 1},
        {{3, 3, 2, 2}, 0},
        {{1, 1, 3, 3}, 1},
        {{3, 0, 0, 2}, 2},
        {{3, 0, 2, 1}, 2},
        {{2, 1, 0, 0}, 0},
        {{3, 1, 1, 0}, 2},
        {{0, 0, 2, 2}, 2},
        {{2, 1, 3, 3}, 2},
        {{1, 1, 1, 2}, 1},
        {{0, 2, 3, 3}, 0},
        {{1, 2, 2, 3}, 0},
        {{3, 1, 2, 0}, 2},
        {{3, 2, 1, 1}, 2},
        {{3, 3, 3, 3}, 0},
        {{3, 0, 0, 0}, 0},
        {{2, 2, 2, 2}, 2},
        {{0, 2, 1, 3}, 0},
        {{2, 0, 0, 1}, 2},
        {{2, 2, 3, 0}, 2},
        {{0, 3, 0, 2}, 1},
        {{1, 2, 0, 0}, 2},
        {{2, 0, 0, 1}, 1},
        {{0, 2, 2, 1}, 1},
        {{3, 2, 3, 0}, 0},
        {{0, 0, 0, 0}, 0},
        {{2, 0, 0, 0}, 1},
        {{3, 3, 3, 1}, 2},
        {{1, 2, 1, 3}, 1},
        {{0, 0, 3, 1}, 0},
        {{2, 3, 0, 1}, 2},
        {{1, 3, 2, 3}, 2},
        {{2, 3, 0, 3}, 1},
        {{1, 1, 3, 1}, 1},
        {{0, 1, 0, 1}, 1},
        {{1, 1, 3, 3}, 2},
        {{2, 0, 3, 3}, 2},
        {{1, 0, 3, 3}, 1},
        {{2, 3, 0, 3}, 0},
        {{3, 3, 1, 1}, 2},
        {{0, 2, 2, 2}, 0},
        {{0, 0, 2, 2}, 2},
        {{2, 2, 3, 0}, 2},
        {{0, 3, 3, 3}, 0},
        {{1, 0, 1, 0}, 1},
        {{3, 0, 0, 1}, 2},
        {{2, 2, 1, 3}, 2},
        {{2, 1, 2, 0}, 1},
        {{1, 0, 0, 2}, 2},
        {{3, 0, 2, 1}, 0},
        {{1, 3, 0, 3}, 1},
        {{0, 0, 2, 0}, 0},
        {{0, 2, 1, 1}, 2},
        {{1, 0, 2, 1}, 1},
        {{2, 0, 1, 2}, 2},
        {{2, 0, 0, 1}, 0},
        {{3, 1, 2, 1}, 1},
        {{2, 2, 2, 0}, 2},
        {{1, 2, 3, 3}, 2},
        {{1, 2, 1, 2}, 0},
        {{1, 2, 1, 1}, 1},
        {{1, 2, 0, 0}, 2},
        {{0, 0, 2, 1}, 2},
        {{3, 0, 1, 3}, 0},
        {{2, 0, 2, 1}, 2},
        {{3, 0, 3, 1}, 2},
        {{3, 2, 1, 0}, 2},
        {{3, 0, 2, 1}, 0},
        {{1, 1, 1, 2}, 0},
        {{0, 3, 1, 2}, 2},
        {{1, 0, 2, 1}, 0},
        {{1, 0, 2, 1}, 0},
        {{3, 1, 0, 3}, 2},
        {{0, 1, 0, 1}, 2},
        {{0, 0, 2, 1}, 1},
        {{3, 1, 1, 1}, 0},
        {{2, 2, 1, 1}, 0},
        {{1, 0, 2, 1}, 0},
        {{3, 1, 1, 0}, 2},
        {{1, 0, 3, 1}, 1},
        {{3, 2, 1, 0}, 2},
        {{3, 2, 0, 1}, 2},
        {{0, 0, 0, 0}, 2},
        {{2, 0, 1, 3}, 2},
        {{1, 1, 3, 3}, 1},
        {{0, 2, 2, 3}, 2},
        {{1, 2, 1, 0}, 2},
        {{0, 3, 1, 0}, 1},
        {{0, 2, 0, 2}, 0},
        {{0, 1, 1, 1}, 0},
        {{0, 0, 0, 0}, 0},
        {{2, 3, 1, 0}, 1},
        {{1, 2, 3, 3}, 0},
        {{1, 1, 3, 1}, 1},
        {{3, 1, 3, 1}, 1},
        {{0, 2, 3, 1}, 0},
        {{2, 3, 2, 1}, 0},
        {{3, 0, 2, 0}, 0},
        {{3, 2, 1, 1}, 0},
        {{3, 2, 1, 0}, 1},
        {{1, 0, 2, 3}, 1},
        {{2, 3, 2, 2}, 0},
        {{2, 0, 2, 0}, 2},
        {{3, 0, 1, 2}, 0},
        {{3, 3, 2, 3}, 0},
        {{3, 0, 0, 3}, 2},
        {{3, 3, 1, 1}, 1},
        {{1, 0, 1, 1}, 2},
        {{1, 0, 2, 0}, 0},
        {{0, 0, 2, 1}, 2},
        {{1, 0, 3, 1}, 0},
        {{1, 2, 2, 3}, 0},
        {{2, 0, 3, 0}, 0},
        {{2, 0, 2, 2}, 1},
        {{1, 1, 2, 0}, 1},
        {{0, 0, 2, 2}, 0},
        {{0, 2, 3, 2}, 2},
        {{1, 2, 1, 0}, 0},
        {{1, 3, 1, 2}, 2},
        {{3, 0, 3, 2}, 0},
        {{2, 2, 2, 3}, 1},
        {{3, 0, 2, 3}, 2},
        {{1, 2, 2, 0}, 0},
        {{3, 3, 0, 1}, 2},
        {{1, 3, 2, 2}, 2},
        {{0, 0, 3, 1}, 1},
        {{1, 3, 1, 1}, 2},
        {{3, 3, 1, 2}, 1},
        {{0, 3, 1, 2}, 0},
        {{1, 0, 0, 3}, 1},
        {{2, 3, 0, 3}, 0},
        {{1, 1, 2, 2}, 1},
        {{3, 1, 1, 1}, 2},
        {{2, 3, 3, 3}, 2},
        {{0, 1, 1, 1}, 2},
        {{3, 0, 0, 2}, 0},
        {{2, 2, 0, 2}, 2},
        {{2, 3, 3, 2}, 0},
        {{1, 3, 0, 3}, 2},
        {{2, 3, 0, 2}, 0},
        {{0, 2, 3, 2}, 2},
        {{2, 1, 0, 2}, 1},
        {{3, 2, 2, 1}, 0},
        {{3, 2, 3, 1}, 1},
        {{3, 0, 3, 0}, 2},
        {{0, 2, 0, 0}, 2},
        {{0, 1, 2, 1}, 2},
        {{1, 2, 0, 2}, 0},
        {{3, 0, 1, 1}, 0},
        {{2, 2, 1, 1}, 1},
        {{0, 3, 2, 1}, 2},
        {{2, 1, 1, 2}, 2},
        {{2, 2, 1, 1}, 1},
        {{3, 3, 0, 0}, 0},
        {{0, 2, 0, 2}, 1},
        {{3, 3, 1, 0}, 0},
        {{1, 1, 1, 3}, 2},
        {{0, 1, 1, 1}, 2},
        {{0, 0, 3, 1}, 0},
        {{3, 2, 3, 1}, 0},
        {{1, 3, 0, 3}, 1},
        {{3, 3, 2, 3}, 2},
        {{0, 3, 3, 2}, 0},
        {{3, 2, 1, 0}, 1},
        {{0, 3, 1, 1}, 2},
        {{2, 3, 0, 2}, 1},
        {{3, 1, 0, 1}, 1},
        {{2, 3, 2, 2}, 0},
        {{1, 2, 3, 1}, 1},
        {{2, 0, 1, 2}, 1},
        {{3, 2, 1, 0}, 1},
        {{0, 0, 0, 2}, 0},
        {{3, 2, 3, 0}, 2},
        {{3, 0, 1, 1}, 0},
        {{0, 0, 1, 1}, 2},
        {{0, 1, 3, 2}, 1},
        {{2, 3, 2, 1}, 0},
        {{2, 1, 3, 1}, 2},
        {{3, 0, 2, 0}, 0},
        {{2, 0, 0, 0}, 2},
        {{0, 0, 2, 3}, 0},
        {{0, 0, 3, 2}, 0},
        {{1, 2, 3, 2}, 0},
        {{1, 0, 2, 0}, 0},
        {{3, 0, 2, 1}, 0},
        {{3, 3, 0, 1}, 0},
        {{3, 1, 1, 1}, 2},
        {{1, 1, 0, 2}, 1},
        {{0, 1, 3, 3}, 1},
        {{3, 2, 0, 0}, 2},
        {{1, 3, 2, 0}, 1},
        {{2, 3, 1, 0}, 0},
        {{1, 1, 2, 2}, 0},
        {{1, 3, 0, 2}, 2},
        {{1, 0, 0, 3}, 2},
        {{1, 2, 1, 1}, 1},
        {{0, 0, 2, 2}, 0},
        {{1, 2, 2, 1}, 2}
    };
    rand_forest_t *forest = train_rand_forest(samples, 200, 4, 3, 1, 4);
    sample_t sample = {{1, 3, 1, 2}, -1};
    printf("%d\n", predict_rand_forest(forest, &sample));
    destroy_rand_forest(&forest);
    return 0;
}