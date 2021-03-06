#include "eigenfaces.h"
#include "file_functions.h"
#include "set_definitions.h"

using namespace cv;
using namespace std;

Mat getEigenFace(Mat eigenVec, Mat A, unsigned int numComp){
	Mat aux1(A.cols, numComp, eigenVec.type());
	Mat	aux2(A.rows, 1, eigenVec.type());
	Mat aux3(A.rows, numComp, eigenVec.type());
	Mat output(numComp, A.rows, eigenVec.type());

	for (unsigned int i = 0; i < numComp; i++){
		//cout << input.row(i) << endl << endl;
		transpose(eigenVec.row(i), aux1.col(i));
		//cout << aux1.col(i) << endl << endl;

		aux2 = A*aux1.col(i);
		//cout << aux2 << endl << endl;
		(aux2).copyTo(aux3.col(i));
		//cout << aux3.col(i) << endl << endl;
		//normalize(aux2, aux3.col(i));
	}
	transpose(aux3, output);
	// cout << output << endl << endl;
	return output;
}

void bruteForceEigen(Mat covMat){
	Mat eigVal_brute, eigVec_bruteT;
	eigen(covMat, eigVal_brute, eigVec_bruteT);
	writeFile(eigVec_bruteT, "eigVec_bruteT");
	writeFile(eigVal_brute, "eigVal_brute");

	Mat eigenFace_bruteT(5, 2500, CV_32FC1);
	int p = 5; // p componentes
	for (int i = 0; i < p; i++){
		(eigVec_bruteT.row(i)).copyTo(eigenFace_bruteT.row(i));
	}

	writeFile(eigenFace_bruteT, "eigenFace_bruteT");

	vector<Mat> eigFaceT2d;
	for (int i = 0; i < p; i++){
		eigFaceT2d.push_back((eigenFace_bruteT.row(i)).reshape(0, 50));
		imwrite("EigenFaceBrute" + to_string(i + 1) + ".jpg", eigFaceT2d[i]);
	}
}

Mat set2matrix(vector<Mat> &image_set){
	Mat output(image_set[0].cols*image_set[0].rows, image_set.size(), CV_32FC1);
	Mat outputT(image_set.size(), image_set[0].cols*image_set[0].rows, CV_32FC1);
	for (unsigned int i = 0; i < image_set.size(); i++)
		(image_set[i].reshape(0, 1)).copyTo(outputT.row(i));
	transpose(outputT, output);
	return output;
}

double classify(Mat trainProjection, Mat testProjection, vector<int> true_index, int setID, vector<int> &TP_index){

	int test_size;
	int train_size = SET1_SIZE;
	vector<int> face;

	switch (setID){
	case 1: face.resize(SET1_SIZE); test_size = SET1_SIZE; break;
	case 2: face.resize(SET2_SIZE); test_size = SET2_SIZE; break;
	case 3: face.resize(SET3_SIZE); test_size = SET3_SIZE; break;
	case 4: face.resize(SET4_SIZE); test_size = SET4_SIZE; break;
	case 5: face.resize(SET5_SIZE); test_size = SET5_SIZE; break;
	default: break;
	}

	double min_dist = 999999;
	int fails = 0;
	int wins = 0;

	double dist;
	int index = -1;

	for (int i = 0; i < test_size; i++){
		Mat curr_image = testProjection.col(i); //check every image weights 
		for (int j = 0; j < train_size; j++){
			dist = norm(curr_image, trainProjection.col(j), NORM_L2); //calculate the abs difference 
			if (dist < min_dist){
				min_dist = dist;
				index = j;
			}
		}
		face[i] = (int)floor(index / 7) + 1;
		if (face[i] != true_index[i]){
			fails++; 
			// cout<<"F"<<fails<<endl; 
			TP_index.push_back(-1);
		}
		else{
			wins++; 
			// cout<<"W"<<wins<<endl; 
			TP_index.push_back(face[i]);
		}
		min_dist = 999999;
	}

	double err_rate = (double)fails / (double)test_size;
	return err_rate;
}

double classifylabel(Mat trainProjection, vector<int> trainlabel, Mat testProjection, vector<int> testlabel) {
	int train_size = trainProjection.cols;
	int test_size = testProjection.cols;
	int index; double minDist = DBL_MAX;
	int negative = 0; int positive = 0;

	for (int i=0; i<test_size; i++) {
		Mat current_image = testProjection.col(i); 
		for (int j=0; j<train_size; j++) {
			double dist = norm(current_image, trainProjection.col(j), NORM_L2);
			if (dist < minDist){
				minDist = dist;
				index = j;
			}
		}
		if (trainlabel[index] == testlabel[i]) {
			// cout<<"This is classified correctly!" <<endl;
			positive++;
		}
		else {
			// cout<<"This is classified incorrectly!" <<endl;
			negative++;
		}
		// cout << "actual = " << testlabel[i] << + " predicted = "<< trainlabel[index] <<endl;
		minDist = DBL_MAX;
	}
	double err;
	err = (double) negative/ (double) test_size;
	return err;
}



