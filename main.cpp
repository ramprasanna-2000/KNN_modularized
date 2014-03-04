#include <iostream>
#include "KNN_Trainer.h"

int main() {

    KNN_Trainer test_knn_obj("/RAW_DATA/TRAIN", 3);
    cout << test_knn_obj.sayHello() << endl;


    return 0;
}
