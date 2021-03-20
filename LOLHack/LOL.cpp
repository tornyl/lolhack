#include <iostream>
#include "ScreenCapture.h"
//#include <opencv2/opencv.hpp>
//#include "opencv2/imgcodecs.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
#include <string>
#include "ScreenShooter.h"
#include "TemplateManager.h"
//#include "opencv2/cudaimgproc.hpp"
//#include <opencv2/core/cuda.hpp>
using namespace cv;
using namespace std;


bool use_mask;
Mat img; Mat templ; Mat mask; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";
int match_method=1;
int max_Trackbar = 5;

//Mat IMAGE;
//Mat TEMPL;

void MatchingMethod(int, void*)
{
	Mat img_display;
	img.copyTo(img_display);
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);
	bool method_accepts_mask = (TM_SQDIFF == match_method || match_method == TM_CCORR_NORMED);
	if (use_mask && method_accepts_mask)
	{
		matchTemplate(img, templ, result, match_method, mask);
	}
	else
	{
		matchTemplate(img, templ, result, match_method);
	}
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}
	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//imshow(image_window, img_display);
	//imshow(result_window, result);
	return;
}

void FindTemplate() {
	//namedWindow(image_window, WINDOW_AUTOSIZE);
	//namedWindow(result_window, WINDOW_AUTOSIZE);
	//const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	//createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);
	MatchingMethod(0, 0);
	//waitKey(0);

}

BITMAPINFOHEADER createBitmapHeader(int width, int height)
{
	BITMAPINFOHEADER  bi;

	// create a bitmap
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	return bi;
}

Mat captureScreenMat(HWND hwnd)
{
	Mat src;

	// get handles to a device context (DC)
	HDC hwindowDC = GetDC(hwnd);
	HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	// define scale, height and width
	int screenx = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int screeny = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	// create mat object
	src.create(height, width, CV_8UC4);

	// create a bitmap
	HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	BITMAPINFOHEADER bi = createBitmapHeader(width, height);

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);

	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);            //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

cuda::GpuMat IMAGE = ScreenShooter::TakeScreenShotV2();
Mat TEMPL_ = imread("D:/Programing/projects/cANDc++/LOLHack/Resources/templates/darius.png", IMREAD_UNCHANGED);
Mat experimental_ = imread("D:/Programing/projects/cANDc++/LOLHack/Resources/img2.png", IMREAD_UNCHANGED);
cuda::GpuMat TEMPL;
TemplateManager::constructor TemplateManager::cons(IMAGE, TEMPL_);
INPUT inputs[4];

