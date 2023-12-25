#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include "NumDetector.h"
#include "NumCal.h"
#include "Uart.h"
using namespace cv;
using namespace std; 

int serialPort;
int main()
{
	Mat numTemplate = imread("/home/thy/Documents/SRP_GuideTheRoboticArm/template.png", 0);
	//　模板数字检测类的实例化
	TemplateNumDetector* templateNumDetector = new TemplateNumDetector(numTemplate);
	templateNumDetector->allInOne();
	// vector<Mat> sub_t_size = templateNumDetector->allInOne();

	// 相机图片数字检测类的实例化
	FrameNumDetector* frameNumDetector = new FrameNumDetector();

	// 数字匹配计算类的实例化
	NumCal numcal;

	// 图片来源
	// VideoCapture cap(0);
	// cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    // cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    // cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	VideoCapture cap("/home/thy/Documents/SRP_GuideTheRoboticArm/2222.avi");
	// 定义输出视频流 
	// VideoWriter out("out.mp4", VideoWriter::fourcc('X', '2', '6', '4'), 30, Size(640, 480));


	if (uartInit(serialPort)) {cout << "串口初始化成功" << endl;}
	else {cout << "串口初始化失败" << endl;}

	// 计时变量
	clock_t start,end;
	while(1)
	{
		start = clock();

		Mat frame;
		bool isRead = cap.read(frame);
		if(!isRead) {break;}

		Mat show = frame.clone();

		// 将每一帧图片装载进数字检测对象
		
		frameNumDetector->setFrame(frame);
		frameNumDetector->allInOne();
		// vector<NumObject> sub_size = frameNumDetector->allInOne();

		
		Mat result = numcal.numberRecognition(*templateNumDetector, *frameNumDetector, show, 400);
		result = numcal.calPos(result);

		end = clock();
		short funTime = (end-start)*1000/CLOCKS_PER_SEC;
		putText(result, to_string(funTime)+"ms", Point(500, 440), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
		

		// out.write(result); 		// 写入一帧到输出视频文件中
		imshow("res", result);

		if(waitKey(1) == 'q') {break;}

	}
	
	delete frameNumDetector;
	delete templateNumDetector;
	destroyAllWindows();
	cap.release();
	return 0;
}
