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

    // 计算两幅归一化图像之间的不同像素的个数
    int calcDiff(Mat src1, Mat src2);

    // 绘制出最符合的匹配结果
    Mat NumberRecognition(TemplateNumDetector& templateNumDetector, FrameNumDetector& frameNumDetector,  Mat Thresh, int thresh);
};

#endif
