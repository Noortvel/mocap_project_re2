#include "DnnHumanBoxDetector.h"
#include <opencv2\opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

openmocap2::DnnHumanBoxDetector::DnnHumanBoxDetector()
{
	boxCorners.push_back(Point(0, 0));
	boxCorners.push_back(Point(0, 0));
	yolov5s = dnn::readNetFromONNX(MODEL_PATH);
}

const vector<Point>& openmocap2::DnnHumanBoxDetector::Forward(const Mat& img)
{
	Mat fittedImg;
	cv::resize(img, fittedImg, INPUT_SIZE);
	cv::cvtColor(fittedImg, fittedImg, COLOR_BGR2RGB);
	Mat meanNrm, stdNrm;
	cv::meanStdDev(fittedImg, meanNrm, stdNrm);
	for (int i = 0; i < fittedImg.cols; i++) {
		for (int j = 0; j < fittedImg.rows; j++) {
			auto& intensity = fittedImg.at<Vec3b>(j, i);
			for (int k = 0; k < fittedImg.channels(); k++) {
				intensity.val[k] -= meanNrm.at<double>(k);
				intensity.val[k] /= stdNrm.at<double>(k);
			}
		}
	}


	Mat blob = dnn::blobFromImage(fittedImg);
	cv::normalize(blob, blob, 0, 1, cv::NORM_MINMAX);

	auto& blobSize = blob.size;
	cout << blobSize << endl;
	cout << typeToString(blob.type()) << endl;

	yolov5s.setInput(blob);
	const auto& names = yolov5s.getLayerNames();

	vector<vector<Mat>> arrayOutputMats;
	int out1[] = { 1, 25200, 85 };
	int out2[] = { 1, 3, 80, 80, 85 };
	int out3[] = { 1, 3, 40, 40, 85 };
	int out4[] = { 1, 3, 20, 20, 85 };
	
	vector<Mat> outputMats;

	outputMats.push_back(Mat (3, out1, CV_32F, Scalar::all(0)));
	outputMats.push_back(Mat (5, out2, CV_32F, Scalar::all(0)));
	outputMats.push_back(Mat (5, out3, CV_32F, Scalar::all(0)));
	outputMats.push_back(Mat (5, out4, CV_32F, Scalar::all(0)));

	arrayOutputMats.push_back(outputMats);
	vector<string> names2 { "561", "397", "458", "519" };

	vector<Mat> outs561;

	vector<Mat> outs;
	auto nmm = yolov5s.getUnconnectedOutLayersNames();
	yolov5s.forward(outs, nmm);
	//yolov5s.forward(arrayOutputMats, names2); //397 - classes, 458 - boxes

	//Mat outblob;
	//for (int layer = 0; layer < 85; layer++) {
	//	Mat personHeatMap = Mat(40, 40, CV_32FC3);
	//	cv::cvtColor(personHeatMap, personHeatMap, COLOR_BGR2RGB);


	//	float* pixelPtr = (float*)personHeatMap.data;
	//	for (int y = 0; y < 40; y++) {
	//		for (int x = 0; x < 40; x++) {
	//			auto& intensity = personHeatMap.at<Vec3f>(y, x);
	//			for (int c = 0; c < 3; c++) {
	//				float& val = outblob.at<float, 5>(Vec<int, 5>(0, c, y, x, 0));
	//				//pixelPtr[y * 80 * 3 + x * 3 + c] = val;
	//				intensity.val[c] = val;
	//			}
	//		}
	//	}

	//	cv::normalize(personHeatMap, personHeatMap, 0, 1, cv::NORM_MINMAX);
	//	cv::threshold(personHeatMap, personHeatMap, 0.5, 1, cv::THRESH_TOZERO);
	//	personHeatMap.convertTo(personHeatMap, CV_8UC3, 255);
	//	cv::resize(personHeatMap, personHeatMap, Size(80 * 5, 80 * 5));
	//	cv::imshow("Map", personHeatMap);
	//	waitKey();
	//	cout << layer << endl;
	//}
	//auto outBlobSize = outBlob.size();
	//cout << outBlobSize << endl;
	return boxCorners;
}