int main(int argc, char** argv) {

	ZeroMemory(inputs, sizeof(inputs));
	ScreenCapture capo;
	if (capo.Init()) {
		cout << "wuba" << endl;
	}
	else {
		cout << "picicic" << endl;
	}

	// construct
	//cout << "cout: " << cuda::getCudaEnabledDeviceCount() << endl;

	//IMAGE.upload(IMAGE_);
	TEMPL.upload(TEMPL_);
	//cuda::cvtColor(TEMPL, TEMPL, COLOR_RGB2RGBA);

	//namedWindow(image_window, WINDOW_AUTOSIZE);
	//cvtColor(ScreenShooter::TakeScreenShot(), img)
	//cout << img.channels();
	//Mat imgo;
	//IMAGE.download(imgo);
	//imwrite("D:/Programing/projects/cANDc++/LOLHack/Resources/pipi.png", imgo);
	cout << IMAGE.channels() << endl;
	cout << TEMPL.channels() << endl;
	cuda::cvtColor(IMAGE, IMAGE, COLOR_RGBA2GRAY);
	cuda::cvtColor(TEMPL, TEMPL, COLOR_RGB2GRAY);
	cuda::threshold(TEMPL, TEMPL, 150, 255, THRESH_BINARY);

	//inRange(experimental, Scalar(0), Scalar())
	//inRange(experimental, Scalar(205), Scalar(255), experimental);
	//cuda::inRange(TEMPL, Scalar(150), Scalar(255), TEMPL);
	//cuda::GpuMat experimental;
	//experimental.upload(experimental_);
	//cuda::cvtColor(experimental, experimental, COLOR_RGB2GRAY);
	//cuda::threshold(experimental, experimental, 200, 255, THRESH_BINARY);
	//Mat templ_temp;
	//experimental.download(templ_temp);
	//imshow(image_window, templ_temp);
	//waitKey(0);
	//cuda::inRange()
	//cout << IMAGE.depth() << endl;
	//return 0;

	//return 0;
	if (IMAGE.empty() || TEMPL.empty() || (use_mask && mask.empty()))
	{
		cout << "Can't read one of the images" << endl;
		//return EXIT_FAILURE;
	}
	if (!IMAGE.empty()) {
		//imshow(image_window, IMAGE);
	}
	else {
		cout << "FKin" << endl;
	}
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wScan = MapVirtualKey(0x48, MAPVK_VK_TO_VSC);
	inputs[0].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wScan = MapVirtualKey(0x4A, MAPVK_VK_TO_VSC);
	inputs[1].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wScan = MapVirtualKey(0x4B, MAPVK_VK_TO_VSC);
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;

	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wScan = MapVirtualKey(0x4C, MAPVK_VK_TO_VSC);
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
	Mat tempImg;
	cuda::GpuMat old_img;
	//old_img.upload(Mat::zeros(IMAGE.cols, IMAGE.rows, CV_8UC1));
	//old_img = IMAGE;
	bool rangeQtoogle = false;
	while (true) {
		//Sleep(5000);
		//waitKey(1);
		//image.release();
		//cuda::cvtColor(ScreenShooter::TakeScreenShot(), IMAGE, COLOR_RGB2GRAY);
		//inputs[0].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		if (GetAsyncKeyState('Q') != 0) {
			rangeQtoogle = true;
		}
		else if (rangeQtoogle) {
			TemplateManager::SetMousePosition();
			inputs[0].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
			rangeQtoogle = false;
		}
		inputs[1].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		inputs[2].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		inputs[3].ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
		//if (GetAsyncKeyState('Q') & 0x8000)
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			//TemplateManager::SetMousePosition();
			// the 'A' key is currently being held down
			inputs[0].ki.dwFlags = KEYEVENTF_SCANCODE;
			SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
			rangeQtoogle = false;
		}
		if (GetAsyncKeyState('W') & 0x8000)
		{
			TemplateManager::SetMousePosition();
			// the 'A' key is currently being held down
			inputs[1].ki.dwFlags = KEYEVENTF_SCANCODE;
			SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
		}
		if (GetAsyncKeyState('E') & 0x8000)
		{
			TemplateManager::SetMousePosition();
			// the 'A' key is currently being held down
			inputs[2].ki.dwFlags = KEYEVENTF_SCANCODE;
			SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
		}
		if (GetAsyncKeyState('R') & 0x8000)
		{
			TemplateManager::SetMousePosition();
			// the 'A' key is currently being held down
			inputs[3].ki.dwFlags = KEYEVENTF_SCANCODE;
			SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
		}
		capo.CaptureScreen(tempImg);
		IMAGE.upload(tempImg);
		//Mat showimg;
		//IMAGE.download(showimg);
		cuda::cvtColor(IMAGE, IMAGE, COLOR_RGBA2GRAY);
		cuda::threshold(IMAGE, IMAGE, 200, 255, THRESH_BINARY);
		//cuda::GpuMat compared;
		//cuda::compare(old_img, IMAGE, compared, CMP_EQ);
		//int perc = cuda::countNonZero(compared);
		/*if (perc > 1) {
			old_img = IMAGE;
			cout << "not same" << endl;
		}
		else {
			cout << "same" << endl;*/
		//}
		//cvtColor(tempImg, tempImg, COLOR_RGBA2GRAY);
		//IMAGE = ScreenShooter::TakeScreenShotV2();
		IMAGE = TemplateManager::FindTemplate_gpu(0,0, IMAGE, TEMPL);
		//img = imread("D:/Programing/projects/cANDc++/LOLHack/Resources/img2.png");
		//FindTemplate();
		//imshow(image_window, showimg);
		//waitKey(0);
	}

	ScreenShooter::Destroy();

	//for (int i = 0; i < 100; i++) {
	//	HDC hdc = GetDC(NULL);
	//	MoveToEx(hdc, 0, 0, NULL);
	//	LineTo(hdc, 1000, 1000);
	//	ReleaseDC(NULL, hdc);
	//	//Sleep(100);
	//}
	//waitKey(0);

	/*if (argc < 3)
	{
		cout << "Not enough parameters" << endl;
		cout << "Usage:\n" << argv[0] << " <image_name> <template_name> [<mask_name>]" << endl;
		return -1;
	}

	img = imread(argv[1], IMREAD_COLOR);
	templ = imread(argv[2], IMREAD_COLOR);
	if (argc > 3) {
		use_mask = true;
		mask = imread(argv[3], IMREAD_COLOR);
	}

	if (img.empty() || templ.empty() || (use_mask && mask.empty()))
	{
		cout << "Can't read one of the images" << endl;
		return EXIT_FAILURE;
	}

	FindTemplate();*/

	//TESTCODE

	//Mat image = imread("D:/Download/fighting-gpi-skill.jpg");

	//if (image.empty()) // Check for failure
	//{
	//	cout << "Could not open or find the image" << endl;
	//	system("pause"); //wait for any key press
	//	return -1;
	//}

	//String windowName = "My HelloWorld Window"; //Name of the window

	//namedWindow(windowName); // Create a window

	//imshow(windowName, image); // Show our image inside the created window.

	//waitKey(0); // Wait for any keystroke in the window

	//destroyWindow(windowName); //destroy the created window

	//TESTCODE END

	//TestCode 2

	/*HWND hwnd = GetDesktopWindow();
	Mat src = captureScreenMat(hwnd);

	String windowName = "My HelloWorld Window";

	namedWindow(windowName);

	imshow(windowName, src);
	waitKey(0);

	destroyWindow(windowName);

	std::cout << "hello" << std::endl;*/

	// TESTCODE 2 END
	return 0;
}
