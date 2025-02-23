/* GPL-2.0
 * Copyright (c) 2025 Steve */
#ifndef __RANDOM_FOREST_H

#define __RANDOM_FOREST_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_FEATURE_NR  30  // maximum of the number of features
#define MAX_SAMPLE_NR   100000  // maximum of the number of samples
#define MAX_CLASS_NR    30  // maximum of the number of classes
#define MAX_TREE_NR     10
#define MAX_TREE_DEPTH  10  // maximum of the number of tree's depth

typedef struct __Sample {
    double features[MAX_FEATURE_NR];
    int label; // the class tag, meaning the behaviors
} sample_t;

typedef struct __Node {
    int feature_index;
    double threshold;
    int label;
    struct __Node *lft;
    struct __Node *rgt;
} node_t;

typedef struct __RandomForest {
    node_t *trees[MAX_TREE_NR];
    int trees_nr;
    int feature_sz;
    int class_sz;
} rand_forest_t;

rand_forest_t* train_rand_forest(sample_t *samples, int sample_sz,
                                 int feature_sz, int class_sz,
                                 int tree_nr, int max_depth);
int predict_rand_forest(rand_forest_t *forest, sample_t *sample);
int destroy_rand_forest(rand_forest_t **forest);

#endif