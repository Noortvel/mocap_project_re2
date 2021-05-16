#pragma once
#include <vector>
#include <string>
#include <opencv2\opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <memory>

using namespace cv;
using namespace std;

class DnnPoseDetectorCUDA
{
private:
	Ort::Env env;
	Ort::SessionOptions session_options;
	Ort::Session session;
	OrtCUDAProviderOptions cudaProviderOpetions;
	Ort::MemoryInfo memory_info;

public:
	const int64_t width = 192;
	const int64_t height = 256;
	const int64_t channels = 3;
	const int64_t batch = 1;
	const int64_t input_tensor_size = batch * channels * height * width;
	const std::vector<int64_t> input_node_dims = { batch, channels, height, width };

	//const wchar_t model_path[43] { L"pose_hrnet_1x3x256x192.onnx" };

	const std::vector<const char*> input_node_names = { "input.1" };
	const std::vector<const char*> output_node_names = { "2873" };
public:
	DnnPoseDetectorCUDA(const wchar_t* modelPath);
private:
	vector<Point2f> keypoints{ KEYPOINTS_COUNT };
private:
	void NormalizeMeanStd(Mat& input);
	void HeatmapToKeypoints(const float* outputPtr);
	void FitToSourceImage(const Mat& blob);

	void _showHeatMap(const Mat& src, const Mat& heatmapsBlob);
	const string& CocoKeyPointToString(int keypoint);
public:
	void _drawKeyPoints(const Mat& src);
	void drawKeyPoints(Mat& src, float xScale, float yScale, float addX, float addY);

public:
	const vector<Point2f>& Forward(const Mat& input);
	const vector<Point2f>& GetKeypoints();

	const size_t KEYPOINTS_COUNT = 17;
	const Size INPUT_SIZE = cv::Size(192, 256);
	const Size OUT_SIZE = Size(48, 64);
	const Vec3f NORMALIZE_MEAN = Vec3f(0.485f, 0.456f, 0.406f);
	const Vec3f NORMALIZE_STD = Vec3f(0.229f, 0.224f, 0.225f);
};

