/* GPL-2.0 LICENSE
 * Copyright (c) 2025 Steve
 * This Code is to provide the implementation of random forest. */
#include "random_forest.h"


/**
 * @param sample_cnt count number of samples in each class
 * @param sample_nr  total number of samples in that DT node.
 * @param cls_nr     number of classes
 * @return the gini impurity
 */
static double gini_impurity(int *sample_cnt, int sample_nr, int cls_nr)
{
    // if the len greater than MAX_CLASS_NR, not support
    if (cls_nr >= MAX_CLASS_NR) {
        return -1.0;
    }

    double impurity = 1.0;
    for (int i = 0; i < cls_nr; i++) {
        double p = (double)sample_cnt[i] / sample_nr;
        impurity -= p * p;
    }
    return impurity;
}

/**
 * @param samples find the best split from which part samples?
 * @param sample_nr number of samples
 * @param feature_nr number of features of each sample
 * @param cls_nr number of classes
 * @param best_threshold threshold to split samples into 2 parts
 * @return which feature will be used to split samples
 */
static int find_best_split(sample_t *samples, int sample_nr,
                    int feature_nr, int cls_nr, double *best_threshold)
{
    if (feature_nr >= MAX_FEATURE_NR || cls_nr >= MAX_CLASS_NR) {
        return -1;
    }

    int best_gain = 0; 
    int best_feature_idx = -1;
    *best_threshold = 0.0;

    // traverse the features, and assume that is the threshold
    for (int feature_idx = 0; feature_idx < feature_nr; feature_idx++) {
        for (int i = 0; i < sample_nr; i++) {
            // choose a better sample to get a feature value as thrshold.
            double threshold = samples[i].features[feature_idx];
            int lft_cnt[MAX_CLASS_NR] = {0}, lft_nr = 0;
            int rgt_cnt[MAX_CLASS_NR] = {0}, rgt_nr = 0;
            int tot_cnt[MAX_CLASS_NR] = {0};

            for (int j = 0; j < sample_nr; j++) {
                if (samples[j].features[feature_idx] < threshold) {
                    lft_cnt[samples[j].label]++;
                    lft_nr++;
                } else {
                    rgt_cnt[samples[j].label]++;
                    rgt_nr++;
                }
                tot_cnt[samples[j].label]++;
            }

            double lft_gini = gini_impurity(lft_cnt, lft_nr, cls_nr);
            double rgt_gini = gini_impurity(rgt_cnt, rgt_nr, cls_nr);
            double gain = gini_impurity(tot_cnt, lft_nr + rgt_nr, cls_nr)
                        - (lft_nr / (double) (lft_nr + rgt_nr)) * lft_gini
                        - (rgt_nr / (double) (lft_nr + rgt_nr)) * rgt_gini;
            if (gain > best_gain) {
                best_gain = gain;
                best_feature_idx = feature_idx;
                *best_threshold = threshold;
            }
        }
    }

    return best_feature_idx;
}

/**
 * @param samples sample used to train current tree
 * @param sample_nr end index of samples
 * @param depth current depth of node in the DT
 * @param feature_nr number of features
 * @param cls_nr number of classes
 * @param max_depth max depth of DT
 * @return root node of current DT
 */
