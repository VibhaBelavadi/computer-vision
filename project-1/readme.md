## The contents of the folder are as follows:

You are asked to write programs, all related to the manipulation of color in digital images.

1. First program (p1q1.cpp): <br />
Write a program that displays continuous changes in color for the xy and the Luv representations. The input to the program is a width and a height. The output is two images of dimensions width and height that are displayed on the screen.

2. Second program (p1q2.cpp): <br />
Write a program that gets as input a color image, performs linear scaling in the Luv domain, and writes the scaled image as output. The scaling in Luv should stretch only the luminance values. You are asked to apply linear scaling that would map the smallest L value in the specified window and all values below it
to 0, and the largest L value in the specified window and all values above it to 100.

3. Third program (p1q3.cpp): <br />
Write a program that gets as input a color image, performs histogram equalization in the Luv domain, and writes the scaled image as output. Histogram equalization in Luv is applied to the luminance values, as computed in the specified window. It requires a discretization step, where the real-valued L is discretized
into 101 values. 
As in the second program, all L values below the smallest L value in the window should be mapped to 0, and all L value above the largest L value in the window should be mapped to 100.

4. Fourth program (p1q4.cpp): <br />
This is the same as the second program, except that the scaling is to be performed in the xyY domain. The scaling should stretch only the luminance (Y) values. You are asked to apply linear scaling that would map the smallest Y value in the specified window and all values below it to 0, and the largest Y value in the
specified window and all values above it to 1.
