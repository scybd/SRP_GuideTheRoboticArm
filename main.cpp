#include <opencv2/opencv.hpp>
#include <iostream>
#include "NumDetector.h"
#include "NumCal.h"
using namespace cv;
using namespace std; 


int main()
{
	Mat numTemplate = imread("/home/thy/Documents/SRP_GuideTheRoboticArm/template.png", 0);
	//　模板数字检测类的实例化
	TemplateNumDetector* templateNumDetector = new TemplateNumDetector(numTemplate);
	templateNumDetector->allInOne();
	// vector<Mat> sub_t_size = templateNumDetector->allInOne();

	VideoCapture video("/home/thy/Documents/SRP_GuideTheRoboticArm/2222.avi");
	// 定义输出视频流 
    // VideoWriter out("out.mp4", VideoWriter::fourcc('X', '2', '6', '4'), 30, Size(640, 480)); 

	// 相机图片数字检测类的实例化
	FrameNumDetector* frameNumDetector = new FrameNumDetector();

	//　数字匹配计算类的实例化
	NumCal numcal;
	
	while(1)
	{
		Mat frame;
		bool isRead = video.read(frame);
		if(!isRead) {break;}

		Mat show = frame.clone();

		// 将每一帧图片装载进数字检测对象
		
		frameNumDetector->setFrame(frame);
		frameNumDetector->allInOne();
		// vector<NumObject> sub_size = frameNumDetector->allInOne();

		
		Mat result = numcal.numberRecognition(*templateNumDetector, *frameNumDetector, show, 400);
		result = numcal.calPos(result);

		// out.write(result); 		// 写入一帧到输出视频文件中
		imshow("res", result);

		if(waitKey(30) == 'q') {break;}

	}
	
	delete frameNumDetector;
	delete templateNumDetector;
	destroyAllWindows();
	video.release();
	return 0;
}
