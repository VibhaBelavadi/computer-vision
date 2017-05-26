#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

void runOnWindow(int W1, int H1, int W2, int H2, Mat inputImage, char *outName) {
	int rows = inputImage.rows;
	int cols = inputImage.cols;

	vector<Mat> i_planes;
	split(inputImage, i_planes);
	Mat iB = i_planes[0];
	Mat iG = i_planes[1];
	Mat iR = i_planes[2];

	// dynamically allocate RGB arrays of size rows x cols
	int** Red = new int*[rows];
	int** Green = new int*[rows];
	int** Blue = new int*[rows];
	for (int i = 0; i < rows; i++) {
		Red[i] = new int[cols];
		Green[i] = new int[cols];
		Blue[i] = new int[cols];
	}

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			Red[i][j] = iR.at<uchar>(i, j);
			Green[i][j] = iG.at<uchar>(i, j);
			Blue[i][j] = iB.at<uchar>(i, j);
		}

	//	   The transformation should be based on the
	//	   historgram of the pixels in the W1,W2,H1,H2 range.
	//	   The following code goes over these pixels

	//converting pixel 3 bytes to R'G'B'
	double** Red_d = new double*[rows];
	double** Green_d = new double*[rows];
	double** Blue_d = new double*[rows];
	for (int i = 0; i < rows; i++) {
		Red_d[i] = new double[cols];
		Green_d[i] = new double[cols];
		Blue_d[i] = new double[cols];
	}

	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			Red_d[i][j] = double(Red[i][j]) / 255.0;
			Green_d[i][j] = double(Green[i][j]) / 255.0;
			Blue_d[i][j] = double(Blue[i][j]) / 255.0;

		}

	//R'G'B' to RGB by Inverse Gamma Correction
	double** Red_Iv = new double*[rows];
	double** Green_Iv = new double*[rows];
	double** Blue_Iv = new double*[rows];

	for (int i = 0; i < rows; i++) {
		Red_Iv[i] = new double[cols];
		Green_Iv[i] = new double[cols];
		Blue_Iv[i] = new double[cols];
	}

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (Red_d[i][j] < 0.03928) {
				Red_Iv[i][j] = (double)Red_d[i][j] / 12.92;
			}
			else {
				Red_Iv[i][j] = (double)pow(((Red_d[i][j] + 0.055) / 1.055), 2.4);
			}

			if (Green_d[i][j] < 0.03928) {
				Green_Iv[i][j] = (double)Green_d[i][j] / 12.92;
			}
			else {
				Green_Iv[i][j] = (double)pow(((Green_d[i][j] + 0.055) / 1.055), 2.4);
			}
			if (Blue_d[i][j] < 0.03928) {
				Blue_Iv[i][j] = (double)Blue_d[i][j] / 12.92;
			}
			else {
				Blue_Iv[i][j] = (double)pow(((Blue_d[i][j] + 0.055) / 1.055), 2.4);
			}
		}
	}

	//Rv Gv Bv to XYZ
	double** X = new double*[rows];
	double** Y = new double*[rows];
	double** Z = new double*[rows];
	for (int i = 0; i < rows; ++i) {
		X[i] = new double[cols];
		Y[i] = new double[cols];
		Z[i] = new double[cols];
	}

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			X[i][j] = (double)(0.412453*Red_Iv[i][j] + 0.35758*Green_Iv[i][j] + 0.180423*Blue_Iv[i][j]);
			Y[i][j] = (double)(0.212671*Red_Iv[i][j] + 0.71516*Green_Iv[i][j] + 0.072169*Blue_Iv[i][j]);
			Z[i][j] = (double)(0.019334*Red_Iv[i][j] + 0.119193*Green_Iv[i][j] + 0.950227*Blue_Iv[i][j]);
		}
	}

	//XYZ to xyY
	double** x = new double*[rows];
	double** y = new double*[rows];
	double** y_two = new double*[rows];
	for (int i = 0; i < rows; ++i) {
		x[i] = new double[cols];
		y[i] = new double[cols];
		y_two[i] = new double[cols];
	}
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if ((X[i][j] + Y[i][j] + Z[i][j]) == 0) {
				x[i][j] = 0;
				y[i][j] = 0;
				y_two[i][j] = Y[i][j];
			}
			else {
				x[i][j] = (double)X[i][j] / (double)(X[i][j] + Y[i][j] + Z[i][j]);
				if (i == 0) cout << x[i][j] << " ";
				y[i][j] = (double)Y[i][j] / (double)(X[i][j] + Y[i][j] + Z[i][j]);
				y_two[i][j] = Y[i][j];
			}
		}
	}

	//cal max and min value of y2 in the window which is meant to be used for scaling later on
	double min = y_two[H1][W1];
	double max = y_two[H1][W1];
	for (int i = H1; i <= H2; ++i) {
		for (int j = W1; j <= W2; ++j) {
			if (y_two[i][j] > max) {
				max = y_two[i][j];
			}
			if (y_two[i][j]<min) {
				min = y_two[i][j];
			}
		}
	}
	cout << "min: " << min << "max: " << max;

	//lin scaling y2 values
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (y_two[i][j] < min) {
				y_two[i][j] = 0.0;
			}
			else if (y_two[i][j]>max) {
				y_two[i][j] = 1.0;
			}
			else {
				if (max > min) {
					y_two[i][j] = (y_two[i][j] - min) / (max - min) * 1.0;
				}
				else {
					y_two[i][j] = 1.0;
				}
			}
		}
	}

	//xyY to XYZ
	double** x_o = new double*[rows];
	double** y_o = new double*[rows];
	double** z_o = new double*[rows];

	for (int i = 0; i < rows; ++i) {
		x_o[i] = new double[cols];
		y_o[i] = new double[cols];
		z_o[i] = new double[cols];
	}
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (y[i][j] == 0) {
				x_o[i][j] = 0.0;
				y_o[i][j] = y_two[i][j];
				z_o[i][j] = 0.0;
			}
			else {
				x_o[i][j] = x[i][j] * y_two[i][j] / y[i][j];
				y_o[i][j] = y_two[i][j];
				z_o[i][j] = (1 - x[i][j] - y[i][j])*y_two[i][j] / y[i][j];
			}
		}
	}

	// XYZ to lin sRGB
	double** linear_sr = new double*[rows];
	double** linear_sg = new double*[rows];
	double** linear_sb = new double*[rows];

	for (int i = 0; i < rows; ++i) {
		linear_sr[i] = new double[cols];
		linear_sg[i] = new double[cols];
		linear_sb[i] = new double[cols];
	}

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			linear_sr[i][j] = (3.240479)*x_o[i][j] + (-1.53715)*y_o[i][j] + (-0.498535)*z_o[i][j];
			linear_sg[i][j] = (-0.969256)*x_o[i][j] + (1.875991)*y_o[i][j] + (0.041556)*z_o[i][j];
			linear_sb[i][j] = (0.055648)*x_o[i][j] + (-0.204043)*y_o[i][j] + (1.057311)*z_o[i][j];
		}
	}


	//clipping
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (linear_sr[i][j] < 0) {
				linear_sr[i][j] = 0;
			}
			else if (linear_sr[i][j]>1) {
				linear_sr[i][j] = 1;
			}

			if (linear_sg[i][j] < 0) {
				linear_sg[i][j] = 0;
			}
			else if (linear_sg[i][j]>1) {
				linear_sg[i][j] = 1;
			}

			if (linear_sb[i][j] < 0) {
				linear_sb[i][j] = 0;
			}
			else if (linear_sb[i][j]>1) {
				linear_sb[i][j] = 1;
			}
		}
	}

	//linear sRGB to non-linear sRGB using Gamma Correction
	double** non_linear_sr = new double*[rows];
	double** non_linear_sg = new double*[rows];
	double** non_linear_sb = new double*[rows];

	for (int i = 0; i < rows; ++i) {
		non_linear_sr[i] = new double[cols];
		non_linear_sg[i] = new double[cols];
		non_linear_sb[i] = new double[cols];
	}

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (linear_sr[i][j] < 0.00304) {
				non_linear_sr[i][j] = 12.92*linear_sr[i][j];
			}
			else {
				non_linear_sr[i][j] = 1.055*pow(linear_sr[i][j], (1 / 2.4)) - 0.055;
			}
			if (linear_sg[i][j] < 0.00304) {
				non_linear_sg[i][j] = 12.92*linear_sg[i][j];
			}
			else {
				non_linear_sg[i][j] = 1.055*pow(linear_sg[i][j], (1 / 2.4)) - 0.055;
			}
			if (linear_sb[i][j] < 0.00304) {
				non_linear_sb[i][j] = 12.92*linear_sb[i][j];
			}
			else {
				non_linear_sb[i][j] = 1.055*pow(linear_sb[i][j], (1 / 2.4)) - 0.055;
			}
		}
	}

	//0-1 to 0 to 255
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			non_linear_sr[i][j] = non_linear_sr[i][j] * 255.0;
			non_linear_sg[i][j] = non_linear_sg[i][j] * 255.0;
			non_linear_sb[i][j] = non_linear_sb[i][j] * 255.0;
		}
	}

	// this is the end of your code

	Mat oR(rows, cols, CV_8UC1);
	Mat oG(rows, cols, CV_8UC1);
	Mat oB(rows, cols, CV_8UC1);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++) {
			oR.at<uchar>(i, j) = non_linear_sr[i][j];
			oG.at<uchar>(i, j) = non_linear_sg[i][j];
			oB.at<uchar>(i, j) = non_linear_sb[i][j];
		}

	Mat o_planes[] = { oB, oG, oR };
	Mat outImage;
	merge(o_planes, 3, outImage);

	namedWindow("output", CV_WINDOW_AUTOSIZE);
	imshow("output", outImage);
	imwrite(outName, outImage);
}

