#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <time.h>
using namespace cv;
using namespace std; 


struct NumObject
{
	Mat img;
	int Index;		//　轮廓索引号
};

vector<Mat> templateUniformSize(vector<Mat> in);
vector<NumObject> frameUniformSize(vector<NumObject> in);
vector<Mat> templateGetNum(Mat src,vector<vector<Point>> contour, vector<Vec4i> hierarchy);
vector<NumObject> frameGetNum(Mat src, vector<vector<Point>> contour, vector<Vec4i> hierarchy);
int calcDiff(Mat src1, Mat src2);
Mat NumberRecognition(Mat Thresh, vector<NumObject> sub_size, vector<Mat> sub_t_size, vector<vector<Point>> contour, int thresh);
 
int a[4] = { 4, 3, 2, 1 };

 
int main()
{
	Mat frame = imread("/home/thy/Documents/SRP_GuideTheRoboticArm/test.jpg");
	Mat numTemplate = imread("/home/thy/Documents/SRP_GuideTheRoboticArm/template.png", 0);
	
	// 模板图片二值化
	Mat templateThre;
	threshold(numTemplate, templateThre, 200, 255, THRESH_BINARY_INV);

	// 相机图像预处理
	Mat frameHSV;
	cvtColor(frame, frameHSV, COLOR_BGR2HSV);
	Mat frameBin;
	inRange(frameHSV, Scalar(0, 0, 0), Scalar(179, 255, 108), frameBin);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(1, 1));	
  	erode(frameBin, frameBin, element1);
	Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(frameBin, frameBin, element2);
 
	// 寻找轮廓
	vector<vector<Point>> templateContours;
	vector<Vec4i> templateHierarchy;
	findContours(templateThre, templateContours, templateHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);	
	vector<vector<Point>> frameContours;
	vector<Vec4i> frameHierarchy;
	findContours(frameBin, frameContours, frameHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	// 分割子区域
	vector<Mat> sub_t = templateGetNum(templateThre, templateContours, templateHierarchy);
	vector<NumObject> sub = frameGetNum(frameBin, frameContours, frameHierarchy);

	// 子区域尺寸统一化
	vector<Mat> sub_t_size = templateUniformSize(sub_t);
	vector<NumObject> sub_size = frameUniformSize(sub);	
	
	// 数字识别（字典法比对）
	int t = 200;
	Mat result = NumberRecognition(frame, sub_size, sub_t_size, frameContours, t);
 
	// imshow("threshold", frameBin);
	imshow("bin", result);
	waitKey(0);
	return 0;
}


// 模板图片获取数字模板
vector<Mat> templateGetNum(Mat src, vector<vector<Point>> contour, vector<Vec4i> hierarchy)
{
	vector<Mat> result;
	if (!contour.empty() && !hierarchy.empty())
	{
		// 遍历所有轮廓
		for (int Index=0; Index<hierarchy.size(); Index++)
		{
			Rect tempRect = boundingRect(Mat(contour[Index]));
			// cout << tempRect.width << "--" << tempRect.height << endl;
			// 轮廓满足长宽比要求（数字外的方框的长宽比）
			if (tempRect.width>0.6*tempRect.height && tempRect.width<0.9*tempRect.height)
			{
				// 如果该轮廓有子轮廓且其子轮廓也有子轮廓，本项目中还可以加上有父轮廓这一条件
				if (hierarchy[Index][2] != -1 && hierarchy[Index+1][2] != -1)
				{
					Rect rect1 = boundingRect(Mat(contour[Index+1]));
					Rect rect2 = boundingRect(Mat(contour[Index+2]));
					// 子轮廓的长宽比要求和子轮廓的子轮廓的特殊情况排除（４）
					if (tempRect.width>0.6*tempRect.height && tempRect.width<0.9*tempRect.height && 
						rect2.width != 79 && rect2.height != 113)
					{
						Mat temp = src(rect2).clone();						// 提取数字
						result.push_back(temp);
					}
					
				}
			}
			
		}

		// 展示效果
		// for (int i=0; i<result.size(); i++)
		// {
		// 	imshow("s"+to_string(i), result[i]);
		// }
		// waitKey(0);

	}
	return result;
}


