#include <map>
#include <opencv2\opencv.hpp>
#include "DnnPoseDetectorCUDA.h"


using namespace cv;
using namespace std;

DnnPoseDetectorCUDA::DnnPoseDetectorCUDA(const wchar_t* modelPath):
    env(),
    session_options(),
    session(env, modelPath, session_options.AppendExecutionProvider_CUDA(cudaProviderOpetions)),
    memory_info(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault))
{
	for (size_t i = 0; i < KEYPOINTS_COUNT; i++) {
		keypoints.push_back(Point(0, 0));
	}
}



void DnnPoseDetectorCUDA::NormalizeMeanStd(cv::Mat& input)
{
	Mat meanNrm, stdNrm;
	cv::meanStdDev(input, meanNrm, stdNrm);
	for (int i = 0; i < input.cols; i++) {
		for (int j = 0; j < input.rows; j++) {
			auto& intensity = input.at<Vec3f>(j, i);
			for (int k = 0; k < input.channels(); k++) {
				intensity.val[k] -= meanNrm.at<double>(k);
				intensity.val[k] /= stdNrm.at<double>(k);
				intensity.val[k] *= NORMALIZE_STD[k];
				intensity.val[k] += NORMALIZE_MEAN[k];
			}
		}
	}
}

void DnnPoseDetectorCUDA::HeatmapToKeypoints(const float* outputPtr)
{
	//TODO: find in blob directly
	Mat hm = Mat(OUT_SIZE, CV_32F);
	Point maxPoint;
	for (size_t layer = 0; layer < KEYPOINTS_COUNT; layer++) {
		for (size_t y = 0; y < OUT_SIZE.height; y++) {
			for (size_t x = 0; x < OUT_SIZE.width; x++) {
				float& val = hm.at<float>(y, x);
				size_t indx = layer * OUT_SIZE.width * OUT_SIZE.height + y * OUT_SIZE.width + x;
				float srcVal = outputPtr[indx];
				val = srcVal;
			}
		}

		cv::minMaxLoc(hm, nullptr, nullptr, nullptr, &maxPoint);
		int nx = (int)roundf(((float)maxPoint.x / (float)OUT_SIZE.width) * INPUT_SIZE.width);
		int ny = (int)roundf(((float)maxPoint.y / (float)OUT_SIZE.height) * INPUT_SIZE.height);
		Point2f& kp = keypoints[layer];
		kp.x = nx;
		kp.y = ny;
	}
}

void DnnPoseDetectorCUDA::FitToSourceImage(const Mat& blob)
{
	for (size_t i = 0; i < keypoints.size(); i++) {
		auto& kp = keypoints[i];
		int nx = (int)roundf(((float)kp.x / (float)INPUT_SIZE.width) * blob.cols);
		int ny = (int)roundf(((float)kp.y / (float)INPUT_SIZE.height) * blob.rows);
		kp.x = nx;
		kp.y = ny;
	}
}



void DnnPoseDetectorCUDA::_showHeatMap(const cv::Mat& src, const cv::Mat& heatmapsBlob)
{
	static Size fitSize(INPUT_SIZE.width * 3, INPUT_SIZE.height * 3);
	Mat srcFitted;
	cv::resize(src, srcFitted, fitSize);

	for (size_t layer = 0; layer < KEYPOINTS_COUNT; layer++) {
		Mat hm = Mat(Size(48, 64), heatmapsBlob.type());
		for (int y = 0; y < 64; y++) {
			for (int x = 0; x < 48; x++) {
				float& val = hm.at<float>(y, x);
				float srcVal = heatmapsBlob.at<float>(Vec<int, 4>(0, layer, y, x));
				val = srcVal;
			}
		}
		cv::resize(hm, hm, fitSize);
		cv::normalize(hm, hm, 0, 1, cv::NORM_MINMAX);
		cv::threshold(hm, hm, 0.5, 1, cv::THRESH_TOZERO);
		hm.convertTo(hm, CV_8UC3, 255);
		cv::cvtColor(hm, hm, COLOR_GRAY2BGR);
		cv::applyColorMap(hm, hm, COLORMAP_JET);

		Mat hmImageShow;
		cv::addWeighted(srcFitted, 0.5, hm, 1, 0, hmImageShow);
		cv::putText(hmImageShow, "layer: " + std::to_string(layer) + CocoKeyPointToString(layer), cv::Point(50, 50), cv::FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0));
		imshow("showHeatMaps", hmImageShow);
		waitKey();
	}
}

void DnnPoseDetectorCUDA::_drawKeyPoints(const Mat& src)
{
	Mat show;
	vector<KeyPoint> showPoints;
	for (size_t i = 0; i < keypoints.size(); i++) {
		auto& kpsrc = keypoints[i];
		showPoints.push_back(KeyPoint(kpsrc.x, kpsrc.y, 2));
	}
	drawKeypoints(src, showPoints, show, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("Points", show);
	waitKey();
}

void DnnPoseDetectorCUDA::drawKeyPoints(Mat& src, float xScale, float yScale, float addX, float addY)
{
	vector<KeyPoint> showPoints;
	for (size_t i = 0; i < keypoints.size(); i++) {
		auto& kpsrc = keypoints[i];
		showPoints.push_back(KeyPoint(kpsrc.x * xScale + addX, kpsrc.y * yScale + addY, 10));
	}
	drawKeypoints(src, showPoints, src, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
}



const std::vector<Point2f>& DnnPoseDetectorCUDA::Forward(const cv::Mat& source)
{
	Mat img;
	source.convertTo(img, CV_32FC3);
	cv::resize(img, img, INPUT_SIZE);
	cv::cvtColor(img, img, COLOR_BGR2RGB);
	NormalizeMeanStd(img);

	auto blob = cv::dnn::blobFromImage(img);

	Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, (float*)blob.data, input_tensor_size, input_node_dims.data(), 4);
	auto output_tensors = session.Run(Ort::RunOptions{ nullptr }, input_node_names.data(), &input_tensor, 1, output_node_names.data(), 1);
	auto& output_tensor = output_tensors;
	float* outputPtr = output_tensor[0].GetTensorMutableData<float>();

	HeatmapToKeypoints(outputPtr);
	FitToSourceImage(source);

	/*_drawKeyPoints(source);
	*/
	//_showHeatMap(source, heatMapsBlob);

	return keypoints;
}

const std::vector<Point2f>& DnnPoseDetectorCUDA::GetKeypoints()
{
	return keypoints;
}


const string& DnnPoseDetectorCUDA::CocoKeyPointToString(int keypoint) {
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
