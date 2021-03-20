#include "TemplateManager.h"

Mat TemplateManager::FindTemplate(int, void*,Mat image, Mat templ) {

	matchTemplate(image, templ, result, match_method);


	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}
	//rectangle(image, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	RECT rect = { matchLoc.x, matchLoc.y, matchLoc.x+50, matchLoc.y+50 };
	FillRect(hDC_Desktop, &rect, blueBrush);
	cout << matchLoc.x << endl;
	//imshow(image_window, img_display);
	//imshow(result_window, result);
	return result;
}

cuda::GpuMat TemplateManager::FindTemplate_gpu(int, void*, cuda::GpuMat image, cuda::GpuMat templ) {


	b->match(image, templ, result_cuda);


	//cuda::normalize(result_cuda, result_cuda, 0, 1, NORM_MINMAX, -1, cuda::GpuMat());
	cuda::minMaxLoc(result_cuda, &minVal, &maxVal, &minLoc, &maxLoc, cuda::GpuMat());

	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}
	//rectangle(image, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//if (matchLoc == lastMatch) return result_cuda;
	float x_plus = (matchLoc.x - lastMatch.x)*5;
	float y_plus = (matchLoc.y - lastMatch.y)*5;

	RECT rect = { matchLoc.x + x_plus, matchLoc.y + y_plus +150	, matchLoc.x + 50 + x_plus, matchLoc.y + 50 + 150  + y_plus };
	//RECT rect = { matchLoc.x, matchLoc.y+150, matchLoc.x + 50 , matchLoc.y + 50+150 };
	FillRect(hDC_Desktop, &rect, blueBrush);
	lastMatch.x = matchLoc.x;
	lastMatch.y = matchLoc.y;
	mousePos.x = matchLoc.x+x_plus;
	mousePos.y = matchLoc.y+150+ y_plus;
	//cout << "x: " << lastMatch.x << "y: " << lastMatch.y << endl;
	//cout << "x: " << matchLoc.x << "y: " << matchLoc.y << endl;
	cout << "x: "<< x_plus << "y: "<< y_plus << endl;
	//imshow(image_window, img_display);
	//imshow(result_window, result);
	return result_cuda;
}

TemplateManager::constructor::constructor(cuda::GpuMat image, Mat templ) {
	result_cols = image.cols - templ.cols + 1;
	result_rows = image.rows - templ.rows + 1;
	match_method = 0;
	minVal = 0.f;
	maxVal =0.f;
	lastMatch = Point(0.f, 0.f);
	mousePos = Point(0.f, 0.f);

	hDC_Desktop = GetDC(0);
	blueBrush = CreateSolidBrush(RGB(0, 0, 0));

	result.create(result_rows, result_cols, CV_32FC1);
	result_cuda.create(result_rows, result_cols, CV_8UC1);
	cout << result_rows << endl << result_cols << endl;

	b = cuda::createTemplateMatching(CV_8UC1, match_method);

}

void TemplateManager::SetMousePosition() {
	SetCursorPos(mousePos.x, mousePos.y);
}