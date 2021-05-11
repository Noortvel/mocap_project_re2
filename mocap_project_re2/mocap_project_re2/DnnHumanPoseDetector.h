#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class DnnHumanPoseDetector
{

private:
	vector<Point> keypoints { KEYPOINTS_COUNT };
	dnn::Net hrNetPose;
private:
	void NormalizeMeanStd(Mat& input);
	void HeatmapToKeypoints(const Mat& blob);
	void FitToSourceImage(const Mat& blob);

	void _showHeatMap(const Mat& src, const Mat& heatmapsBlob);
	void _drawKeyPoints(const Mat& src);
	const string& CocoKeyPointToString(int keypoint);

public:
	DnnHumanPoseDetector();

	const vector<Point>& Forward(const Mat& input);
	const vector<Point>& GetKeypoints();
	
	const size_t KEYPOINTS_COUNT = 17;
	const Size INPUT_SIZE = cv::Size(192, 256);
	const Size OUT_SIZE = Size(48, 64);
	const Vec3f NORMALIZE_MEAN = Vec3f(0.485f, 0.456f, 0.406f);
	const Vec3f NORMALIZE_STD = Vec3f(0.229f, 0.224f, 0.225f);
	const string MODEL_PATH = "./dnn_models/pose_hrnet_w32_256x192.onnx";
};

