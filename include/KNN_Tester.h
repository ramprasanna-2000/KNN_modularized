#ifndef KNN_TESTER_H
#define KNN_TESTER_H

/**********************************************************************
*
*	<--- DIRECTORY STRUCTURE EXPECTED FOR TESTER --->
*
* --/
*	 --/RAW_DATA/
*				--/TESTING
*						 --/0 					//ABSENT - SAMPLE DATA
*						 --/1 					//PRESENT - SAMPLE DATA
*						 --/2 					//Blank - SAMPLE DATA
*
*	<--- DIRECTORY STRUCTURE EXPECTED FOR TESTER --->
***********************************************************************/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "unistd.h"

using namespace std;
using namespace cv;

class KNN_Tester {

private:

    Mat _samples;			// For storing samples
    Mat _labels;			// For storing labels
    KNearest knn;

    int TOTAL_SAMPLES;
    int TOTAL_CLASSES;

    // Test the KNN classifier
    void test_classifier( string  PATH_TO_TESTING_DATA);

    // Serialize (save) the training to file.
    void deserialize_training();

public:

    KNN_Tester(string PATH_TO_TESTING_DATA, int total_samples, int total_classes) : TOTAL_SAMPLES(total_samples), TOTAL_CLASSES(total_classes) {

        deserialize_training();

        test_classifier(PATH_TO_TESTING_DATA);

        cvWaitKey(0);
    }

    // Test if class works
    string sayHello() {
        return "hello";
    }

    // Convert (input) integer to String (using stringstream)
    string convertIntToString(int number) {
        stringstream ss;			//create a stringstream
        ss << number;				//add number to the stream
        return ss.str();			//return a string with the contents of the stream
    }

};



/********************************************************************
*							READ_TRAINING
*
********************************************************************/

// This function reads the training data, into the KNN trainer
void KNN_Tester::deserialize_training() {

    // Read stored sample and label for training

    FileStorage Data("TrainingData.yml",FileStorage::READ); // Read training data to a Mat
    Data["data"] >> _samples;
    Data.release();

    FileStorage Label("LabelData.yml",FileStorage::READ); // Read label data to a Mat
    Label["label"] >> _labels;
    Label.release();

    knn.train(_samples,_labels); // Train with sample and responses
    cout<<"Training compleated.....!!"<<endl;
}


/********************************************************************
*							TEST_CLASSIFIER
*
*	Input Arguments:
*		string_to_test_data  : [string] String to test data
*		TOTAL_SAMPLES_in_dir : [int] Number of files in directory
*
*
* <-- FILE NAMING EXPECTED IN INPUT FILES -->
*
* <-- Also, there should be equal number of files, in each class -->
*
*	--/TEST
*			--/0
*			  	--/IMG (0).png
*			  	--/IMG (1).png
*			  	...
*			  	--/IMG (n).png
*
*
* <-- FILE NAMING EXPECTED IN INPUT FILES -->
*
*********************************************************************/

// Test the KNN classifier
void KNN_Tester::test_classifier( string string_to_training_data) {

    // Mat for storing a single sample
    Mat sample;

    // Mat for storing CV_32FC1
    Mat sample_flattened;

    // Change directory to training data directory
    chdir( string_to_training_data.c_str() );

    char stackBuffer[512];

    //count of correct classifications
    int* correct_class = new int[TOTAL_CLASSES] {};

    //count of wrong classifications
    int* wrong_class = new int[TOTAL_CLASSES] {};

    for(int CURR_CLASS=0; CURR_CLASS<TOTAL_CLASSES; CURR_CLASS++) {

        for(int CURR_SAMPLE=0; CURR_SAMPLE<TOTAL_SAMPLES; CURR_SAMPLE++) {

            chdir( (convertIntToString(CURR_CLASS)).c_str());


            string filename = "IMG ("+ convertIntToString(CURR_SAMPLE+1) + ").png";
#if DEBUG
            cout << "current dir is "  << getcwd(stackBuffer, sizeof(stackBuffer)) << endl;
            cout << filename << endl;
#endif // DEBUG

            // Read each sample
            sample = imread(filename, 0);
            //threshold(sample,sample,200,255,THRESH_BINARY_INV);
            cv::threshold(sample, sample, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

            /* TODO TRY CATCH FOR -> IF NOT ABLE TO READ FILE THEN THROW EXCEPTION - HALT*/

            // Resize sample to 32,32
            resize(sample, sample, Size(32,32), 0, 0, INTER_CUBIC );

            // Convert to float - flatten the multidim to single dim (single chan)
            sample.convertTo(sample_flattened, CV_32FC1);

            float p=knn.find_nearest(sample_flattened.reshape(1,1), 1);


            // if sample is NOT classified correctly
            if (p != CURR_CLASS) {

                wrong_class[CURR_CLASS]++;
            } else {

                correct_class[CURR_CLASS]++;
            }

        }

        // Go up 1 file directory
        chdir("../");
    }

    // Go up 2 file directories
    chdir("../..");

    //testing %
    for(int CURR_CLASS=0; CURR_CLASS<TOTAL_CLASSES; CURR_CLASS++) {
        cout << "Correct classification for " << CURR_CLASS << " is ";
        cout <<  correct_class[CURR_CLASS]*100/TOTAL_SAMPLES << "%" <<  endl;

    }

    delete[] correct_class;
    delete[] wrong_class;
}

#endif // KNN_TESTER_H
