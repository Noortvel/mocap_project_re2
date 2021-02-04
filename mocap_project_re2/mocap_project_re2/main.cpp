#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <string>
#include <fstream>
#include "json\json_struct.h"
#include <opencv2/core/persistence.hpp>
#include <spdlog/spdlog.h>

#include "SceneSettings.h"

#include "MainProcessor.h"
#include "CameraCalibratior.h"
#include "CameraChessPatternCalibrateTask.h"
#include "StereoCameraChessPatternCalibrateTask.h"
#include "CachedCameraChessPatternCalibrateTask.h"
#include "CachedStereoCameraChessPatternCalibrateTask.h"



using namespace cv;
using namespace cv::Error;
using namespace std;

void FindDistancions(float _focalLenght) {

	Mat imgL = imread("./DepthTest1/CameraL-Depth-.png", IMREAD_COLOR);
	Mat imgR = imread("./DepthTest1/CameraR-Depth-.png", IMREAD_COLOR);
	Mat gImgL;
	Mat gImgR;

	cv::cvtColor(imgL, gImgL, cv::COLOR_BGR2GRAY);
	cv::cvtColor(imgR, gImgR, cv::COLOR_BGR2GRAY);

	double thresh = 90;
	double maxValue = 255;
	threshold(gImgL, gImgL, thresh, maxValue, THRESH_BINARY);
	threshold(gImgR, gImgR, thresh, maxValue, THRESH_BINARY);

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
		cout << "Distance on shpere = " << Z1 << endl;

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

	cout << "Chessboard find begin" << endl;
	int chessboardIndex = 0;
	for (auto&& imageName : inputImagesPaths) {
		chessboardIndex++;
		//cout << image << endl;
		vector<Point2f> corners;
		auto imageMat = imread(imageName, IMREAD_COLOR);
		image_size.height = imageMat.rows;
		image_size.width = imageMat.cols;

		bool succes = findChessboardCorners(imageMat, patternSize, corners);
		cout << imageName << " Chessboard [" << chessboardIndex << "/" << inputImagesPaths.size() << "] " << succes << endl;
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
	cout << "Chessboard find end" << endl;

	cout << "Find corners end" << endl;

	Mat camerMatrix = Mat(3, 3, CV_32FC1);//instricts//CV_32FC1
	Mat distCoeffs;
	vector<Mat> rvecs;
	vector<Mat> tvecs;

	cout << "Calibrate camera begin" << endl;

	calibrateCamera(all_3dpoints, finded_corners, image_size, camerMatrix, distCoeffs, rvecs, tvecs, 0, TermCriteria((TermCriteria::EPS + TermCriteria::COUNT), 30, 0.001));
	cout << "Calibrate camera end" << endl;
	cout << "Camera matrix = \n" << camerMatrix << endl;
	cout << "Dist coeffs = \n " << distCoeffs << endl;


	float_t _focalLenght = camerMatrix.at<float_t>(0, 0);
	cout << _focalLenght << endl;
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

	cout << camerMatrix << endl;
}

void PlayVideo(VideoCapture cap) {
	double fps = cap.get(CAP_PROP_FPS);
	cout << "Frames per seconds : " << fps << endl;

	String window_name = "My First Video";

	namedWindow(window_name, WINDOW_NORMAL); //create a window

	while (true)
	{
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video 

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			cout << "Found the end of the video" << endl;
			break;
		}

		//show the frame in the created window
		imshow(window_name, frame);

		//wait for for 10 ms until any key is pressed.  
		//If the 'Esc' key is pressed, break the while loop.
		//If the any other key is pressed, continue the loop 
		//If any key is not pressed withing 10 ms, continue the loop
		if (waitKey(10) == 27)
		{
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
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

	cout << "Find blobs started" << endl;
	bool isSucces = false;
	while (true) {
		bool bSuccess = videoR.read(frame);
		if (!bSuccess) break;

		cv::cvtColor(frame, thresholdMat, cv::COLOR_BGR2GRAY);

		double thresh = 210;//90//150
		double maxValue = 255;
		threshold(thresholdMat, thresholdMat, thresh, maxValue, THRESH_BINARY);
		detector->detect(thresholdMat, keypointsL);
		drawKeypoints(thresholdMat, keypointsL, frameWithKeyPoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		keypointsL.clear();
		imshow("Main", frameWithKeyPoints);
		waitKey(100);
	}
	cout << "Find blobs ended, press any key" << endl;
	cv::waitKey(0);
}

int main() {
	//MainProcessor mainProceccor;

	//VideoBlobs();

	spdlog::set_level(spdlog::level::debug);
	auto patternSize = Size2i(6, 9);
	auto cellSize = 2 * 0.025226f;//
	vector<Point3f> chessboardPoints3D;
	for (int y = patternSize.height - 1; y >= 0; y--) {
		for (int x = 0; x < patternSize.width; x++) {
			chessboardPoints3D.push_back(Point3f(x * cellSize, y * cellSize, 0.0f));
		}
	}

	CachedCameraChessPatternCalibrateTask::Input calibrateCameraCommonTaskInput;
	calibrateCameraCommonTaskInput.calibrateImagesPathMask = "./data/calibration/inner_params/camera_common/Callibration-CameraCommon*.png";
	calibrateCameraCommonTaskInput.cellSize = cellSize;
	calibrateCameraCommonTaskInput.patternSize = patternSize;
	calibrateCameraCommonTaskInput.chessboard3dPoints = &chessboardPoints3D;
	calibrateCameraCommonTaskInput.cachedResultFilePath = "./data_cache/cameraCalibCommon.json";

	CachedCameraChessPatternCalibrateTask::Task cachedCameraCalib;
	cachedCameraCalib.Execute(calibrateCameraCommonTaskInput);
	auto cachedCameraCalibResult = cachedCameraCalib.Result();

	CachedStereoCameraChessPatternCalibrateTask::Input cachedStereoCamCalibInput;
	cachedStereoCamCalibInput.cameraLCalibrateImagesPathMask = "./data/calibration/common/StereoCamera0*.png";
	cachedStereoCamCalibInput.cameraRCalibrateImagesPathMask = "./data/calibration/common/StereoCamera1*.png";
	cachedStereoCamCalibInput.cellSize = cellSize;
	cachedStereoCamCalibInput.chessboard3dPoints = &chessboardPoints3D;
	cachedStereoCamCalibInput.patternSize = patternSize;
	cachedStereoCamCalibInput.cachedResultFilePath = "./data_cache/stereoCameraCalibCommon.json";
	cachedStereoCamCalibInput.camera1Result = &cachedCameraCalibResult;
	cachedStereoCamCalibInput.camera2Result = &cachedCameraCalibResult;

	CachedStereoCameraChessPatternCalibrateTask::Task cachedStereoCameraCalib;
	cachedStereoCameraCalib.Execute(cachedStereoCamCalibInput);
	Mat _calibImg = imread("./data/calibration/common/StereoCamera0-1611526867375.png");
	auto _imgSize = _calibImg.size();
	auto stereoResult = cachedStereoCameraCalib.Result();
	Mat R1, R2, P1, P2, Q;
	Rect ROI1, ROI2;
	cv::stereoRectify(
		cachedCameraCalibResult.cameraMatrix,
		cachedCameraCalibResult.distCoeffs,
		cachedCameraCalibResult.cameraMatrix,
		cachedCameraCalibResult.distCoeffs,
		_imgSize,
		stereoResult.R,
		stereoResult.T,
		R1, R2, P1, P2, Q, 1024, -1, Size(), &ROI1, &ROI2);
	//DEPTH TESTER
	Mat depthL = imread("./data/depth/CameraL-Depth-.png");
	Mat depthR = imread("./data/depth/CameraR-Depth-.png");
	
	/*rectangle(depthL, ROI1, Scalar(255, 0, 0), 4, 8, 0);
	rectangle(depthR, ROI2, Scalar(255, 0, 0), 4, 8, 0);*/
	Mat map11, map12, map21, map22;
	initUndistortRectifyMap(
		cachedCameraCalibResult.cameraMatrix,
		cachedCameraCalibResult.distCoeffs,
		R1, P1, _imgSize, CV_32F, map11, map12);
	cv::initUndistortRectifyMap(
		cachedCameraCalibResult.cameraMatrix,
		cachedCameraCalibResult.distCoeffs,
		R2, P2, _imgSize, CV_32F, map21, map22);

	Mat depth1Rmpd, depth2Rmpd;
	

	cv::remap(depthL, depth1Rmpd, map11, map12, INTER_LINEAR);
	remap(depthR, depth2Rmpd, map21, map22, INTER_LINEAR);

	/*rectangle(depth1Rmpd, ROI1, Scalar(255, 0, 0), 4, 8, 0);
	rectangle(depth2Rmpd, ROI2, Scalar(255, 0, 0), 4, 8, 0);*/


	//imshow("Main", depth1Rmpd);
	/*depth1Af.create(depthL.size(), CV_64F);
	auto depthAfSize = depthL.size();
	warpAffine(depthL, depth1Af, R1, depthAfSize);
	warpAffine(depthR, depth2Af, R2, depthAfSize);
	*/
	//waitKey(0);

	/*Mat depthFull;
	cv::hconcat(depth1Rmpd, depth2Rmpd, depthFull);
	namedWindow("Main", WINDOW_NORMAL);
	resizeWindow("Main", 1024 , 1024);
	imshow("Main", depthFull);
	waitKey(0);*/

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
	threshold(depthRGray, depthRGray, thresh, maxValue, THRESH_BINARY);

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
	sort(vecL.begin(), vecL.end(), comp);
	sort(vecR.begin(), vecR.end(), comp);

	Mat outMat(1, keypointsL.size(), CV_64F);
	triangulatePoints(P1, P2, vecL, vecR, outMat);
	for (int j = 0; j < vecL.size(); j++) {
		cout << "Coord" << endl;
		cout << "(";
		//To Decard system
		float w = outMat.at<float>(3, j);
		for (int i = 0; i < 4; i++) {
			cout << " " << (outMat.at<float>(i, j) / w)  << " ";
		}
		cout << ")\n";
	}
	
	waitKey(0);

	//cout << R1 << endl << R2 << endl << P1 << endl << P2 << endl << Q << endl;
	return 0;
	//OLD
	//FindDistancions((float)cameraMatrix.at<double>(0, 0));
	//auto focalLen = result.cameraMatrix.at<float>(0, 0);
	//cout << "Focal Len=" << focalLen << endl;
	//http://www.vicon.com/hardware/cameras/vero/
}