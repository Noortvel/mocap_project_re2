#pragma once
#include <vector>
#include <opencv2\opencv.hpp>
#include "Keypoint.h"
#include "FullStereoCalibration.h"
#include "DnnPoseDetectorCUDA.h"
#include "DnnBoxDetectorCUDA.h"


namespace openmocap2 {
	using namespace std;
	using namespace cv;
	class MotionCapter
	{
	public:
		MotionCapter();
		bool TryGrab(VideoCapture& left, VideoCapture& right, vector<Point3f>& outPoints);
	private:
		FullStereoCalibration stereoCalib;
		DnnBoxDetectorCUDA boxDetector{ L"dnn_models/yolov5s_640x640.onnx" };
		DnnPoseDetectorCUDA poseDetector{ L"dnn_models/pose_hrnet_1x3x256x192.onnx" };
		vector<Point3f> capturedPoints;
	private:
		void FitRect(cv::Rect& mutableRect, float scaleX, float scaleY, float fitWidth, float fitHeight);
	};
}

