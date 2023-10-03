#ifndef __NUMDETECTOR_H__
#define __NUMDETECTOR_H__
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;


struct NumObject
{
	Mat img;
	int Index;		//　轮廓索引号
};


class TemplateNumDetector
{
public:
    TemplateNumDetector(Mat img);
    ~TemplateNumDetector();

    // 预处理
    void preprocess();

    // 获取其中数字
    void getNum();

    // 将提取出的数字归一化
    void getNormNum();

    // 综合以上三步，得出模板数字数组
    vector<Mat> allInOne();

    friend class NumCal;
private:
    Mat templateImg;    // 模板数字图片
    
    vector<vector<Point>> templateContours;     //　模板图片的轮廓
	vector<Vec4i> templateHierarchy;            //　模板图片的轮廓层级


    vector<Mat> templateNum;        //　目标数字的小图片
    vector<Mat> templateNumNorm;    //　归一化后的目标数字的小图片
};


class FrameNumDetector
{
public:
    FrameNumDetector(Mat img);
    ~FrameNumDetector();

    // 预处理
    void preprocess();

    // 获取其中可能的数字
    void getNum();

    // 将提取出的数字归一化
    void getNormNum();

    // 综合以上三步，得出待匹配的对象
    vector<NumObject> allInOne();
    vector<vector<Point>> t();

    friend class NumCal;
private:
    Mat frame;        // 相机图片
    
    vector<vector<Point>> frameContours;     //　相机图片的轮廓
	vector<Vec4i> frameHierarchy;            //　相机图片的轮廓层级


    vector<NumObject> frameNum;         //　目标数字的小图片
    vector<NumObject> frameNumNorm;     //　归一化后的目标数字的小图片
};
#endif
