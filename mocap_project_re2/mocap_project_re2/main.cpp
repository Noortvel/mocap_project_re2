#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <string>
#include <fstream>
#include "json\json_struct.h"
#include <opencv2/core/persistence.hpp>
#include <opencv2/dnn/dnn.hpp>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include "JsonHelper.h"
#include "NamePosition.h"

#include "SceneSettings.h"

#include "MainProcessor.h"

#include "FullStereoCalibration.h"
#include "CameraCalibrate.h"
#include "StereoCameraCalibrate.h"
#include "CachedCameraCalibrate.h"
#include "CachedStereoCameraCalibrate.h"
#include "StereoRectify.h"
#include "InitUndistortRectifyMap.h"
#include <limits>
#include <algorithm>
#include <map>

using namespace cv;
using namespace cv::Error;
using namespace std;
using namespace openmocap2;

void FindDistancions(float _focalLenght) {

	Mat imgL = imread("./DepthTest1/CameraL-Depth-.png", IMREAD_COLOR);
	Mat imgR = imread("./DepthTest1/CameraR-Depth-.png", IMREAD_COLOR);
	Mat gImgL;
	Mat gImgR;

	cv::cvtColor(imgL, gImgL, cv::COLOR_BGR2GRAY);
	cv::cvtColor(imgR, gImgR, cv::COLOR_BGR2GRAY);

	double thresh = 90;
	double maxValue = 255;
	cv::threshold(gImgL, gImgL, thresh, maxValue, THRESH_BINARY);
	cv::threshold(gImgR, gImgR, thresh, maxValue, THRESH_BINARY);

	SimpleBlobDetector::Params params;
	params.minDistBetweenBlobs = 0;
	params.filterByInertia = false;
	params.filterByConvexity = false;
	params.filterByColor = false;
	params.filterByCircularity = false;
	params.filterByArea = false;


	std::vector<KeyPoint> keypointsL;
	std::vector<KeyPoint> keypointsR;

	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	detector->detect(gImgL, keypointsL);
	detector->detect(gImgR, keypointsR);


	Mat im_with_keypointsL;
	Mat im_with_keypointsR;

	drawKeypoints(imgL, keypointsL, im_with_keypointsL, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(imgR, keypointsR, im_with_keypointsR, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	imshow("CameraL", im_with_keypointsL);
	imshow("CameraR", im_with_keypointsR);

	//waitKey(0);

	float focalLenght = _focalLenght;// intrinsic.at<float>(0, 0);
	float distance = 0.5f;
	std::sort(keypointsL.begin(), keypointsL.end(), [](cv::KeyPoint& a, cv::KeyPoint& b)->bool { return a.pt.y >= b.pt.y; });
	std::sort(keypointsR.begin(), keypointsR.end(), [](cv::KeyPoint& a, cv::KeyPoint& b)->bool { return a.pt.y >= b.pt.y; });



	for (int i = 0; i < keypointsL.size(); i++) {
		auto& itemL = keypointsL[i];
		auto& itemR = keypointsR[i];
		auto point1 = itemL.pt.x - itemR.pt.x;
		float Z1 = point1 * 1 / (distance * focalLenght);
		std::cout << "Distance on shpere = " << Z1 << endl;

	}
}



void oldDetection() {
	vector<cv::String> inputImagesPaths;
	glob("./Callibration/Callibration-CCameraL*.png", inputImagesPaths);
	auto patternSize = cv::Size2i(6, 9);
	auto drawColor = Scalar(0, 0, 255);
	vector<Point3f> obj;
	int numSquares = patternSize.area();

	float metricScale = 0.025226f;//cellSize
	for (int y = patternSize.height - 1; y >= 0; y--) {
		for (int x = 0; x < patternSize.width; x++) {
			obj.push_back(Point3f(x * metricScale, y * metricScale, 0.0f));
			//cout << obj.back().x << " " << obj.back().y << " " << endl;
		}
	}
	/*for (int j = 0; j < numSquares; j++) {
		obj.push_back(Point3d((int)(j / patternSize.width) , j % patternSize.width, 0.0f));
		cout << obj.back().x << " " << obj.back().y << " " << endl;
	}*/

	vector<vector<Point2f>> finded_corners;
	vector<vector<Point3f>> all_3dpoints;
	int image_width, image_height = -1;
	Size image_size;

	std::cout << "Chessboard find begin" << endl;
	int chessboardIndex = 0;
	for (auto&& imageName : inputImagesPaths) {
		chessboardIndex++;
		//cout << image << endl;
		vector<Point2f> corners;
		auto imageMat = imread(imageName, IMREAD_COLOR);
		image_size.height = imageMat.rows;
		image_size.width = imageMat.cols;

		bool succes = findChessboardCorners(imageMat, patternSize, corners);
		std::cout << imageName << " Chessboard [" << chessboardIndex << "/" << inputImagesPaths.size() << "] " << succes << endl;
		Mat grayscale;
		cv::cvtColor(imageMat, grayscale, ColorConversionCodes::COLOR_BGR2GRAY);
		if (succes) {
			cornerSubPix(grayscale, corners, Size(11, 11), Size(-1, -1), TermCriteria((TermCriteria::EPS + TermCriteria::COUNT), 30, 0.001));
			finded_corners.push_back(corners);
			all_3dpoints.push_back(obj);
			/*for (auto&& position : corners) {
				drawMarker(imageMat, position, drawColor);
				imshow(imageName, imageMat);
				cv::waitKey(0);
			}*/
		}
		/*cout << "isFound: " << succes << " Corners Count: " << corners.size() << endl;
		imshow(imageName, imageMat);
		cv::waitKey(0);*/
		//

		/*if (succes) {
		}*/
	}
	std::cout << "Chessboard find end" << endl;

	std::cout << "Find corners end" << endl;

	Mat camerMatrix = Mat(3, 3, CV_32FC1);//instricts//CV_32FC1
	Mat distCoeffs;
	vector<Mat> rvecs;
	vector<Mat> tvecs;

	std::cout << "Calibrate camera begin" << endl;

	calibrateCamera(all_3dpoints, finded_corners, image_size, camerMatrix, distCoeffs, rvecs, tvecs, 0, TermCriteria((TermCriteria::EPS + TermCriteria::COUNT), 30, 0.001));
	std::cout << "Calibrate camera end" << endl;
	std::cout << "Camera matrix = \n" << camerMatrix << endl;
	std::cout << "Dist coeffs = \n " << distCoeffs << endl;


	float_t _focalLenght = camerMatrix.at<float_t>(0, 0);
	std::cout << _focalLenght << endl;
	FindDistancions(_focalLenght);

	/*cout << "Rotate vectors: " << endl;
	for (auto&& item : rvecs) {
		cout << item << endl;
	}
	cout << "Tranlate vectors: " << endl;
	for (auto&& item : tvecs) {
		cout << item << endl;
	}*/

	//Detection part
	/*vector<cv::String> detectionsImagePath;

	glob("./DepthTest1/Camera*.png", detectionsImagePath);

	for (auto& item : detectionsImagePath) {

	}*/



	cv::waitKey(0);
}

void MatWork() {
	Mat camerMatrix = Mat(3, 3, CV_32FC1);
	float& a = camerMatrix.at<float>(0, 0);
	a = 100;

	std::cout << camerMatrix << endl;
}

void PlayVideo(VideoCapture cap) {
	double fps = cap.get(CAP_PROP_FPS);
	std::cout << "Frames per seconds : " << fps << endl;

	String window_name = "My First Video";

	namedWindow(window_name, WINDOW_NORMAL); //create a window

	while (true)
	{
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video 

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			std::cout << "Found the end of the video" << endl;
			break;
		}

		//show the frame in the created window
		imshow(window_name, frame);

		//wait for for 10 ms until any key is pressed.  
		//If the 'Esc' key is pressed, break the while loop.
		//If the any other key is pressed, continue the loop 
		//If any key is not pressed withing 10 ms, continue the loop
		if (cv::waitKey(10) == 27)
		{
			std::cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}
	}
}

void VideoBlobs() {
	VideoCapture videoL("./data/video/camera0_007.mp4");
	VideoCapture videoR("./data/video/camera1_007.mp4");

	cv::SimpleBlobDetector::Params params;
	params.minDistBetweenBlobs = 0;
	params.filterByInertia = false;
	params.filterByConvexity = false;
	params.filterByColor = false;
	params.filterByCircularity = false;
	params.filterByArea = false;


	vector<KeyPoint> keypointsL;
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	Mat thresholdMat;
	Mat frame;
	Mat frameWithKeyPoints;

	std::cout << "Find blobs started" << endl;
	bool isSucces = false;
	while (true) {
		bool bSuccess = videoR.read(frame);
		if (!bSuccess) break;

		cv::cvtColor(frame, thresholdMat, cv::COLOR_BGR2GRAY);

		double thresh = 210;//90//150
		double maxValue = 255;
		cv::threshold(thresholdMat, thresholdMat, thresh, maxValue, THRESH_BINARY);
		detector->detect(thresholdMat, keypointsL);
		drawKeypoints(thresholdMat, keypointsL, frameWithKeyPoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		keypointsL.clear();
		imshow("Main", frameWithKeyPoints);
		cv::waitKey(100);
	}
	std::cout << "Find blobs ended, press any key" << endl;
	cv::waitKey(0);
}


string CocoKeyPointToString(int keypoint) {
	static map<int, string> keypoins = {
		{0, "nose"},
		{1, "left_eye"},
		{2, "right_eye"},
		{3  ,"left_ear"},
		{4  ,"right_ear"},
		{5  ,"left_shoulder"},
		{6  ,"right_shoulder"},
		{7  ,"left_elbow"},
		{8  ,"right_elbow"},
		{9  ,"left_wrist"},
		{10 , "right_wrist"},
		{11 , "left_hip"},
		{12 , "right_hip"},
		{13 , "left_knee"},
		{14 , "right_knee"},
		{15 , "left_ankle"},
		{16 , "right_ankle"}
	};
	return keypoins[keypoint];
}

int main() {
	//MainProcessor mainProceccor;
	//VideoBlobs();

	//2.jpg
	//i3.png
	//m1.png
	auto testImg = cv::imread("./data/dnn_test/m1.png", cv::IMREAD_COLOR);
	Mat sourceImageTransformed;

	cv::resize(testImg, sourceImageTransformed, Size(192, 256));
	cv::cvtColor(sourceImageTransformed, sourceImageTransformed, COLOR_BGR2RGB);


	//cv::normalize(testImg2Fitted, testImg2Fitted, 0, 1, cv::NORM_MINMAX);

	auto inputMean = cv::mean(sourceImageTransformed);
	cout << inputMean << endl;

	auto testImg2FittedType = sourceImageTransformed.type();
	auto srctypeStr = cv::typeToString(testImg2FittedType);

	Mat testImgNormalized;
	sourceImageTransformed.convertTo(testImgNormalized, CV_32FC3); //(double)1 / (double)255
	cout << "testImgNormalizedMean " << cv::mean(testImgNormalized) << endl;

	cv::normalize(testImgNormalized, testImgNormalized, 1, 0, cv::NORM_MINMAX);

	//[0.485, 0.456, 0.406]
	auto targetMean = Scalar(0.485, 0.456, 0.406);
	auto imgMean = cv::mean(testImgNormalized);
	auto meanToAdd = targetMean - imgMean;


	Mat tMean = Mat({ 0.485, 0.456, 0.406 });
	Mat tStd = Mat({ 0.229, 0.224, 0.225 });

	Mat meanNrm, stdNrm;
	cv::meanStdDev(testImgNormalized, meanNrm, stdNrm);
	
	Mat meanAdd, stdAdd;

	cv::subtract(tMean, meanNrm, meanAdd);
	cv::subtract(tStd, stdNrm, meanAdd);

	auto _sa = cv::typeToString(meanNrm.type());
	double val = meanNrm.at<double>(0);

	//meanNrm.convertTo()
	cout << "mean " << meanNrm << endl << "std " << stdNrm << endl;

	
	for (int i = 0; i < testImgNormalized.cols; i++) {
		for (int j = 0; j < testImgNormalized.rows; j++) {
			auto& intensity = testImgNormalized.at<Vec3f>(j, i);
			for (int k = 0; k < testImgNormalized.channels(); k++) {
				intensity.val[k] -= meanNrm.at<double>(k);
				intensity.val[k] /= stdNrm.at<double>(k);
				intensity.val[k] *= tStd.at<double>(k);
				intensity.val[k] += tMean.at<double>(k);
			}
		}
	}

	cv::meanStdDev(testImgNormalized, meanNrm, stdNrm);
	cout << "mean " << meanNrm << endl << "std " << stdNrm << endl;

	sourceImageTransformed = testImgNormalized;



	//cv::meanStdDev(testImgNormalized, meanNrm, stdNrm);

	/*auto testBlob = cv::dnn::blobFromImage(testImg2Fitted);
	auto testBlobMean = cv::mean(testBlob);
	cout << testBlobMean << endl;*/
	
	//./dnn_models/mmpose_hrnet_w32_coco_topdown_256x192.onnx -- broken
	//./dnn_models/pose_hrnet_w32_256x192.onnx -- worked
	//./dnn_models/mmpose_hrnet_w32_coco_256x192_handzoo.onnx -- worked

	auto uniposeDnn = cv::dnn::readNetFromONNX("./dnn_models/pose_hrnet_w32_256x192.onnx");

	std::cout << "in img size: " << sourceImageTransformed.size << endl;
	//1 3 256 192
	auto blob = cv::dnn::blobFromImage(sourceImageTransformed);
	std::cout << "in blob size: " << blob.size << endl;
	uniposeDnn.setInput(blob);
	auto outBlob = uniposeDnn.forward();
	std::cout << "out blob size: " << outBlob.size << endl;

	vector<Mat> heatmaps;
	float minVal = numeric_limits<float>::min();
	float maxVal = numeric_limits<float>::max();

	Mat showMixed;
	int fitWidth = 192 * 3;
	int fitHeight = 256 * 3;
	Mat heatMapsmShow = Mat(Size(fitWidth, fitHeight), CV_32F);
	cv::resize(testImg, showMixed, Size(fitWidth, fitHeight));

	size_t keyPoints = 17;
	for (size_t layer = 0; layer < keyPoints; layer++) {
		heatmaps.push_back(Mat(Size(48, 64), outBlob.type()));
		Mat& hm = heatmaps.back();
		for (int y = 0; y < 64; y++) {
			for (int x = 0; x < 48; x++) {
				float& val = hm.at<float>(y, x);
				float& srcVal = outBlob.at<float>(Vec<int, 4>(0, layer, y, x));
				val = srcVal;
				minVal = std::min(val, minVal);
				maxVal = std::max(val, maxVal);
			}
		}

		
		Mat hmImageShow;
		cv::resize(hm, hmImageShow, Size(fitWidth, fitHeight));

		cv::normalize(hmImageShow, hmImageShow, 0, 1, cv::NORM_MINMAX);
		cv::threshold(hmImageShow, hmImageShow, 0.5, 1, cv::THRESH_TOZERO);
		
		
		Mat hmColorizedShow;
		hmImageShow.convertTo(hmColorizedShow, CV_8UC3, 255);
		cv::cvtColor(hmColorizedShow, hmColorizedShow, COLOR_GRAY2RGB);
		//cv::applyColorMap(hmColorizedShow, hmColorizedShow, COLORMAP_JET);

		cv::addWeighted(heatMapsmShow, 1, hmImageShow, 1, 0, heatMapsmShow);
		//cv::addWeighted(showMixed, 0.5, hmColorizedShow, 1, 0, showMixed);
		
		//cv::putText(showMixed, "layer: " + std::to_string(layer) + CocoKeyPointToString(layer), cv::Point(50, 50), cv::FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0));

		//cv::imwrite("layer_" + std::to_string(layer) + ".png", showMixed);
		std::cout << layer << endl;
		//std::cout << "dtLen: " << dtLen << endl;
	}

	cv::normalize(heatMapsmShow, heatMapsmShow, 0, 1, cv::NORM_MINMAX);
	heatMapsmShow.convertTo(heatMapsmShow, CV_8UC3, 255);
	cv::cvtColor(heatMapsmShow, heatMapsmShow, COLOR_GRAY2RGB);
	cv::applyColorMap(heatMapsmShow, heatMapsmShow, COLORMAP_JET);

	cv::applyColorMap(heatMapsmShow, heatMapsmShow, COLORMAP_JET);
	cv::addWeighted(showMixed, 0.5, heatMapsmShow, 1, 0, showMixed);

	cv::imshow("MM", heatMapsmShow);
	cv::waitKey();

	

	/*cv::FileStorage fs("./ttt2.json", cv::FileStorage::WRITE);
	fs << "matrix" << mm;
	fs.release();

	cout << mm;
	*/

	
	//auto mh = outBlob.at<Mat>(cv::Vec<int, 2>(1, 1));


	//auto outBlobSize = outBlob.size;
	//std::cout << outBlobSize << endl;

	vector<Mat> outImgs;
	cv::dnn::imagesFromBlob(outBlob, outImgs);

	auto outImageSize = outImgs[0].size;
	std::cout << outImageSize << endl;

	//cout << outImgs[0] << endl;

	/*
	cv::FileStorage fs("./ttt2.json", cv::FileStorage::WRITE);
	fs << "matrix" << outBlob;
	fs.release();
	*/

	//imshow("Main", outImgs[0]);

	cv::waitKey();
	return 0;

	spdlog::set_level(spdlog::level::debug);
	FullStereoСalibration stereoCalib;
	stereoCalib.Calibrate();




	//DEPTH TESTER
	Mat depthL = imread("./data/depth/CameraL-Depth-.png");
	Mat depthR = imread("./data/depth/CameraR-Depth-.png");

	Mat depth1Rmpd, depth2Rmpd;

	cv::remap(depthL, depth1Rmpd,
		stereoCalib.InitUndistortRectifyMap1().map1, stereoCalib.InitUndistortRectifyMap1().map2, INTER_LINEAR);
	cv::remap(depthR, depth2Rmpd,
		stereoCalib.InitUndistortRectifyMap2().map1, stereoCalib.InitUndistortRectifyMap2().map2, INTER_LINEAR);

	depth1Rmpd = depthL;
	depth2Rmpd = depthR;

	cv::SimpleBlobDetector::Params params;
	params.minDistBetweenBlobs = 0;
	params.filterByInertia = false;
	params.filterByConvexity = false;
	params.filterByColor = false;
	params.filterByCircularity = false;
	params.filterByArea = false;
	vector<KeyPoint> keypointsL;
	vector<KeyPoint> keypointsR;
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	Mat depthLGray, depthRGray;
	cv::cvtColor(depth1Rmpd, depthLGray, cv::COLOR_BGR2GRAY);
	cv::cvtColor(depth2Rmpd, depthRGray, cv::COLOR_BGR2GRAY);

	double thresh = 90;//90//150
	double maxValue = 255;
	cv::threshold(depthLGray, depthLGray, thresh, maxValue, THRESH_BINARY);
	cv::threshold(depthRGray, depthRGray, thresh, maxValue, THRESH_BINARY);

	detector->detect(depthLGray, keypointsL);
	detector->detect(depthRGray, keypointsR);

	//drawKeypoints(depthLGray, keypointsL, depthLGray, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//cv::imshow("Main", depthLGray);

	std::vector<Mat> outArray(keypointsL.size());
	for (auto iter : outArray)
	{
		iter.create(4, 1, CV_64F);
	}
	vector<Point2f> vecL;
	vector<Point2f> vecR;
	for (auto& iter : keypointsL) vecL.push_back(iter.pt);
	for (auto& iter : keypointsR) vecR.push_back(iter.pt);
	auto comp = [](Point2f& a, Point2f& b) { return a.y > b.y; };
	std::sort(vecL.begin(), vecL.end(), comp);
	std::sort(vecR.begin(), vecR.end(), comp);

	Mat outMat(1, keypointsL.size(), CV_64F);
	cv::triangulatePoints(stereoCalib.Rectify().P1, stereoCalib.Rectify().P2, vecL, vecR, outMat);

	using namespace std;
	ifstream json_file("./data/depth/ObjectsDistances.json");
	string json_string((istreambuf_iterator<char>(json_file)), istreambuf_iterator<char>());
	json_file.close();
	nlohmann::json namePosition = nlohmann::json::parse(json_string);
	vector<Vec3> realObjectsPositions;
	for (auto& el : namePosition) {
		std::cout << el.dump() << endl;
		Vec3 v;
		v.x = el["Position"]["x"].get<float>();
		v.y = el["Position"]["y"].get<float>();
		v.z = el["Position"]["z"].get<float>();
		realObjectsPositions.push_back(v);
	}

	vector<Vec3> detectedObjectPositions;
	for (int j = 0; j < vecL.size(); j++) {
		std::cout << "Coord" << endl;
		std::cout << "(";
		//To Decard system
		float w = outMat.at<float>(3, j);
		Vec3 v;
		v.x = outMat.at<float>(0, j) / w;
		v.y = -outMat.at<float>(1, j) / w; // Coord y is reversed in image coord system
		v.z = outMat.at<float>(2, j) / w;
		detectedObjectPositions.push_back(v);
		for (int i = 0; i < 4; i++) {
			std::cout << " " << (outMat.at<float>(i, j) / w)  << " ";
		}

		std::cout << ")\n";
	}
	
	int minObjectsCount = std::min(detectedObjectPositions.size(), realObjectsPositions.size());
	vector<Vec3> linarDelta;
	for (size_t i = 0; i < minObjectsCount; i++) {
		Vec3& detected = detectedObjectPositions[i];
		Vec3& real = realObjectsPositions[i];
		Vec3 result = real - detected;
		linarDelta.push_back(result);
		std::cout << "vec delta" << endl;
		std::cout << "(" << result.x << ", " << result.y << ", " << result.z << ")";
		std::cout << endl;
	}
	//[](auto& a, auto& b) { return a < b; }
	auto minMaxDelta = std::minmax_element(
		linarDelta.begin(), linarDelta.end(), [](auto& a, auto& b) { return a.length() < b.length(); });

	std::cout << "Linar error: " << (*minMaxDelta.first - *minMaxDelta.second).length() << endl;
	cv::waitKey(0);

	//cout << R1 << endl << R2 << endl << P1 << endl << P2 << endl << Q << endl;
	return 0;
	//OLD
	//FindDistancions((float)cameraMatrix.at<double>(0, 0));
	//auto focalLen = result.cameraMatrix.at<float>(0, 0);
	//cout << "Focal Len=" << focalLen << endl;
	//http://www.vicon.com/hardware/cameras/vero/
}