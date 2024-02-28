#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include "NumDetector.h"
#include "NumCal.h"
#include "Uart.h"
#include <apriltag.h>
#include <tag36h11.h>
using namespace cv;
using namespace std; 


int serialPort;
const int packetLength = 9; 		// 数据包长度
unsigned char buf[packetLength];	// 接收数据
bool startSendPosErr = false;		// 是否开始发送偏差数据（放置货物的对准阶段）


float code_halfW = 1.75;			// aprilTag的真实半宽度(cm)
float code_halfH = 1.75;			// aprilTag的真实半高度(cm)
Mat cam_mat_code = (Mat_<double>(3, 3) << 366.7, 0.0, 292.4, 0.0, 367, 256.4, 0.0, 0.0, 1.0); // 内参矩阵
Mat dis_coeff_code = (Mat_<double>(4, 1) << 0.0767, -0.0722, 0.0, 0.0);               // 畸变矩阵
Mat r_vector_code = Mat::zeros(3, 1, CV_64FC1);  //旋转向量初始化
Mat t_vector_code = Mat::zeros(3, 1, CV_64FC1);  //平移向量初始化
vector<Point3f> code3d;      // aprilTag的世界坐标系坐标，以其中心为原点
vector<Point2f> code2d;      // aprilTag的图像坐标系坐标

bool pos_flag = false;      // 发送目标坐标的标志位，在单片机给我发信息后置位
bool id_flag = false;      	// 发送目标类型的标志位，在单片机给我发信息后置位
int frame_count = 0;		// 统计帧数，若干帧未识别到则发0（无货物）


