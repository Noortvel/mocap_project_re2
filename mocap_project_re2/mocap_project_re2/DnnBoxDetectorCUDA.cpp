#include "DnnBoxDetectorCUDA.h"

using namespace cv;

DnnBoxDetectorCUDA::DnnBoxDetectorCUDA(const wchar_t* modelPath)
    :
    env(),
    session_options(),
    session(env, modelPath, session_options.AppendExecutionProvider_CUDA(cudaProviderOpetions)),
    memory_info(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault))
{

}

void DnnBoxDetectorCUDA::Forward(const cv::Mat& source)
{
	auto blob = dnn::blobFromImage(source, 1 / (double)255, Size(width, height), Scalar(0, 0, 0), true, false);
	Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, (float*)blob.data, input_tensor_size, input_node_dims.data(), 4);
	auto output_tensors = session.Run(Ort::RunOptions{ nullptr }, input_node_names.data(), &input_tensor, 1, output_node_names.data(), 4);
	auto& output_tensor = output_tensors;
    DecodeOutput(output_tensor[0].GetTensorMutableData<float>());
}

const std::vector<cv::Rect>& DnnBoxDetectorCUDA::GetRects()
{
    return src_rects;
}

bool DnnBoxDetectorCUDA::TryGetHumanBox(cv::Rect& outRec) const
{
    if (src_rects.size() == 0) {
        return false;
    }

    auto it = std::find(labels.begin(), labels.end(), personLabel);
    if (it == labels.end()) {
        return false;
    }
    int index = it - labels.begin();
    outRec = src_rects[index];
    return true;
}

void DnnBoxDetectorCUDA::DecodeOutput(float* output)
{
    size_t size = 2142000;// output_tensor[0].GetTensorTypeAndShapeInfo().GetElementCount(); // 1x25200x85=2142000
    int dimensions = 85; // 0,1,2,3 ->box,4->confidence，5-85 -> coco classes confidence 
    int rows = size / dimensions; //25200
    int confidenceIndex = 4;
    int labelStartIndex = 5;
    float modelWidth = 640.0;
    float modelHeight = 640.0;
    float xGain = modelWidth / width;
    float yGain = modelHeight / height;

    confidences.clear();
    locations.clear();
    src_rects.clear();
    labels.clear();
    res_indexs.clear();

    for (int i = 0; i < rows; ++i) {
        int index = i * dimensions;
        if (output[index + confidenceIndex] <= 0.4f) continue;

        for (int j = labelStartIndex; j < dimensions; ++j) {
            output[index + j] = output[index + j] * output[index + confidenceIndex];
        }

        for (int k = labelStartIndex; k < dimensions; ++k) {
            if (output[index + k] <= 0.5f) continue;

            location[0] = (output[index] - output[index + 2] / 2) / xGain;//top left x
            location[1] = (output[index + 1] - output[index + 3] / 2) / yGain;//top left y
            location[2] = (output[index] + output[index + 2] / 2) / xGain;//bottom right x
            location[3] = (output[index + 1] + output[index + 3] / 2) / yGain;//bottom right y

            locations.emplace_back(location);

            rect = cv::Rect(location[0], location[1],
                location[2] - location[0], location[3] - location[1]);
            src_rects.push_back(rect);
            labels.emplace_back(k - labelStartIndex);
            confidences.emplace_back(output[index + k]);
        }
    }
}
