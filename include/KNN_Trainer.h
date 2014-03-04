#ifndef KNN_TRAINER_H
#define KNN_TRAINER_H

/*
*
*	<--- DIRECTORY STRUCTURE EXPECTED FOR TRAINER --->
*
* --/
*	 --/RAW_DATA/
*				--/TRAIN
*						 --/0 					//ABSENT - SAMPLE DATA
*						 --/1 					//PRESENT - SAMPLE DATA
*						 --/2 					//Blank - SAMPLE DATA
*				--/TESTING
*						 --/0 					//ABSENT - SAMPLE DATA
*						 --/1 					//PRESENT - SAMPLE DATA
*						 --/2 					//Blank - SAMPLE DATA
*
*	<--- DIRECTORY STRUCTURE EXPECTED FOR TRAINER --->
*/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "unistd.h"

using namespace std;
using namespace cv;

//Number of classifications (Real Number - should start from 1)
#define NUMBER_OF_CLASSES 2



class KNN_Trainer {

private:

	Mat _samples;			// For storing samples
	Mat _labels;			// For storing labels
	Mat sample_labels;		// For storing sample's label

	int number_of_samples;


	// Train the KNN classifier
	void train_classifier( string  PATH_TO_TRAINING_DATA , int NUMBER_OF_FILES);

	// Test the KNN classifier
	//void test_classifier( string  PATH_TO_TESTING_DATA , int NUMBER_OF_FILES);

	// Serialize (save) the training to file.
	void serialize_training();

public:

	KNN_Trainer(string  PATH_TO_TRAINING_DATA,/* string PATH_TO_TESTING_DATA,*/ int number_of_samples) {

		train_classifier(PATH_TO_TRAINING_DATA, number_of_samples);
		//test_classifier(//RAW_DATA//TESTING//);
		serialize_training();

		cvWaitKey(0);
	}


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


/********************************************************************/
/*							TRAIN_CLASSIFIER
*
*	Input Arguments:
*		string_to_training_data  : [string] String to training data
*		number_of_samples_in_dir : [int] Number of files in directory
*
*
* <-- FILE NAMING EXPECTED IN INPUT FILES -->
*
*	--/TRAIN
*		  	--/IMG (0).png
*		  	--/IMG (1).png
*		  	...
*		  	--/IMG (n).png
*
*	- Same for /TESTING -
*
* <-- FILE NAMING EXPECTED IN INPUT FILES -->
*/
/********************************************************************/

// Train the KNN classifier
void KNN_Trainer::train_classifier( string string_to_training_data,
					int number_of_samples_in_dir ) {

	// Mat for storing a single sample
	Mat sample;

	Mat sample_flattened;


// !IMP FIX  DIR PROBLEM

	//cout << string_to_training_data << endl;

	// Change directory to training data directory
	chdir( "RAW_DATA/TRAIN/" );

//    char stackBuffer[512];
//    cout << "current dir "  << getcwd(stackBuffer, sizeof(stackBuffer)) << endl;

// !IMP FIX  DIR PROBLEM


	for(int CURR_CLASS=0; CURR_CLASS<NUMBER_OF_CLASSES; CURR_CLASS++) {

		for(int CURR_SAMPLE=0; CURR_SAMPLE<number_of_samples_in_dir; CURR_SAMPLE++) {

// !IMP FIX  DIR PROBLEM
            string asdfasd = convertIntToString(CURR_CLASS);

            //cout << asdfasd << endl;
            chdir(asdfasd.c_str());
            //cout << "current dir asdfasd"  << getcwd(stackBuffer, sizeof(stackBuffer)) << endl;
// !IMP FIX  DIR PROBLEM

			string filename = "IMG ("+ convertIntToString(CURR_SAMPLE) + ").png";
			//cout << filename << endl;

			// Read each sample
			sample = imread(filename, 0);
            //threshold(sample,sample,200,255,THRESH_BINARY_INV); //Threshold to find contour
            cv::threshold(sample, sample, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

			// Consider using bezier-cubic - check output of resize -compare
			resize(sample, sample, Size(10,10), 0, 0, INTER_CUBIC );

			// Convert to float - flatten the multidim to single dim (single chan)
			sample.convertTo(sample_flattened, CV_32FC1);

			// Store sample (continuous) data
			_samples.push_back(sample_flattened.reshape(1,1));
			int c = CURR_CLASS;
			cout << "VALUE OF C IS " << c << endl;

			// Store label to a mat
			sample_labels.push_back(CURR_CLASS);

		}
// !IMP FIX  DIR PROBLEM
		chdir("..");
// !IMP FIX  DIR PROBLEM

	}
// !IMP FIX  DIR PROBLEM
	chdir("..");
	chdir("..");
// !IMP FIX  DIR PROBLEM

    // Store the data to file
    Mat sample_labels_temp;

	//make continuous
    sample_labels_temp = sample_labels.reshape(1,1);

	// Convert  to float and store in _labels
    sample_labels_temp.convertTo(_labels,CV_32FC1);

}


/********************************************************************/
/*							SERIALIZE_TRAINING
*/
/********************************************************************/

// This function serializes (writes to file) the training data.
void KNN_Trainer::serialize_training() {

	// Store the sample data in a file
    FileStorage Data("TrainingData.yml",FileStorage::WRITE);
    Data << "data" << _samples;
    Data.release();

	// Store the label data in a file
    FileStorage Label("LabelData.yml",FileStorage::WRITE);
    Label << "label" << _labels;
    Label.release();
    cout << "Training and Label data created successfully....!! " << endl;

}


// EXTRA
/********************************************************************

from line 25
something like
	crop the segment to only the boundary
		then parse it to the ROI thing.

		[todo]
		label according to folder name currently in (of the for loop)


examine the MLP trainer
	see how the iteration thing takes place for that.

********************************************************************/

#endif // KNN_TRAINER_H
