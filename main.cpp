#include <iostream>
#include "KNN_Trainer.h"
#include "KNN_Tester.h"

int main() {

    KNN_Trainer train_knn_obj("RAW_DATA/TRAIN/", 141, 2);
    KNN_Tester test_knn_obj("RAW_DATA/TEST/", 141, 2);

    return 0;
}
