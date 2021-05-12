#include "MotionCapter.h"
#include <opencv2\opencv.hpp>
#include <vector>
#include "FullStereoCalibration.h"
#include "DnnHumanPoseDetector.h"
#include "DnnHumanBoxDetector.h"


using namespace std;

void openmocap2::MotionCapter::Start(vector<vector<Keypoint>>& animation)
{
	DnnHumanBoxDetector boxDetector;

	DnnHumanPoseDetector dnnDetector;
	const size_t keypointsCount = dnnDetector.KEYPOINTS_COUNT;
	openmocap2::FullStereoCalibration stereoCalib;
	stereoCalib.Calibrate();
	VideoCapture videoL("./data/video/camera0_007.mp4");
	VideoCapture videoR("./data/video/camera1_007.mp4");

	vector<Point> keyPointsL;
	vector<Point> keyPointsR;
	
	Mat keypoints3D(1, 17, CV_64F);

	
	while (true)
	{
		Mat frameL, frameR;

		bool succes = videoL.read(frameL) && videoR.read(frameR);

		if (succes == false)
		{
			std::cout << "Found the end of the video" << endl;
			break;
		}

		boxDetector.Forward(frameL);

		keyPointsL = dnnDetector.Forward(frameL);
		keyPointsR = dnnDetector.Forward(frameR);

		cv::remap(frameL, frameL,
			stereoCalib.InitUndistortRectifyMap1().map1, stereoCalib.InitUndistortRectifyMap1().map2, INTER_LINEAR);
		cv::remap(frameR, frameR,
			stereoCalib.InitUndistortRectifyMap2().map1, stereoCalib.InitUndistortRectifyMap2().map2, INTER_LINEAR);
		cv::triangulatePoints(stereoCalib.Rectify().P1, stereoCalib.Rectify().P2, keyPointsL, keyPointsR, keypoints3D);
		
		vector<Keypoint> keypointsInner;
		for (int j = 0; j < keypointsCount; j++) {
			//To Decard system
			float w = keypoints3D.at<float>(3, j);
			Keypoint kp;
			kp.position.x = keypoints3D.at<float>(0, j) / w;
			kp.position.y = -keypoints3D.at<float>(1, j) / w; // Coord y is reversed in image coord system
			kp.position.z = keypoints3D.at<float>(2, j) / w;
			kp.time = 0.1;
			keypointsInner.push_back(kp);
		}
		animation.push_back(keypointsInner);
		break;
	}
}
