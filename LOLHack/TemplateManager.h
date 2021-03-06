#pragma once
//#include <opencv2/opencv.hpp>
#include <iostream>
#include <Windows.h>
#include "opencv2/cudaimgproc.hpp"
#include <opencv2/core/cuda.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#undef min
#undef max
#include <opencv2/cudaarithm.hpp>

using namespace cv;
using namespace std;

class TemplateManager {
private:
	static inline int result_cols;
	static inline int result_rows;
	static inline int match_method;
	static inline Mat result;
	static inline cuda::GpuMat result_cuda;
	static inline double minVal;
	static inline double maxVal;
	static inline Point minLoc;
	static inline Point maxLoc;
	static inline Point matchLoc;
	static inline Point lastMatch;
	static inline Point mousePos;
	static inline HDC hDC_Desktop;
	static inline HBRUSH blueBrush;
	static inline Ptr < cuda::TemplateMatching> b;
public:


	static struct constructor {
	public:
		//friend class TemplateManager;
		constructor(cuda::GpuMat image, Mat templ);
	} cons;


	//static constructor cons;

	static Mat FindTemplate(int, void*, Mat image, Mat templ);
	static cuda::GpuMat FindTemplate_gpu(int, void*, cuda::GpuMat image, cuda::GpuMat templ);
	static void SetMousePosition();

};

//TemplateManager::constructor TemplateManager::cons(4);