#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


/////////////////  Project 1 - Virtual Painter //////////////////////

Mat img;
VideoCapture cap(0);
vector<vector<int>> newPoints;  // to store all points   which is drawn in output window

//     -------------------  COLOR VALUES -------------------------------/// if we want to add more colors just add its values in this two vectors.
						                                         // hmin, smin, vmin hmax, smax, vmax
vector<vector<int>> myColors{ {158, 144, 128, 179, 229, 212},     // Pink //    
							  { 72,113,75,96,255,198 } };         // Green
vector<Scalar> myColorValues{ {255, 0, 255},		             // Purple the color which we want to output when above colors are detected.
							  {0,255,0} };                       // Green 	the color which we want to output when above colors are detected.


Point getContours(Mat image) {


	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);
	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;

		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);                 //boundary rectangle
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;     //tip of marker  and its middle of two corners  
			myPoint.y = boundRect[i].y;                              //y already have that point from x  

			//drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
			//rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
		}
	}
	return myPoint;                        //this the point where we need to draw circle in output
}


vector<vector<int>> findColor(Mat img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++)
	{
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);        //in 18,19th lines we have points
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);        //{158, 144, 128, 179, 229, 212} - {0, 1, 2, 3, 4, 5}indexes fromat 
		Mat mask;                                                           //mask will be diff for both  - mask means window for diff color
		inRange(imgHSV, lower, upper, mask);
		//imshow(to_string(i), mask);
		Point myPoint = getContours(mask);
		if (myPoint.x != 0) {                                           //if finds point thenonly it prints circles/dots
			newPoints.push_back({ myPoint.x,myPoint.y,i });
		}
	}
	return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)        //parameters decclared in starting.... need to recognise and draw color
{

	for (int i = 0; i < newPoints.size(); i++)
	{
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);   //(index 0 - myPoint.x in 76th line, index 1 - myPoint.y in 76th line, 10- radius of circle/dot, index 2 -i value
	}
}


void main() {

	while (true) {

		cap.read(img);

		newPoints = findColor(img);
		drawOnCanvas(newPoints, myColorValues);         // this is fun which draws the circles/dots in output acc to marker color

		imshow("Image", img);
		waitKey(1);
	}
}