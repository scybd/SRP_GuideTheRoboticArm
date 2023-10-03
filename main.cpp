#include <opencv2/opencv.hpp>
#include <iostream>
#include "NumDetector.h"
#include "NumCal.h"
using namespace cv;
using namespace std; 

 
int main()
{
	Mat frame = imread("/home/thy/Documents/SRP_GuideTheRoboticArm/test.jpg");
	Mat numTemplate = imread("/home/thy/Documents/SRP_GuideTheRoboticArm/template.png", 0);
	Mat show = frame.clone();
	
	// 模板数字检测类的实例化
	TemplateNumDetector templateNumDetector(numTemplate);
	vector<Mat> sub_t_size = templateNumDetector.allInOne();
	
	//　相机图片数字检测类的实例化
	FrameNumDetector frameNumDetector(frame);
	vector<NumObject> sub_size = frameNumDetector.allInOne();
	vector<vector<Point>> frameContours = frameNumDetector.t();


	NumCal numcal;
	Mat result = numcal.NumberRecognition(templateNumDetector, frameNumDetector, show, 400);
 
	// imshow("threshold", frameBin);
	imshow("bin", result);
	waitKey(0);
	return 0;
}
