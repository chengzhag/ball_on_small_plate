#include "my_opencv.h"

void verticalProject(const cv::Mat& src, std::vector<unsigned long>& dst)
{
	CV_Assert(src.depth() != sizeof(unsigned char));
	int i, j;
	const unsigned char* p;
	dst.resize(src.cols);
	for (j = 0; j < src.cols; j++) {
		dst[j] = 0;
		for (i = 0; i < src.rows; i++) {
			p = src.ptr<unsigned char>(i);
			dst[j] += p[j];
		}
	}
}