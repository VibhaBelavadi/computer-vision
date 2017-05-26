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


	//px values to R'G'B' (0-1 range)
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

	//R'G'B' to RGB by Inv. Gamma Corr.
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
				Red_Iv[i][j] = Red_d[i][j] / 12.92;
			}
			else {
				Red_Iv[i][j] = pow(((Red_d[i][j] + 0.055) / 1.055), 2.4);
			}

			if (Green_d[i][j] < 0.03928) {
				Green_Iv[i][j] = Green_d[i][j] / 12.92;
			}
			else {
				Green_Iv[i][j] = pow(((Green_d[i][j] + 0.055) / 1.055), 2.4);
			}
			if (Blue_d[i][j] < 0.03928) {
				Blue_Iv[i][j] = Blue_d[i][j] / 12.92;
			}
			else {
				Blue_Iv[i][j] = pow(((Blue_d[i][j] + 0.055) / 1.055), 2.4);
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
			X[i][j] = 0.412453*Red_Iv[i][j] + 0.35758*Green_Iv[i][j] + 0.180423*Blue_Iv[i][j];
			Y[i][j] = 0.212671*Red_Iv[i][j] + 0.71516*Green_Iv[i][j] + 0.072169*Blue_Iv[i][j];
			Z[i][j] = 0.019334*Red_Iv[i][j] + 0.119193*Green_Iv[i][j] + 0.950227*Blue_Iv[i][j];
		}
	}

	//XYZ to Luv
	double X_w = 0.95;
	double Y_w = 1.0;
	double Z_w = 1.09;

	double u_w = 4 * X_w / (X_w + 15 * Y_w + 3 * Z_w);
	double v_w = 9 * Y_w / (X_w + 15 * Y_w + 3 * Z_w);

	double** L = new double*[rows];
	double** u = new double*[rows];
	double** v = new double*[rows];

	for (int i = 0; i < rows; ++i) {
		L[i] = new double[cols];
		u[i] = new double[cols];
		v[i] = new double[cols];
	}

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			double t = Y[i][j] / Y_w;
			if (t>0.008856) {
				L[i][j] = 116 * pow(t, 1 / 3.0) - 16;
			}
			else {
				L[i][j] = 903.3*t;
			}
			double d = X[i][j] + 15 * Y[i][j] + 3 * Z[i][j];
			double u_d;
			double v_d;
			if (d == 0) {
				u[i][j] = 0;
				v[i][j] = 0;
			}
			else {
				u_d = 4 * X[i][j] / d;
				v_d = 9 * Y[i][j] / d;
				u[i][j] = 13 * L[i][j] * (u_d - u_w);
				v[i][j] = 13 * L[i][j] * (v_d - v_w);
			}
		}
	}

	//cal max and min of L in the window to be used for scaling later on
	double min = L[H1][W1];
	double max = L[H1][W1];
	for (int i = H1; i <= H2; ++i) {
		for (int j = W1; j <= W2; ++j) {
			if (L[i][j] > max) {
				max = L[i][j];
			}
			if (L[i][j]<min) {
				min = L[i][j];
			}
		}
	}
	cout << "min: " << min << "max: " << max;

	//scale L values (linear)
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (L[i][j] < min) {
				L[i][j] = 0.0;
			}
			else if (L[i][j]>max) {
				L[i][j] = 100.0;
			}
			else {
				if (max > min) {
					L[i][j] = (L[i][j] - min) / (max - min) * 100;
				}
				else {
					L[i][j] = 100;
				}
			}
		}
	}

	//Luv to XYZ
	double** ud = new double*[rows];
	double** vd = new double*[rows];
	double** x_o = new double*[rows];
	double** y_o = new double*[rows];
	double** z_o = new double*[rows];


	for (int i = 0; i < rows; ++i) {
		ud[i] = new double[cols];
		vd[i] = new double[cols];
		x_o[i] = new double[cols];
		y_o[i] = new double[cols];
		z_o[i] = new double[cols];

	}
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (L[i][j] != 0) {
				ud[i][j] = (u[i][j] + 13 * u_w*L[i][j]) / (13 * L[i][j]);
				vd[i][j] = (v[i][j] + 13 * v_w*L[i][j]) / (13 * L[i][j]);
			}
			else {
				ud[i][j] = 0;
				vd[i][j] = 0;
			}
			if (L[i][j] > 7.9996) {
				y_o[i][j] = ((L[i][j] + 16) / 116.0)*((L[i][j] + 16) / 116.0)*((L[i][j] + 16) / 116.0)*Y_w;
			}
			else {
				y_o[i][j] = (L[i][j] / 903.3)*Y_w;
			}
			if (vd[i][j] == 0) {
				x_o[i][j] = 0;
				z_o[i][j] = 0;
			}
			else {
				x_o[i][j] = y_o[i][j] * 2.25*ud[i][j] / vd[i][j];
				z_o[i][j] = y_o[i][j] * (3 - 0.75*ud[i][j] - 5 * vd[i][j]) / vd[i][j];
			}

		}
	}

	// XYZ to linear sRGB
	double** linear_SR = new double*[rows];
	double** linear_SG = new double*[rows];
	double** linear_SB = new double*[rows];

	for (int i = 0; i < rows; ++i) {
		linear_SR[i] = new double[cols];
		linear_SG[i] = new double[cols];
		linear_SB[i] = new double[cols];
	}

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			linear_SR[i][j] = (3.240479)*x_o[i][j] + (-1.53715)*y_o[i][j] + (-0.498535)*z_o[i][j];
			linear_SG[i][j] = (-0.969256)*x_o[i][j] + (1.875991)*y_o[i][j] + (0.041556)*z_o[i][j];
			linear_SB[i][j] = (0.055648)*x_o[i][j] + (-0.204043)*y_o[i][j] + (1.057311)*z_o[i][j];
		}
	}

	//clipping	
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (linear_SR[i][j] < 0) {
				linear_SR[i][j] = 0;
			}
			else if (linear_SR[i][j]>1) {
				linear_SR[i][j] = 1;
			}

			if (linear_SG[i][j] < 0) {
				linear_SG[i][j] = 0;
			}
			else if (linear_SG[i][j]>1) {
				linear_SG[i][j] = 1;
			}

			if (linear_SB[i][j] < 0) {
				linear_SB[i][j] = 0;
			}
			else if (linear_SB[i][j]>1) {
				linear_SB[i][j] = 1;
			}
		}
	}

	//linear sRGB to non-linear sRGB (gamma Corr.)
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
			if (linear_SR[i][j] < 0.00304) {
				non_linear_sr[i][j] = 12.92*linear_SR[i][j];
			}
			else {
				non_linear_sr[i][j] = 1.055*pow(linear_SR[i][j], (1 / 2.4)) - 0.055;
			}
			if (linear_SG[i][j] < 0.00304) {
				non_linear_sg[i][j] = 12.92*linear_SG[i][j];
			}
			else {
				non_linear_sg[i][j] = 1.055*pow(linear_SG[i][j], (1 / 2.4)) - 0.055;
			}
			if (linear_SB[i][j] < 0.00304) {
				non_linear_sb[i][j] = 12.92*linear_SB[i][j];
			}
			else {
				non_linear_sb[i][j] = 1.055*pow(linear_SB[i][j], (1 / 2.4)) - 0.055;
			}
		}
	}

	//0-1 to 255
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