// 相机图片获取数字
vector<NumObject> frameGetNum(Mat src, vector<vector<Point>> contour, vector<Vec4i> hierarchy)
{
	vector<NumObject> result;
	if (!contour.empty() && !hierarchy.empty())
	{
		// 遍历所有轮廓
		for (int Index=0; Index<hierarchy.size(); Index++)
		{
			Rect tempRect = boundingRect(Mat(contour[Index]));
			// cout << tempRect.width << "--" << tempRect.height << endl;
			// 轮廓满足长宽比要求（数字外的方框的长宽比）
			if (tempRect.width>0.6*tempRect.height && tempRect.width<0.9*tempRect.height)
			{
				// 如果该轮廓有子轮廓且其子轮廓也有子轮廓
				if (hierarchy[Index][2] != -1 && hierarchy[Index+1][2] != -1)
				{
					// drawContours(frame, contour, Index, Scalar(255, 0,0), 1);
					Rect rect1 = boundingRect(Mat(contour[Index+1]));
					Rect rect2 = boundingRect(Mat(contour[Index+2]));
					// 子轮廓的长宽比要求和子轮廓的子轮廓的特殊情况排除（４）
					if (tempRect.width>0.6*tempRect.height && tempRect.width<0.9*tempRect.height && 
						rect2.width != 79 && rect2.height != 113)
					{
						NumObject temp;
						temp.img = src(rect2).clone();						// 提取数字
						temp.Index = Index;
						result.push_back(temp);
					}
					
				}
			}
			
		}
		// imshow("s", frame);
		// 展示效果
		// for (int i=0; i<result.size(); i++)
		// {
		// 	imshow("s"+to_string(i), result[i]);
		// }
		// waitKey(0);

	}
	return result;
}
 
// 模板统一尺寸
vector<Mat> templateUniformSize(vector<Mat> in)
{
	vector<cv::Mat> result;
	for (auto it = in.begin(); it != in.end(); ++it)
	{
		cv::Mat temp;
		resize(*it,temp,Size(48,48));
		result.push_back(temp);
	}
	return result;
}


// 相机图像中的数字统一尺寸
vector<NumObject> frameUniformSize(vector<NumObject> in)
{
	vector<NumObject> result;
	for (auto it = in.begin(); it != in.end(); ++it)
	{
		NumObject temp;
		Mat r;
		resize((*it).img,r,Size(48,48));
		temp.img = r;
		temp.Index = it->Index;
		result.push_back(temp);
	}

	//　展示效果
	// for (int i=0; i<result.size(); i++)
	// {
	// 	imshow("s"+to_string(i), result[i]);
	// }
	// waitKey(0);
	return result;
}

 
// 对比图像差值
int calcDiff(Mat src1, Mat src2)
{
	CV_Assert(src1.size() == src2.size());
	cv::Mat dif;
	cv::absdiff(src1, src2, dif);
	Scalar Sum=cv::sum(dif);
	return int(Sum[0] / 255);
}

 
// 数字识别
Mat NumberRecognition(Mat Thresh, vector<NumObject> sub_size, vector<Mat> sub_t_size, vector<vector<Point>> contour,int thresh)
{
	Mat result = Thresh.clone();
	for (int i = 0; i < sub_size.size(); ++i)
	{
		int min = 999999;
		int idx = 0;
		for (int j = 0; j < sub_t_size.size(); ++j)
		{
			int d = calcDiff(sub_size[i].img, sub_t_size[j]);
			if (d < min)
			{
				min = d;
				idx = j;
			}
		}
		if (min < thresh)
		{
			cout << "第" << i + 1 << "个轮廓数字识别为:" << a[idx] << endl;
			Rect rect = boundingRect(contour[sub_size[i].Index]);

			rectangle(result, rect, Scalar(255,0,0), 2, 8);
			putText(result, to_string(a[idx]), Point(rect.x, rect.y-10), 1, 2, Scalar(0,0,255));
		}
		else {
			cout << "第" << i + 1 << "个轮廓数字识别为: 空" << endl;
		}
	}
	return result;
}