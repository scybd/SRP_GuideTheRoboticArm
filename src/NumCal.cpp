#include "NumCal.h"
#include "Uart.h"

int a[4] = { 4, 3, 2, 1 };

float halfW = 2.5;		// 实际数字的边框半宽度
float halfH = 4.25;		// 实际数字的边框半高度
Mat cam_mat = (Mat_<double>(3, 3) << 366.7, 0.0, 292.4, 0.0, 367, 256.4, 0.0, 0.0, 1.0); // 内参矩阵
Mat dis_coeff = (Mat_<double>(4, 1) << 0.0767, -0.0722, 0.0, 0.0);               // 畸变矩阵
Mat r_vector = Mat::zeros(3, 1, CV_64FC1);  //旋转向量初始化
Mat t_vector = Mat::zeros(3, 1, CV_64FC1);  //平移向量初始化

extern int serialPort;


NumCal::NumCal()
{
	// 数字的3d点初始化
	num3d.push_back(Point3f(-halfW, halfH, 0));
	num3d.push_back(Point3f(halfW, halfH, 0));
	num3d.push_back(Point3f(halfW, -halfH, 0));
	num3d.push_back(Point3f(-halfW, -halfH, 0));

	doPos = false;
	doUart = false;
}


NumCal::~NumCal()
{

}


int NumCal::calcDiff(Mat src1, Mat src2)
{
    CV_Assert(src1.size() == src2.size());
	Mat dif;
	absdiff(src1, src2, dif);
	Scalar Sum = sum(dif);
	return int(Sum[0] / 255);
}


Mat NumCal::numberRecognition(TemplateNumDetector& templateNumDetector ,FrameNumDetector& frameNumDetector,  Mat img, int thresh)
{
	doPos = false;
	doUart = false;
    // Mat result = img.clone();
	for (int i = 0; i < frameNumDetector.frameNumNorm.size(); ++i)
	{
		int min = 999999;
		int idx = 0;
		for (int j = 0; j < templateNumDetector.templateNumNorm.size(); ++j)
		{
			int d = calcDiff(frameNumDetector.frameNumNorm[i].img, templateNumDetector.templateNumNorm[j]);
			if (d < min)
			{
				min = d;
				idx = j;
			}
		}
		if (min < thresh)
		{
			// cout << "第" << i + 1 << "个轮廓数字识别为:" << a[idx] << endl;

			// 找到匹配数字的外接矩形
			Rect rect = boundingRect(frameNumDetector.frameContours[frameNumDetector.frameNumNorm[i].Index]);
			num2d.clear();
			num2d.push_back(Point2d(rect.x, rect.y));
			num2d.push_back(Point2d(rect.x + rect.width, rect.y));
			num2d.push_back(Point2d(rect.x + rect.width, rect.y + rect.height));
			num2d.push_back(Point2d(rect.x, rect.y + rect.height));
			doPos = true;
			doUart = true;
			rectangle(img, rect, Scalar(255,0,0), 2, 8);
			putText(img, to_string(a[idx]), Point(rect.x, rect.y-10), 1, 2, Scalar(255,255,125), 2);
		}
		// else {
		// 	cout << "第" << i + 1 << "个轮廓数字识别为: 空" << endl;
		// }
	}
	return img;
}


// 对齐未使用这个
Mat NumCal::calPos(Mat img)
{
	if(doPos)
	{
		solvePnP(num3d, num2d, cam_mat, dis_coeff, r_vector, t_vector);
		double tx = t_vector.at<double>(0, 0);
		double ty = t_vector.at<double>(1, 0);
		double tz = t_vector.at<double>(2, 0);
		// 距离
		double dis = sqrt(tx * tx + ty * ty + tz * tz);
		// cout << tx << endl << ty << endl << tz << endl;
		putText(img, "x:"+to_string(tx)+"cm", Point(0,20), 1, 2, Scalar(20, 120, 20), 2);
		putText(img, "y:"+to_string(ty)+"cm", Point(0,40), 1, 2, Scalar(20, 120, 20), 2);
		putText(img, "z:"+to_string(tz)+"cm", Point(0,60), 1, 2, Scalar(20, 120, 20), 2);
		
	}
	else
	{
		// cout << "未检测到\n";
	}
	
	return img;
}


// 对齐只使用了这个
Mat NumCal::NumXY(Mat img)
{
	if (doUart) 
	{
		int num_x = (int)((num2d[0].x + num2d[1].x) / 2 - 320);
		if (num_x > 127) {num_x = 127;} else if (num_x < -128) {num_x = -128;}

		unsigned char pos_error = (char)num_x + 128;
		unsigned char crc = 0xff + 0x01 + pos_error;
		unsigned char data[9] = {0xff,0x01,pos_error,0x00,0x00,0x00,0x00,crc,0xfe};
		sendData(serialPort, data, sizeof(data));

		putText(img, "pos_err:"+to_string(pos_error)+"pix", Point(0,20), 1, 2, Scalar(20, 120, 20), 2);
		cout << "pos_err: " << (int)pos_error << endl;
	}
	return img;
}
