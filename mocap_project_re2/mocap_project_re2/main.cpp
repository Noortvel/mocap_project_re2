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

//#include "JsonHelper.h"
//#include "NamePosition.h"
//
//#include "SceneSettings.h"
//
//#include "MainProcessor.h"
//
//#include "FullStereoCalibration.h"
//#include "CameraCalibrate.h"
//#include "StereoCameraCalibrate.h"
//#include "CachedCameraCalibrate.h"
//#include "CachedStereoCameraCalibrate.h"
//#include "StereoRectify.h"
//#include "InitUndistortRectifyMap.h"
//#include "DnnHumanPoseDetector.h"

//#include <limits>
//#include <algorithm>
//#include <map>
#include "Application.h"

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


int main() {
	//MainProcessor mainProceccor;
	//VideoBlobs();

	//2.jpg
	//i3.png
	//m1.png
	//syntatic1.png


	openmocap2::Application app;
	app.Start();

	return 0;

	//spdlog::set_level(spdlog::level::debug);
	//FullStereoСalibration stereoCalib;
	//stereoCalib.Calibrate();



	////DEPTH TESTER
	//Mat depthL = imread("./data/depth/CameraL-Depth-.png");
	//Mat depthR = imread("./data/depth/CameraR-Depth-.png");

	//Mat depth1Rmpd, depth2Rmpd;

	//cv::remap(depthL, depth1Rmpd,
	//	stereoCalib.InitUndistortRectifyMap1().map1, stereoCalib.InitUndistortRectifyMap1().map2, INTER_LINEAR);
	//cv::remap(depthR, depth2Rmpd,
	//	stereoCalib.InitUndistortRectifyMap2().map1, stereoCalib.InitUndistortRectifyMap2().map2, INTER_LINEAR);

	//depth1Rmpd = depthL;
	//depth2Rmpd = depthR;

	//cv::SimpleBlobDetector::Params params;
	//params.minDistBetweenBlobs = 0;
	//params.filterByInertia = false;
	//params.filterByConvexity = false;
	//params.filterByColor = false;
	//params.filterByCircularity = false;
	//params.filterByArea = false;
	//vector<KeyPoint> keypointsL;
	//vector<KeyPoint> keypointsR;
	//Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	//Mat depthLGray, depthRGray;
	//cv::cvtColor(depth1Rmpd, depthLGray, cv::COLOR_BGR2GRAY);
	//cv::cvtColor(depth2Rmpd, depthRGray, cv::COLOR_BGR2GRAY);

	//double thresh = 90;//90//150
	//double maxValue = 255;
	//cv::threshold(depthLGray, depthLGray, thresh, maxValue, THRESH_BINARY);
	//cv::threshold(depthRGray, depthRGray, thresh, maxValue, THRESH_BINARY);

	//detector->detect(depthLGray, keypointsL);
	//detector->detect(depthRGray, keypointsR);

	////drawKeypoints(depthLGray, keypointsL, depthLGray, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	////cv::imshow("Main", depthLGray);

	//std::vector<Mat> outArray(keypointsL.size());
	//for (auto iter : outArray)
	//{
	//	iter.create(4, 1, CV_64F);
	//}
	//vector<Point2f> vecL;
	//vector<Point2f> vecR;
	//for (auto& iter : keypointsL) vecL.push_back(iter.pt);
	//for (auto& iter : keypointsR) vecR.push_back(iter.pt);
	//auto comp = [](Point2f& a, Point2f& b) { return a.y > b.y; };
	//std::sort(vecL.begin(), vecL.end(), comp);
	//std::sort(vecR.begin(), vecR.end(), comp);

	//Mat outMat(1, keypointsL.size(), CV_64F);
	//cv::triangulatePoints(stereoCalib.Rectify().P1, stereoCalib.Rectify().P2, vecL, vecR, outMat);

	//using namespace std;
	//ifstream json_file("./data/depth/ObjectsDistances.json");
	//string json_string((istreambuf_iterator<char>(json_file)), istreambuf_iterator<char>());
	//json_file.close();
	//nlohmann::json namePosition = nlohmann::json::parse(json_string);
	//vector<Vec3> realObjectsPositions;
	//for (auto& el : namePosition) {
	//	std::cout << el.dump() << endl;
	//	Vec3 v;
	//	v.x = el["Position"]["x"].get<float>();
	//	v.y = el["Position"]["y"].get<float>();
	//	v.z = el["Position"]["z"].get<float>();
	//	realObjectsPositions.push_back(v);
	//}

	//vector<Vec3> detectedObjectPositions;
	//for (int j = 0; j < vecL.size(); j++) {
	//	std::cout << "Coord" << endl;
	//	std::cout << "(";
	//	//To Decard system
	//	float w = outMat.at<float>(3, j);
	//	Vec3 v;
	//	v.x = outMat.at<float>(0, j) / w;
	//	v.y = -outMat.at<float>(1, j) / w; // Coord y is reversed in image coord system
	//	v.z = outMat.at<float>(2, j) / w;
	//	detectedObjectPositions.push_back(v);
	//	for (int i = 0; i < 4; i++) {
	//		std::cout << " " << (outMat.at<float>(i, j) / w)  << " ";
	//	}

	//	std::cout << ")\n";
	//}
	//
	//int minObjectsCount = std::min(detectedObjectPositions.size(), realObjectsPositions.size());
	//vector<Vec3> linarDelta;
	//for (size_t i = 0; i < minObjectsCount; i++) {
	//	Vec3& detected = detectedObjectPositions[i];
	//	Vec3& real = realObjectsPositions[i];
	//	Vec3 result = real - detected;
	//	linarDelta.push_back(result);
	//	std::cout << "vec delta" << endl;
	//	std::cout << "(" << result.x << ", " << result.y << ", " << result.z << ")";
	//	std::cout << endl;
	//}
	////[](auto& a, auto& b) { return a < b; }
	//auto minMaxDelta = std::minmax_element(
	//	linarDelta.begin(), linarDelta.end(), [](auto& a, auto& b) { return a.length() < b.length(); });

	//std::cout << "Linar error: " << (*minMaxDelta.first - *minMaxDelta.second).length() << endl;
	//cv::waitKey(0);

	////cout << R1 << endl << R2 << endl << P1 << endl << P2 << endl << Q << endl;
	//return 0;
	////OLD
	////FindDistancions((float)cameraMatrix.at<double>(0, 0));
	////auto focalLen = result.cameraMatrix.at<float>(0, 0);
	////cout << "Focal Len=" << focalLen << endl;
	////http://www.vicon.com/hardware/cameras/vero/
}