static node_t* build_tree(sample_t *samples, int sample_nr, int depth,
                   int feature_nr, int cls_nr, int max_depth)
{
    if (sample_nr <= 1 || depth >= max_depth) {
        node_t *leaf = (node_t *)malloc(sizeof(node_t));
        int cls_cnt[MAX_CLASS_NR] = {0};
        int label = -1, label_cnt = 0;
        for (int i = 0; i < sample_nr; i++) {
            cls_cnt[samples[i].label]++;
            if (cls_cnt[samples[i].label] > label_cnt) {
                label_cnt = cls_cnt[samples[i].label];
                label = samples[i].label;
            }
        }
        leaf->label = label;
        leaf->lft = NULL;
        leaf->rgt = NULL;
        return leaf;
    }

    double best_threshold;
    int best_feature = find_best_split(samples, sample_nr, feature_nr, cls_nr, &best_threshold);
    if (best_feature == 0 || best_feature == sample_nr - 1) {
        node_t *leaf = (node_t *)malloc(sizeof(node_t));
        int cls_cnt[MAX_CLASS_NR] = {0};
        int label = -1, label_cnt = 0;
        for (int i = 0; i < sample_nr; i++) {
            cls_cnt[samples[i].label]++;
            if (cls_cnt[samples[i].label] > label_cnt) {
                label_cnt = cls_cnt[samples[i].label];
                label = samples[i].label;
            }
        }
        leaf->label = label;
        leaf->lft = NULL;
        leaf->rgt = NULL;
        return leaf;
    }

    node_t* node = (node_t *)malloc(sizeof(node_t));
    node->feature_index = best_feature;
    node->threshold = best_threshold;

    // the first node which upper than threshold
    int mid = 0;
    for (int i = 0; i < sample_nr; i++) {
        if (samples[i].features[best_feature] <= best_threshold) {
            if (i == mid) {
                mid++;
                continue;
            }
            sample_t temp = samples[mid];
            samples[mid] = samples[i];
            samples[i] = temp;
            mid++;
        }
    }

    node->lft = build_tree(samples, mid, depth + 1, feature_nr, cls_nr, max_depth);
    node->rgt = build_tree(samples + mid, sample_nr - mid, depth + 1, feature_nr, cls_nr, max_depth);

    return node;
}

static int destroy_tree(node_t **root)
{
    int err = 0;    // how many nodes destroy
    if ((*root)->lft) {
        int errno = destroy_tree(&((*root)->lft));
        if (errno < 0) {
            return errno;
        }
        err += errno;
    }
    if ((*root)->rgt) {
        int errno = destroy_tree(&((*root)->rgt));
        if (errno < 0) {
            return errno;
        }
        err += errno;
    }
    free(*root);
    *root = NULL;
    err++;
    return err;
}

/**
 * @param samples samples used to train
 * @param sample_nr number of samples
 * @param feature_nr maximum number of features of sample
 * @param cls_nr how many classes to classify
 * @param tree_nr number of trees of forest
 * @param max_depth maximum depth of each tree
 * @return a random forest module
 */
rand_forest_t* train_rand_forest(sample_t *samples, int sample_nr,
                                 int feature_nr, int cls_nr,
                                 int tree_nr, int max_depth)
{
    if (tree_nr >= MAX_TREE_NR || sample_nr >= MAX_SAMPLE_NR ||
        cls_nr >= MAX_CLASS_NR || feature_nr >= MAX_FEATURE_NR) {
        return NULL;
    }

    rand_forest_t *forest = (rand_forest_t *)malloc(sizeof(rand_forest_t));
    forest->trees_nr = tree_nr;
    forest->feature_sz = feature_nr;
    forest->class_sz = cls_nr;

    for (int i = 0; i < tree_nr; i++) {
        forest->trees[i] = build_tree(samples, sample_nr, 1,
                                      feature_nr, cls_nr, max_depth);
    }
    return forest;
}

int destroy_rand_forest(rand_forest_t **forest)
{
    int err = 0;
    int tree_nr = (*forest)->trees_nr;
    for (int i = 0; i < tree_nr; i++) {
        int errno = destroy_tree(&((*forest)->trees[i]));
        if (errno < 0) {
            return errno;
        }
    }
    free((*forest));
    *forest = NULL;
    return err;
}

static int predict_sample(node_t *node, sample_t *sample) {
    if (node->lft == NULL && node->rgt == NULL) {
        return node->label;
    }
    if (sample->features[node->feature_index] <= node->threshold) {
        return predict_sample(node->lft, sample);
    } else {
        return predict_sample(node->rgt, sample);
    }
}

int predict_rand_forest(rand_forest_t *forest, sample_t *sample) {
    int votes[MAX_CLASS_NR] = {0};

    for (int i = 0; i < forest->trees_nr; i++) {
        int prediction = predict_sample(forest->trees[i], sample);
        votes[prediction]++;
    }

    int max_votes = 0;
    int predict_label = 0;
    for (int i = 0; i < forest->class_sz; i++) {
        if (votes[i] > max_votes) {
            max_votes = votes[i];
            predict_label = i;
        }
    }

    return predict_label;
}