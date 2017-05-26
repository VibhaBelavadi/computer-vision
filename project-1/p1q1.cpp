#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	if (argc != 3) {
		cout << argv[0] << ": "
			<< "got " << argc - 1 << " arguments. Expecting two: width height."
			<< endl;
		return(-1);
	}

	int width = atoi(argv[1]);
	int height = atoi(argv[2]);
	int** RED1 = new int*[height];
	int** GREEN1 = new int*[height];
	int** BLUE1 = new int*[height];
	int** RED2 = new int*[height];
	int** GREEN2 = new int*[height];
	int** BLUE2 = new int*[height];

	for (int i = 0; i < height; i++) {
		RED1[i] = new int[width];
		GREEN1[i] = new int[width];
		BLUE1[i] = new int[width];
		RED2[i] = new int[width];
		GREEN2[i] = new int[width];
		BLUE2[i] = new int[width];
	}

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			int red1, green1, blue1;
			int red2, green2, blue2;

			double x = (double)j / (double)width;
			double y = (double)i / (double)height;
			double Y = 1.0;

			double L = 90;
			double u = x * 512 - 255;
			double v = y * 512 - 255;


			/* Your code should be placed here
			It should translate xyY to byte sRGB
			and Luv to byte sRGB
			*/

			//generating XYZ values
			double X = 0.0;
			double Z = 0.0;
			if (y != 0)
			{
				X = (x*Y) / y;
				Z = ((1 - x - y) / (y))*Y;
			}

			//cal r11 g11 b11 from XYZ
			double red11 = (3.240479 * X + (-1.53715 * Y) + (-0.498535 * Z));
			double green11 = (-0.969256 * X + 1.875991 * Y + 0.041556 * Z);
			double blue11 = (0.055648 * X + (-0.204043 * Y) + 1.057311 * Z);
			if (red11 > 1.0)
			{
				red11 = 1.0;
			}
			else if (red11 < 0.0)
			{
				red11 = 0.0;
			}
			if (green11 > 1.0)
			{
				green11 = 1.0;
			}
			else if (green11 < 0.0)
			{
				green11 = 0.0;
			}
			if (blue11 > 1.0)
			{
				blue11 = 1.0;
			}
			else if (blue11 < 0.0)
			{
				blue11 = 0.0;
			}

			if (red11 > 0.00304)
			{
				red11 = (1.055 * pow(red11, (1.0 / 2.4))) - 0.055;
			}
			else
			{
				red11 = 12.92* red11;				
			}
			if (green11 > 0.00304)
			{
				green11 = (1.055 * pow(green11, (1.0 / 2.4))) - 0.055;				
			}
			else
			{
				green11 = 12.92* green11;
			}
			if (blue11 > 0.00304)
			{
				blue11 = (1.055 * pow(blue11, (1.0 / 2.4))) - 0.055;
			}
			else
			{
				blue11 = 12.92* blue11;
			}

			//scale up the vals to 0-255 range
			red11 = red11 * (double)255;
			green11 = green11 *(double)255;
			blue11 = blue11 *(double)255;

			//convert Luv to XYZ
			double uw = 0.1977;
			double vw = 0.468;
			double u1 = 0.0;
			double v1 = 0.0;

			if (L != 0)
			{
				u1 = (u + (13.0 * uw) * L) / (13.0 * L);
				v1 = (v + (13.0 * vw) * L) / (13.0 * L);
			}
			if (L > 7.996)
			{
				Y = pow(((L + 16.0) / 116.0), 3.0) * 1.0;
			}
			else
			{
				Y = (L / 903.3)* 1.0;
			}
			if (v1 == 0.0)
			{
				X = 0.0, Z = 0.0;
			}
			else {
				X = Y * 2.25 * (u1 / v1);
				Z = (Y*(3.0 - (0.75*u1) - (5.0 * v1))) / (double)v1;
			}

			//convert XYZ to r22 g22 b22
			double red22 = (3.240479 * X + (-1.53715 * Y) + (-0.498535 * Z));
			double green22 = (-0.969256 * X + 1.875991 * Y + 0.041556 * Z);
			double blue22 = (0.055648 * X + (-0.204043 * Y) + 1.057311 * Z);
			if (red22 > 1.0)
			{
				red22 = 1.0;
			}
			else if (red22 < 0.0)
			{
				red22 = 0.0;
			}
			if (green22 > 1.0)
			{
				green22 = 1.0;
			}
			else if (green22 < 0.0)
			{
				green22 = 0.0;
			}
			if (blue22 > 1.0)
			{
				blue22 = 1.0;
			}
			else if (blue22 < 0.0)
			{
				blue22 = 0.0;
			}

			if (red22 > 0.00304)
			{
				red22 = (1.055 * pow(red22, (1.0 / 2.4))) - 0.055;				
			}
			else
			{
				red22 = 12.92* red22;
			}
			if (green22 > 0.00304)
			{
				green22 = (1.055 * pow(green22, (1.0 / 2.4))) - 0.055;				
			}
			else
			{
				green22 = 12.92* green22;
			}
			if (blue22 > 0.00304)
			{
				blue22 = (1.055 * pow(blue22, (1.0 / 2.4))) - 0.055;
			}
			else
			{
				blue22 = 12.92* blue22;
			}

			red22 = red22 * (double)255;
			green22 = green22 *(double)255;
			blue22 = blue22 *(double)255;

			// this is the end of your code

			RED1[i][j] = red11;
			GREEN1[i][j] = green11;
			BLUE1[i][j] = blue11;
			RED2[i][j] = red22;
			GREEN2[i][j] = green22;
			BLUE2[i][j] = blue22;
		}


	Mat R1(height, width, CV_8UC1);
	Mat G1(height, width, CV_8UC1);
	Mat B1(height, width, CV_8UC1);

	Mat R2(height, width, CV_8UC1);
	Mat G2(height, width, CV_8UC1);
	Mat B2(height, width, CV_8UC1);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++) {

			R1.at<uchar>(i, j) = RED1[i][j];
			G1.at<uchar>(i, j) = GREEN1[i][j];
			B1.at<uchar>(i, j) = BLUE1[i][j];

			R2.at<uchar>(i, j) = RED2[i][j];
			G2.at<uchar>(i, j) = GREEN2[i][j];
			B2.at<uchar>(i, j) = BLUE2[i][j];
		}

	Mat xyY;
	Mat xyY_planes[] = { B1, G1, R1 };
	merge(xyY_planes, 3, xyY);
	namedWindow("xyY", CV_WINDOW_AUTOSIZE);
	imshow("xyY", xyY);
	imwrite("xyY.png", xyY);

	Mat Luv;
	Mat Luv_planes[] = { B2, G2, R2 };
	merge(Luv_planes, 3, Luv);
	namedWindow("Luv", CV_WINDOW_AUTOSIZE);
	imshow("Luv", Luv);
	imwrite("Luv.png", Luv);
	waitKey(0); // Wait for a keystroke
	return(0);
}