int main(int argc, char** argv) {
	if (argc != 7) {
		cerr << argv[0] << ": "
			<< "got " << argc - 1
			<< " arguments. Expecting six: w1 h1 w2 h2 ImageIn ImageOut."
			<< endl;
		cerr << "Example: proj1b 0.2 0.1 0.8 0.5 fruits.jpg out.bmp" << endl;
		return(-1);
	}
	double w1 = atof(argv[1]);
	double h1 = atof(argv[2]);
	double w2 = atof(argv[3]);
	double h2 = atof(argv[4]);
	char *inputName = argv[5];
	char *outputName = argv[6];

	if (w1<0 || h1<0 || w2 <= w1 || h2 <= h1 || w2>1 || h2>1) {
		cerr << " arguments must satisfy 0 <= w1 < w2 <= 1"
			<< " ,  0 <= h1 < h2 <= 1" << endl;
		return(-1);
	}

	Mat inputImage = imread(inputName, CV_LOAD_IMAGE_UNCHANGED);
	if (inputImage.empty()) {
		cout << "Could not open or find the image " << inputName << endl;
		return(-1);
	}

	string windowInput("input: ");
	windowInput += inputName;

	namedWindow(windowInput, CV_WINDOW_AUTOSIZE);
	imshow(windowInput, inputImage);

	if (inputImage.type() != CV_8UC3) {
		cout << inputName << " is not a standard color image  " << endl;
		return(-1);
	}

	int rows = inputImage.rows;
	int cols = inputImage.cols;
	int W1 = (int)(w1*(cols - 1));
	int H1 = (int)(h1*(rows - 1));
	int W2 = (int)(w2*(cols - 1));
	int H2 = (int)(h2*(rows - 1));

	runOnWindow(W1, H1, W2, H2, inputImage, outputName);

	waitKey(0); // Wait for a keystroke
	return(0);
}