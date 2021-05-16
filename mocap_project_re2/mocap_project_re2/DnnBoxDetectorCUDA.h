#pragma once
#include <vector>
#include <string>
#include <opencv2\opencv.hpp>
#include <onnxruntime_cxx_api.h>
#include <memory>

class DnnBoxDetectorCUDA
{

private:
	Ort::Env env;
	Ort::SessionOptions session_options;
	Ort::Session session;
	OrtCUDAProviderOptions cudaProviderOpetions;
	Ort::MemoryInfo memory_info;

public:
	const int64_t width = 640;
	const int64_t height = 640;
	const int64_t channels = 3;
	const int64_t input_tensor_size = width * height * channels;
	const std::vector<int64_t> input_node_dims = { 1, channels, height, width };

	//const wchar_t model_path[32] { L"dnn_models/yolov5s_640x640.onnx" };
	const std::vector<const char*> input_node_names = { "images" };
	const std::vector<const char*> output_node_names = { "561", "397", "458", "519" }; //S
public:
	DnnBoxDetectorCUDA(const wchar_t* modelPath);
	void Forward(const cv::Mat& source);
	const std::vector<cv::Rect>& GetRects();
	bool TryGetHumanBox(cv::Rect& outRec) const;

private:
	const int personLabel = 0;
private:
	std::vector<cv::Vec4f> locations;
	std::vector<int> labels;
	std::vector<float> confidences;

	std::vector<cv::Rect> src_rects;
	std::vector<cv::Rect> res_rects;
	std::vector<int> res_indexs;

	cv::Rect rect;
	cv::Vec4f location;

	void DecodeOutput(float* output);
};

