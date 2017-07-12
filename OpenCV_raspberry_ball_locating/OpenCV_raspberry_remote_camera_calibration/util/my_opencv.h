#pragma once
#include "opencv2/opencv.hpp"
#include <vector>

template<typename T>
	void plotSimple(const std::vector<T> src, cv::Mat& dst, unsigned int plotHeight = 256)
	{
		dst = cv::Mat(cv::Size(src.size(), plotHeight), CV_8UC1, cv::Scalar(0));
		T maxVal = *max_element(src.begin(), src.end());
		for (int i = 0; i < src.size(); i++)
		{
			unsigned int y;
			y = plotHeight - 1 - (float)src[i] * (plotHeight - 1) / maxVal;
			for (int j = y; j < plotHeight; j++)
			{
				dst.at<uchar>(j, i) = 255;
			}
		}
	}

void verticalProject(const cv::Mat& src, std::vector<unsigned long>& dst);