#include "stdafx.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


#define GetY(R, G, B) (unsigned char)(+0.2990 * R + 0.5870 * G + 0.1140 * B + 0)
#define GetU(R, G, B) (unsigned char)(-0.1687 * R - 0.3313 * G + 0.5000 * B + 128)
#define GetV(R, G, B) (unsigned char)(+0.5000 * R - 0.4187 * G - 0.0813 * B + 128)


void rgb2yuv422Planar(const unsigned char *rgbData, int width, int height, int widthstep, unsigned char*yuvData)
{
	unsigned char temp = 0;
	unsigned char *lineUV = (unsigned char *)malloc(width * sizeof(unsigned char));

	if (NULL == lineUV)
	{
		perror("malloc lineUV memory failed!");
		exit(1);
	}

	memset(lineUV, 0, sizeof(width * sizeof(unsigned char)));
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++) //y  
		{
			unsigned char b = rgbData[i * widthstep + 3 * j];
			unsigned char g = rgbData[i * widthstep + 3 * j + 1];
			unsigned char r = rgbData[i * widthstep + 3 * j + 2];
			unsigned char *y = yuvData + i * width + j;
			temp = (int)GetY(r, g, b);

			*y = (temp < 0) ? 0 : ((temp > 255) ? 255 : temp);
			if (0 == (j % 2))
			{
				temp = (int)GetU(r, g, b);
				lineUV[j] = (temp < 0) ? 0 : ((temp > 255) ? 255 : temp);

			}
			else
			{
				temp = (int)GetV(r, g, b);
				lineUV[j] = (temp < 0) ? 0 : ((temp > 255) ? 255 : temp);
			}
		}

		for (int j = 0; j < width; j++) //u,v  
		{
			if (0 == (j % 2)) //u  
			{
				unsigned char *u = (yuvData + (height * width) + (i * width / 2) + (j / 2));
				*u = lineUV[j];
			}
			else //v  
			{
				unsigned char *v = (yuvData + (height * width * 3 / 2) + (i * width / 2) + (int)(j / 2));
				*v = lineUV[j];
			}
		}
	}

	if (lineUV != NULL)
	{
		free(lineUV);
		lineUV = NULL;
	}

}


void rgb2yuv422Packed(const unsigned char *rgbData, int width, int height, int widthstep, unsigned char *yuvData)
{
	int index = 0;
	for (int y = 0; y < height; y++)
	{
		bool bsetU = true;
		for (int x = 0; x < width; x++)
		{
			unsigned char b = rgbData[y * widthstep + 3 * x + 0];
			unsigned char g = rgbData[y * widthstep + 3 * x + 1];
			unsigned char r = rgbData[y * widthstep + 3 * x + 2];
			yuvData[index++] = GetY(r, g, b);
			yuvData[index++] = (bsetU ? GetU(r, g, b) : GetV(r, g, b));
			bsetU = !bsetU;
		}
	}
}


void convertYUV(string img_path, string yuv_path)
{
	Mat mat_src = cv::imread(img_path, CV_LOAD_IMAGE_COLOR);
	IplImage *pSrc = (IplImage *)&IplImage(mat_src);
	if (pSrc != NULL)
	{
		int iSize = pSrc->width * pSrc->height * sizeof(char) * 3;
		unsigned char *yuvData = new unsigned char[iSize];
		memset(yuvData, 0, iSize);
		rgb2yuv422Planar((const unsigned char*)pSrc->imageData, pSrc->width, pSrc->height, pSrc->widthStep, yuvData);

		fstream f;
		f.open(yuv_path, ios::out);

		f.write((const char*)yuvData, iSize);
		f.close();
		cout << "YUV file has been saved!" << endl;
	}
}