int main()
{
	// 初始化 AprilTag 检测器
    apriltag_family_t *tf = tag36h11_create();
    apriltag_detector_t *td = apriltag_detector_create();
    apriltag_detector_add_family(td, tf);

	// 
    code3d.push_back(Point3f(-code_halfW, code_halfH, 0));
    code3d.push_back(Point3f(code_halfW, code_halfH, 0));
    code3d.push_back(Point3f(code_halfW, -code_halfH, 0));
    code3d.push_back(Point3f(-code_halfW, -code_halfH, 0));

	
	// 读取模板图片
	Mat numTemplate = imread("/home/thy/Documents/SRP_GuideTheRoboticArm/template.png", 0);
	//　模板数字检测类的实例化
	TemplateNumDetector* templateNumDetector = new TemplateNumDetector(numTemplate);
	templateNumDetector->allInOne();
	
	// 相机图片数字检测类的实例化
	FrameNumDetector* frameNumDetector = new FrameNumDetector();

	// 数字匹配计算类的实例化
	NumCal numcal;

	// 图像流来源，摄像头或视频文件
	// VideoCapture cap(0);
	// cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    // cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    // cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	VideoCapture cap("/home/thy/Documents/SRP_GuideTheRoboticArm/test.mp4");

	// 定义输出视频流 
	// VideoWriter out("out.mp4", VideoWriter::fourcc('X', '2', '6', '4'), 30, Size(640, 480));

	if (uartInit(serialPort)) {cout << "串口初始化成功" << endl;}
	else {cout << "串口初始化失败" << endl;}

	// 计时变量
	clock_t start,end;
	while(1)
	{
		start = clock();
		int recivedNums = read(serialPort, buf, packetLength); 		// 读取串口数据

		// 判断串口传来的数据包合法性及内容
		if (recivedNums == 9 && buf[0] == 0xff && buf[8] == 0xfe)
		{
			if (buf[1] == 0x01) 
			{
				cout << "开始发送数字偏差" << endl;
				startSendPosErr = true;
			}
			else if (buf[1] == 0x02)
			{
				cout << "停止发送数字偏差" << endl;
				startSendPosErr = false;
			}
			else if (buf[1] == 0x03) 
			{
				cout << "发送物体坐标" << endl;
				pos_flag = true;
            }
            else if (buf[1] == 0x04)
            {
                cout << "发送物体类型" << endl;
                id_flag = true;
            }
		}

	
		Mat frame, frame_gray;
		bool isRead = cap.read(frame);
		if(!isRead) {break;}
		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);	// 
		// out.write(frame); 		// 写入一帧到输出视频文件中
		
		Mat show = frame.clone();
		Mat test;
		// 将每一帧图片装载进数字检测对象
		frameNumDetector->setFrame(frame);
		frameNumDetector->allInOne(test);
			
		// 模板匹配
		Mat result = numcal.numberRecognition(*templateNumDetector, *frameNumDetector, show, 1000);
		if (startSendPosErr) {result = numcal.NumXY(result);}


		end = clock();
		short funTime = (end-start)*1000/CLOCKS_PER_SEC;
		putText(result, "TM:"+to_string(funTime)+"ms", Point(500, 440), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);
		

		// 以下是apriltag检测代码
        image_u8_t im = {.width = frame_gray.cols, .height = frame_gray.rows, .stride = frame_gray.cols, .buf = frame_gray.data};
        zarray_t *detections = apriltag_detector_detect(td, &im);
		// 处理检测结果
        for (int i = 0; i < zarray_size(detections); i++) {
            apriltag_detection_t *det;
            zarray_get(detections, i, &det);

            // 计算角度
            code2d.clear();
			code2d.push_back(Point2d(det->p[3][0],det->p[3][1]));
			code2d.push_back(Point2d(det->p[2][0],det->p[2][1]));
			code2d.push_back(Point2d(det->p[1][0],det->p[1][1]));
			code2d.push_back(Point2d(det->p[0][0],det->p[0][1]));
            solvePnP(code3d, code2d, cam_mat_code, dis_coeff_code, r_vector_code, t_vector_code);
            Mat rotM;
            Rodrigues(r_vector_code, rotM);
            double theta_z = atan2(rotM.at<double>(1, 0), rotM.at<double>(0, 0));
            theta_z = theta_z * (180 / CV_PI);
            int tx = (int)(t_vector_code.at<double>(0, 0) * 10);
		    int ty = (int)(t_vector_code.at<double>(1, 0) * 10);

            putText(result, to_string(det->id), Point(det->c[0],det->c[1]), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0), 1);
            putText(result, to_string(theta_z), Point(0,30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0), 1);
            putText(result, "x: "+to_string(tx), Point(0,50), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0), 1);
            putText(result, "y: "+to_string(ty), Point(0,70), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0), 1);
            for (int i = 0; i < 4; ++i) 
            {
                putText(result, to_string(i), Point(det->p[i][0],det->p[i][1]), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 0), 2);
                int next = (i + 1) % 4;
                cv::line(result, Point(det->p[i][0],det->p[i][1]), Point(det->p[next][0],det->p[next][1]), cv::Scalar(0, 255, 0), 2);
            }
            
            if (pos_flag) {
                tx += 100;
                ty += 100;
                unsigned char theta;
                if(theta_z>=0 && theta_z<=90) theta = theta_z;
                else theta = 0;
                unsigned char crc = 0x03 + (unsigned char)tx + (unsigned char)ty + theta;
                unsigned char data[9] = {0xff,0x03,(unsigned char)tx,(unsigned char)ty,theta,0x00,0x00,crc,0xfe};
                sendData(serialPort, data, sizeof(data));
				pos_flag = false;
                cout << "sent pos! x="+to_string(tx)+" y="+to_string(ty)+" theta="+to_string(theta) << endl;
            }

            if (id_flag) {
                unsigned char id = det->id + 1;
                unsigned char crc = 0x02 + id;
                unsigned char data[9] = {0xff,0x02,id,0x00,0x00,0x00,0x00,crc,0xfe};
                sendData(serialPort, data, sizeof(data));
                cout << "sent id! id="+to_string(id) << endl;
				frame_count = 0;
				id_flag = false;
            }

            apriltag_detection_destroy(det);
        }

		imshow("res", result);
		imshow("test", test);

		if (id_flag)
			frame_count += 1;
		// 100帧未检测到aprilTag便发送没有物块的信息
		if (frame_count >= 100)
		{
			unsigned char data[9] = {0xff,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0xfe};
            sendData(serialPort, data, sizeof(data));
			cout << "没有物块" << endl;
			id_flag = false;
			frame_count = 0;
		}

		if(waitKey(100) == 'q') {break;}

	}
	
	delete frameNumDetector;
	delete templateNumDetector;
	destroyAllWindows();
	cap.release();
	return 0;
}
