#include "NumDetector.h"

TemplateNumDetector::TemplateNumDetector(Mat img)
{
    templateImg = img;
}

TemplateNumDetector::~TemplateNumDetector() {}

void TemplateNumDetector::preprocess()
{
    // 将模板图片转为二值图
	threshold(templateImg, templateImg, 200, 255, THRESH_BINARY_INV);

    // 寻找轮廓
	findContours(templateImg, templateContours, templateHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
}

void TemplateNumDetector::getNum()
{
	if (!templateContours.empty() && !templateHierarchy.empty())
	{
		// 遍历所有轮廓
		for (int Index=0; Index<templateHierarchy.size(); Index++)
		{
			Rect tempRect = boundingRect(Mat(templateContours[Index]));
			// cout << tempRect.width << "--" << tempRect.height << endl;
			// 轮廓满足长宽比要求（数字外的方框的长宽比）
			if (tempRect.width>0.6*tempRect.height && tempRect.width<0.9*tempRect.height)
			{
				// 如果该轮廓有子轮廓且其子轮廓也有子轮廓，本项目中还可以加上有父轮廓这一条件
				if (templateHierarchy[Index][2] != -1 && templateHierarchy[Index+1][2] != -1)
				{
					Rect rect1 = boundingRect(Mat(templateContours[Index+1]));
					Rect rect2 = boundingRect(Mat(templateContours[Index+2]));
					// 子轮廓的长宽比要求和子轮廓的子轮廓的特殊情况排除（４）
					if (tempRect.width>0.6*tempRect.height && tempRect.width<0.9*tempRect.height && 
						rect2.width != 79 && rect2.height != 113)
					{
						Mat temp = templateImg(rect2).clone();						// 提取数字
						templateNum.push_back(temp);
					}
				}
			}	
		}
	}
}

void TemplateNumDetector::getNormNum()
{
	for (auto it = templateNum.begin(); it != templateNum.end(); ++it)
	{
		Mat temp;
		resize(*it,temp,Size(48,48));
		templateNumNorm.push_back(temp);
	}
}

vector<Mat> TemplateNumDetector::allInOne()
{
    preprocess();
    getNum();
    getNormNum();
    return templateNumNorm;
}


FrameNumDetector::FrameNumDetector()
{
    
}

FrameNumDetector::~FrameNumDetector() {}

void FrameNumDetector::setFrame(Mat frame)
{
	frameContours.clear();
	frameHierarchy.clear();
	frameNum.clear();
	frameNumNorm.clear();
	this->frame = frame;
}

void FrameNumDetector::preprocess()
{
    // 相机图像预处理
	cvtColor(frame, frame, COLOR_BGR2HSV);
	inRange(frame, Scalar(0, 0, 0), Scalar(179, 255, 180), frame);
	Mat element1 = getStructuringElement(MORPH_RECT, Size(1, 1));	
  	erode(frame, frame, element1);
	Mat element2 = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(frame, frame, element2);

    // 寻找轮廓		
	findContours(frame, frameContours, frameHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

}

void FrameNumDetector::getNum()
{
	if (!frameContours.empty() && !frameHierarchy.empty())
	{
		// 遍历所有轮廓
		for (int Index=0; Index<frameHierarchy.size(); Index++)
		{
			Rect tempRect = boundingRect(Mat(frameContours[Index]));
			// cout << tempRect.width << "--" << tempRect.height << endl;
			// 轮廓满足长宽比要求（数字外的方框的长宽比）
			if (tempRect.width>0.6*tempRect.height && tempRect.width<0.9*tempRect.height)
			{
				// 如果该轮廓有子轮廓且其子轮廓也有子轮廓
				if (frameHierarchy[Index][2] != -1 && frameHierarchy[Index+1][2] != -1)
				{
					// drawContours(frame, frameContours, Index, Scalar(255, 0,0), 1);
					Rect rect1 = boundingRect(Mat(frameContours[Index+1]));
					Rect rect2 = boundingRect(Mat(frameContours[Index+2]));
					// 子轮廓的长宽比要求和子轮廓的子轮廓的特殊情况排除（４）
					if (tempRect.width>0.6*tempRect.height && tempRect.width<0.9*tempRect.height && 
						rect2.width != 79 && rect2.height != 113)
					{
						NumObject temp;
						temp.img = frame(rect2).clone();						// 提取数字
						temp.Index = Index;
						frameNum.push_back(temp);
					}
				}
			}
		}
	}
}

void FrameNumDetector::getNormNum()
{
	for (auto it = frameNum.begin(); it != frameNum.end(); ++it)
	{
		NumObject temp;
		resize((*it).img,temp.img,Size(48,48));
		temp.Index = it->Index;
		frameNumNorm.push_back(temp);
	}
}

vector<NumObject> FrameNumDetector::allInOne()
{
    preprocess();
    getNum();
    getNormNum();
    return frameNumNorm;
}