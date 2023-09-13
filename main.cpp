#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <time.h>
using namespace cv;
using namespace std; 
 
vector<cv::Mat> UniformSize(vector<cv::Mat> in);
vector<cv::Mat> GetSubarea(cv::Mat src,vector<vector<Point>> contour, vector<Vec4i> hierarchy);
int calcDiff(cv::Mat src1, cv::Mat src2);
void Clear_MicroConnected_Areas(cv::Mat src, cv::Mat &dst, double min_area);
cv::Mat NumberRecognition(cv::Mat Thresh, vector<cv::Mat> sub_size, vector<cv::Mat> sub_t_size, vector<vector<Point>> contour, int thresh);
 
int a[10] = { 7,0,9,8,6,5,4,3,2,1 };
 
int main()
{
	Mat src = imread("/home/thy/Documents/SRP_GuideTheRoboticArm/test.png");
	Mat Template_1 = imread("/home/thy/Documents/SRP_GuideTheRoboticArm/test.png", 0);
	
	// 模板二值化
	Mat templateThre;
	threshold(Template_1, templateThre, 200, 255, THRESH_BINARY_INV);
 
	// 寻找轮廓
	vector<vector<Point>> templateContour;
	vector<Vec4i> templateHierarchy;
	findContours(templateThre, templateContour, templateHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);	
 
	// 分割子区域
	vector<Mat> sub_t = GetSubarea(templateThre, templateContour, templateHierarchy);
 
	// 子区域尺寸统一化
	vector<Mat> sub_t_size = UniformSize(sub_t);
 
	// // 数字识别（字典法比对）
	// int t = 400;
	// cv::Mat result = NumberRecognition(src, sub_size, sub_t_size, contour, t);
 
	imshow("threshold", templateThre);
	waitKey(0);
	return 0;
}


// 获取子区域集合
vector<Mat> GetSubarea(Mat src, vector<vector<Point>> contour, vector<Vec4i> hierarchy)
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
				// 如果该轮廓有子轮廓且其子轮廓也有子轮廓
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
		for (int i=0; i<result.size(); i++)
		{
			imshow("s"+to_string(i), result[i]);
		}
		waitKey(0);

	}
	return result;
}

 
// 统一尺寸
vector<cv::Mat> UniformSize(vector<cv::Mat> in)
{
	vector<cv::Mat> result;
	for (auto it = in.begin(); it != in.end(); ++it)
	{
		cv::Mat temp;
		resize(*it,temp,Size(48,48));
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
int calcDiff(cv::Mat src1, cv::Mat src2)
{
	CV_Assert(src1.size() == src2.size());
	cv::Mat dif;
	cv::absdiff(src1, src2, dif);
	Scalar Sum=cv::sum(dif);
	return int(Sum[0] / 255);
}

 
// 数字识别
cv::Mat NumberRecognition(cv::Mat Thresh, vector<cv::Mat> sub_size, vector<cv::Mat> sub_t_size, vector<vector<Point>> contour,int thresh)
{
	cv::Mat result = Thresh.clone();
	for (int i = 0; i < sub_size.size(); ++i)
	{
		int min = 999999;
		int idx = 0;
		for (int j = 0; j < sub_t_size.size(); ++j)
		{
			int d = calcDiff(sub_size[i], sub_t_size[j]);
			if (d < min)
			{
				min = d;
				idx = j;
			}
		}
		if (min < thresh)
		{
			cout << "第" << i + 1 << "个轮廓数字识别为:" << a[idx] << endl;
			cv::Rect rect = cv::boundingRect(contour[i]);
			rectangle(result, rect, Scalar(255,0,0), 1, 8);
			putText(result, to_string(a[idx]), Point(rect.x, rect.y - 20), 1, 2, Scalar(0,0,255));
		}
		else {
			cout << "第" << i + 1 << "个轮廓数字识别为: 空" << endl;
		}
	}
	return result;
}