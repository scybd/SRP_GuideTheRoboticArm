#ifndef __NUMCAL_H__
#define __NUMCAL_H__
#include <opencv2/opencv.hpp>
#include <iostream>
#include "NumDetector.h"

using namespace cv;
using namespace std;

// 此类用于计算目标数字的位姿
class NumCal
{
public:
    NumCal();
    ~NumCal();

    // 计算两幅归一化图像之间的不同像素的个数，供NumberRecognition函数使用
    static int calcDiff(Mat src1, Mat src2);

    // 绘制出最符合的匹配结果
    Mat numberRecognition(TemplateNumDetector& templateNumDetector, FrameNumDetector& frameNumDetector,  Mat img, int thresh);

    Mat calPos(Mat img);

    // 只输出目标数字的xy像素坐标，用于对准
    Mat NumXY(Mat img);

private:
    vector<Point3f> num3d;      // 数字的真实坐标，以其中心为原点
    vector<Point2f> num2d;      // 数字在图像中的坐标
    bool doPos;                 // 是否开启位姿解算的标志位
    bool doUart;                // 是否串口发送消息的标志位

};

#endif
