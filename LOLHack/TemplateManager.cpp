#include "TemplateManager.h"

Mat TemplateManager::FindTemplate(int, void*,Mat image, Mat templ) {

	matchTemplate(image, templ, result, match_method);


	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc[0] = minLoc;
	}
	else
	{
		matchLoc[0] = maxLoc;
	}
	//rectangle(image, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	RECT rect = { matchLoc[0].x, matchLoc[0].y, matchLoc[0].x+50, matchLoc[0].y+50 };
	FillRect(hDC_Desktop, &rect, blueBrush);
	//cout << matchLoc[0].x << endl;
	//imshow(image_window, img_display);
	//imshow(result_window, result);
	return result;
}

void TemplateManager::FindTemplate_gpu(int, void*, cuda::GpuMat &image, cuda::GpuMat &templ, int index) {


	/*if (index == 0) {
		b[0]->match(image, templ, result_cuda_0);
		cuda::minMaxLoc(result_cuda_0, &minVal, &maxVal, &minLoc, &maxLoc, cuda::GpuMat());
	}
	else {
		b[1]->match(image, templ, result_cuda_1);
		cuda::minMaxLoc(result_cuda_1, &minVal, &maxVal, &minLoc, &maxLoc, cuda::GpuMat());
	}*/
	//auto t1 = chrono::high_resolution_clock::now();
	b[index]->match(image, templ, result_cuda[index], stream[index]);
	//auto t2 = chrono::high_resolution_clock::now();
	//auto ms_int = chrono::duration_cast<chrono::milliseconds>(t2 - t1);
	//cout << ms_int.count() << "ms\n";
	//cuda::normalize(result_cuda[index], result_cuda[index], 0, 1, NORM_MINMAX, -1, cuda::GpuMat());
	cuda::minMaxLoc(result_cuda[index], &minVal, &maxVal, &minLoc, &maxLoc, cuda::GpuMat());


	//cuda::normalize(result_cuda[index], result_cuda[index], 0, 1, NORM_MINMAX, -1, cuda::GpuMat());

	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc[index] = minLoc;
	}
	else
	{
		matchLoc[index] = maxLoc;
	}
	x_plus[index] = (matchLoc[index].x - lastMatch[index].x) * 2;
	y_plus[index] = (matchLoc[index].y - lastMatch[index].y) * 2;
	//rectangle(image, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	//if (matchLoc == lastMatch) return result_cuda;
	//cout << minVal << endl;
	lastMinVal[index] = minVal;

	//RECT rect = { matchLoc.x, matchLoc.y+150, matchLoc.x + 50 , matchLoc.y + 50+150 };
	lastMatch[index].x = matchLoc[index].x;
	lastMatch[index].y = matchLoc[index].y;
	//cout << "x: " << lastMatch.x << "y: " << lastMatch.y << endl;
	//cout << "x: " << matchLoc[index].x << "y: " << matchLoc[index].y << endl;
	//cout << "x: "<< x_plus << "y: "<< y_plus << endl;
	//imshow(image_window, img_display);
	//imshow(result_window, result);
	//return result_cuda[index];
	//if (index == 4) cout << "yep" << endl;
}

TemplateManager::constructor::constructor(cuda::GpuMat image, Mat templ) {
	result_cols = image.cols - templ.cols + 1;
	result_rows = image.rows - templ.rows + 1;
	match_method = 0;
	minVal = 0.f;
	maxVal =0.f;
	mousePos = Point(0.f, 0.f);

	hDC_Desktop = GetDC(0);
	blueBrush = CreateSolidBrush(RGB(0, 0, 0));

	result.create(result_rows, result_cols, CV_32FC1);
	result_cuda_0.create(result_rows, result_cols, CV_8UC1);
	result_cuda_1.create(result_rows, result_cols, CV_8UC1);
	cout << result_rows << endl << result_cols << endl;

	for (int i = 0; i < 5; i++) {
		b[i]=cuda::createTemplateMatching(CV_8UC1, match_method);
		result_cuda[i].create(result_rows, result_cols, CV_8UC1);
		lastMatch[i] = Point(0.f, 0.f);
		stream[i] = cuda::Stream();
	}

	cuda::HostMem bum;


}

void TemplateManager::SetMousePosition() {
	SetCursorPos(mousePos.x, mousePos.y);
}

void TemplateManager::DrawRectAndUpdateMousePos() {
	size_t bestIndex = distance(begin(lastMinVal), min_element(begin(lastMinVal), end(lastMinVal)));
	//int bestIndex =  min_element(begin(lastMinVal), end(lastMinVal)) - begin(lastMinVal) ;
	RECT rect = { matchLoc[bestIndex].x+640, matchLoc[bestIndex].y + 150+320, matchLoc[bestIndex].x + 20+640 , matchLoc[bestIndex].y + 20 + 150+320 };
	//RECT rect = { matchLoc[bestIndex].x + x_plus[bestIndex], matchLoc[bestIndex].y + y_plus[bestIndex] + 200	, matchLoc[bestIndex].x + 50 + x_plus[bestIndex], matchLoc[bestIndex].y + 50 + 200 + y_plus[bestIndex] };
	FillRect(hDC_Desktop, &rect, blueBrush);
	mousePos.x = matchLoc[bestIndex].x + x_plus[bestIndex] +640;
	mousePos.y = matchLoc[bestIndex].y + 150 + y_plus[bestIndex]+320;
	//cout << lastMinVal[0] << lastMinVal[0] << lastMinVal[0] << lastMinVal[0] << lastMinVal[0] << endl;
	//cout << bestIndex << endl;
}