#ifndef KNN_TRAINER_H
#define KNN_TRAINER_H

/**********************************************************************
*
*	<--- DIRECTORY STRUCTURE EXPECTED FOR TRAINER --->
*
* --/
*	 --/RAW_DATA/
*				--/TRAIN
*						 --/0 					//ABSENT - SAMPLE DATA
*						 --/1 					//PRESENT - SAMPLE DATA
*						 --/2 					//Blank? - SAMPLE DATA
*
*	<--- DIRECTORY STRUCTURE EXPECTED FOR TRAINER --->
***********************************************************************/

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "unistd.h"

using namespace std;
using namespace cv;

//------------------------------------------------------------------------------------------
// Custom comparator
bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2) {

    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i < j );
}
//------------------------------------------------------------------------------------------

class KNN_Trainer {

private:

    Mat _samples;			// For storing samples
    Mat _labels;			// For storing labels
    Mat sample_labels;		// For storing sample's label

    int TOTAL_SAMPLES;
    int TOTAL_CLASSES;


    // Train the KNN classifier
    void train_classifier( string  PATH_TO_TRAINING_DATA);

    // Serialize (save) the training to file.
    void serialize_training();

//EEEEEXTRA
Mat getCroppedContour(Mat& segment);
//EEEEEXTRA

public:

    KNN_Trainer(string  PATH_TO_TRAINING_DATA, int total_samples, int total_classes) : TOTAL_SAMPLES(total_samples),  TOTAL_CLASSES(total_classes) {

        train_classifier(PATH_TO_TRAINING_DATA);

        serialize_training();

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

/*******************************************************************/
//-----------------TO REFACTOR-------------

Mat KNN_Trainer::getCroppedContour(Mat& segment) {
    vector< vector <Point> > contours; // Vector for storing contour
    vector< Vec4i > hierarchy;
    Mat croppedContour;
    Mat seg_copy = segment.clone(); //to prevent decomposition
    GaussianBlur(segment, segment, Size(5,5), 0, 0);


    findContours( segment, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
//	for( unsigned int i = 0; i< contours.size(); i=hierarchy[i][0] ) {
//	// iterate through each contour for first hierarchy level.
//			if(contourArea(contours[i]) < 100)
//				continue;
//
//		Rect r= boundingRect(contours[i]);
//		croppedContour = seg_copy(r);
//	}

    //sort the corners with a custom comparator
    sort(contours.begin(), contours.end(), compareContourAreas);


    // largest contour
    vector<Point> pt_largest;

    //take largest contour
    pt_largest = contours[contours.size()-1];

    Rect r= boundingRect(pt_largest);
    croppedContour = seg_copy(r);

    return croppedContour;
}
//-----------------TO REFACTOR-------------

/********************************************************************
*							TRAIN_CLASSIFIER
*
*	Input Arguments:
*		string_to_training_data  : [string] String to training data
*		TOTAL_SAMPLES_in_dir : [int] Number of files in directory
*
*
* <-- FILE NAMING EXPECTED IN INPUT FILES -->
*
* <-- Also, there should be equal number of files, in each class -->
*
*	--/TRAIN
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

// Train the KNN classifier
void KNN_Trainer::train_classifier( string string_to_training_data) {

    // Mat for storing a single sample
    Mat sample;

    // Mat for storing CV_32FC1
    Mat sample_flattened;

    // Change directory to training data directory
    chdir( string_to_training_data.c_str() );

    char stackBuffer[512];

    for(int CURR_CLASS=0; CURR_CLASS<TOTAL_CLASSES; CURR_CLASS++) {

        for(int CURR_SAMPLE=0; CURR_SAMPLE<TOTAL_SAMPLES; CURR_SAMPLE++) {

            chdir( (convertIntToString(CURR_CLASS)).c_str());


            string filename = "IMG ("+ convertIntToString(CURR_SAMPLE+1) + ").png";
#if DEBUG
            cout << "current dir asdfasd"  << getcwd(stackBuffer, sizeof(stackBuffer)) << endl;
            cout << filename << endl;
#endif // DEBUG

            // Read each sample
            sample = imread(filename, 0);
            //threshold(sample,sample,200,255,THRESH_BINARY_INV); //Threshold to find contour
            cv::threshold(sample, sample, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

sample = getCroppedContour(sample);
//imwrite(filename+"a.png", sample);

            // Resize sample to 32,32
            resize(sample, sample, Size(32,32), 0, 0, INTER_CUBIC );

            // Convert to float - flatten the multidim to single dim (single chan)
            sample.convertTo(sample_flattened, CV_32FC1);

            // Store sample (continuous) data
            _samples.push_back(sample_flattened.reshape(1,1));

            // Store label to a mat
            sample_labels.push_back(CURR_CLASS);

        }

        // Go up 1 file directory
        chdir("../");
    }

    // Go up 2 file directories
    chdir("../..");

    // Store the data to file
    Mat sample_labels_temp;

    //make continuous
    sample_labels_temp = sample_labels.reshape(1,1);

    // Convert  to float and store in _labels
    sample_labels_temp.convertTo(_labels,CV_32FC1);

}


/********************************************************************
*						SERIALIZE_TRAINING
*
********************************************************************/

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

    cout << "Training and Label data serialized successfully.... " << endl;

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
