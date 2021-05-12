#pragma once
#include <opencv2\opencv.hpp>
#include <vector>
#include <string>

namespace openmocap2 {
	using namespace std;
	using namespace cv;
	class DnnHumanBoxDetector
	{
	private:
		dnn::Net yolov5s;
		vector<Point> boxCorners;
	public:
		DnnHumanBoxDetector();
		const vector<Point>& Forward(const cv::Mat& img);
		const cv::Size INPUT_SIZE = cv::Size(640, 640);
		//ssd_mobilenet_v1_10.onnx
		//yolov5s_4_640x640.onnx
		const string MODEL_PATH = "./dnn_models/ssd_mobilenet_v1_10.onnx"; //yolov5s_640x640.onnx
	};
}

