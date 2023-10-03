#include "NumCal.h"


int a[4] = { 4, 3, 2, 1 };

NumCal::NumCal()
{

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

Mat NumCal::NumberRecognition(TemplateNumDetector& templateNumDetector ,FrameNumDetector& frameNumDetector,  Mat Thresh, int thresh)
{
    Mat result = Thresh.clone();
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
			cout << "第" << i + 1 << "个轮廓数字识别为:" << a[idx] << endl;
			Rect rect = boundingRect(frameNumDetector.frameContours[frameNumDetector.frameNumNorm[i].Index]);

			rectangle(result, rect, Scalar(255,0,0), 2, 8);
			putText(result, to_string(a[idx]), Point(rect.x, rect.y-10), 1, 2, Scalar(0,0,255));
		}
		else {
			cout << "第" << i + 1 << "个轮廓数字识别为: 空" << endl;
		}
	}
	return result;
}