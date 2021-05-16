#include "MotionCapter.h"
#include <opencv2\opencv.hpp>
#include <vector>



using namespace std;

/*
void openmocap2::MotionCapter::Start(vector<vector<Keypoint>>& animation)
{
	DnnBoxDetectorCUDA boxDetector(L"dnn_models/yolov5s_640x640.onnx");
	DnnPoseDetectorCUDA poseDetector(L"dnn_models/pose_hrnet_1x3x256x192.onnx");

	

	VideoCapture videoL("./data/video/camera0_007.mp4");
	VideoCapture videoR("./data/video/camera1_007.mp4");

    const size_t keypointsCount = poseDetector.KEYPOINTS_COUNT;
    openmocap2::FullStereoCalibration stereoCalib;
    stereoCalib.Calibrate();

    float cWidthL = videoL.get(cv::CAP_PROP_FRAME_WIDTH);
    float cHeightL = videoL.get(cv::CAP_PROP_FRAME_HEIGHT);
    float scaleXL = cWidthL / boxDetector.width;
    float scaleYL = cHeightL / boxDetector.height;
    
    float cWidthR = videoR.get(cv::CAP_PROP_FRAME_WIDTH);
    float cHeightR = videoR.get(cv::CAP_PROP_FRAME_HEIGHT);
    float scaleXR = cWidthR / boxDetector.width;
    float scaleYR = cHeightR / boxDetector.height;

	vector<Point2f> keyPointsL;
	vector<Point2f> keyPointsR;
    vector<Point3f> tKeypoints3D (17);


    Rect humanRectL;
    Rect humanRectR;
	
    Mat keypoints3D(1, 17, CV_32FC4);

	while (true)
	{
		Mat frameL, frameR;

		bool succes = videoL.read(frameL) && videoR.read(frameR);

        if(!succes){
            break;
        }

		boxDetector.Forward(frameL);
        bool isLRect = boxDetector.TryGetHumanBox(humanRectL);
        boxDetector.Forward(frameR);
        bool isRRect = boxDetector.TryGetHumanBox(humanRectR);

        if (isLRect && isRRect) {
            //Crop and pass to dnn
            Rect mutableRectL(humanRectL);
            Rect mutableRectR(humanRectR);

            FitRect(mutableRectL, scaleXL, scaleYL, cWidthL, cHeightL);
            FitRect(mutableRectR, scaleXR, scaleYR, cWidthR, cHeightR);
            Mat frameHumanL(frameL, mutableRectL);
            Mat frameHumanR(frameR, mutableRectR);

            keyPointsL = poseDetector.Forward(frameHumanL);
            keyPointsR = poseDetector.Forward(frameHumanR);

            //Shift by corner of rect
            std::transform(keyPointsL.begin(), keyPointsL.end(), keyPointsL.begin(),
                [mutableRectL](Point2f& p) -> Point2f& {
                    p.x += mutableRectL.x;
                    p.y += mutableRectL.y;
                    return p;
                });
            std::transform(keyPointsR.begin(), keyPointsR.end(), keyPointsR.begin(),
                [mutableRectR](Point2f& p) -> Point2f& {
                    p.x += mutableRectR.x;
                    p.y += mutableRectR.y;
                    return p;
                });

            //Remap and triangulate
            cv::remap(frameL, frameL,
                stereoCalib.InitUndistortRectifyMap1().map1, stereoCalib.InitUndistortRectifyMap1().map2, INTER_LINEAR);
            cv::remap(frameR, frameR,
                stereoCalib.InitUndistortRectifyMap2().map1, stereoCalib.InitUndistortRectifyMap2().map2, INTER_LINEAR);

            cv::triangulatePoints(stereoCalib.Rectify().P1, stereoCalib.Rectify().P2, keyPointsL, keyPointsR, keypoints3D); //keypoints3D

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
        }
	}
}
*/

openmocap2::MotionCapter::MotionCapter()
{
    stereoCalib.Calibrate();
}

bool openmocap2::MotionCapter::TryGrab(VideoCapture& left, VideoCapture& right, vector<Point3f>& outPoints)
{
    const size_t keypointsCount = poseDetector.KEYPOINTS_COUNT;
    float cWidthL = left.get(cv::CAP_PROP_FRAME_WIDTH);
    float cHeightL = left.get(cv::CAP_PROP_FRAME_HEIGHT);
    float scaleXL = cWidthL / boxDetector.width;
    float scaleYL = cHeightL / boxDetector.height;

    float cWidthR = right.get(cv::CAP_PROP_FRAME_WIDTH);
    float cHeightR = right.get(cv::CAP_PROP_FRAME_HEIGHT);
    float scaleXR = cWidthR / boxDetector.width;
    float scaleYR = cHeightR / boxDetector.height;

    vector<Point2f> keyPointsL;
    vector<Point2f> keyPointsR;

    Rect humanRectL;
    Rect humanRectR;

    Mat keypoints3D(1, 17, CV_32FC4);

  
    Mat frameL, frameR;

    bool succes = left.read(frameL) && right.read(frameR);

    if (!succes) {
        return false;
    }

    boxDetector.Forward(frameL);
    bool isLRect = boxDetector.TryGetHumanBox(humanRectL);
    boxDetector.Forward(frameR);
    bool isRRect = boxDetector.TryGetHumanBox(humanRectR);

    if (isLRect && isRRect) {
        //Crop and pass to dnn
        Rect mutableRectL(humanRectL);
        Rect mutableRectR(humanRectR);

        FitRect(mutableRectL, scaleXL, scaleYL, cWidthL, cHeightL);
        FitRect(mutableRectR, scaleXR, scaleYR, cWidthR, cHeightR);
        Mat frameHumanL(frameL, mutableRectL);
        Mat frameHumanR(frameR, mutableRectR);

        keyPointsL = poseDetector.Forward(frameHumanL);
        keyPointsR = poseDetector.Forward(frameHumanR);

        //Shift by corner of rect
        std::transform(keyPointsL.begin(), keyPointsL.end(), keyPointsL.begin(),
            [mutableRectL](Point2f& p) -> Point2f& {
            p.x += mutableRectL.x;
            p.y += mutableRectL.y;
            return p;
        });
        std::transform(keyPointsR.begin(), keyPointsR.end(), keyPointsR.begin(),
            [mutableRectR](Point2f& p) -> Point2f& {
            p.x += mutableRectR.x;
            p.y += mutableRectR.y;
            return p;
        });

        //Remap and triangulate
        cv::remap(frameL, frameL,
            stereoCalib.InitUndistortRectifyMap1().map1, stereoCalib.InitUndistortRectifyMap1().map2, INTER_LINEAR);
        cv::remap(frameR, frameR,
            stereoCalib.InitUndistortRectifyMap2().map1, stereoCalib.InitUndistortRectifyMap2().map2, INTER_LINEAR);

        cv::triangulatePoints(stereoCalib.Rectify().P1, stereoCalib.Rectify().P2, keyPointsL, keyPointsR, keypoints3D); //keypoints3D

        for (int j = 0; j < keypointsCount; j++) {
            //To Decard system
            float w = keypoints3D.at<float>(3, j);
            outPoints.push_back(Point3f(
                keypoints3D.at<float>(0, j) / w,
                -keypoints3D.at<float>(1, j) / w,
                keypoints3D.at<float>(2, j) / w));
        }
    }
    
}

void openmocap2::MotionCapter::FitRect(
    cv::Rect& mutableRect, float scaleX, float scaleY, float fitWidth, float fitHeight)
{
    mutableRect.height = mutableRect.height * scaleY;
    mutableRect.width = mutableRect.width * scaleX;
    mutableRect.x = mutableRect.x * scaleX;
    mutableRect.y = mutableRect.y * scaleY;
    if (mutableRect.x < 0) {
        mutableRect.x = 0;
    }
    if (mutableRect.y < 0) {
        mutableRect.y = 0;
    }
    if (mutableRect.x + mutableRect.width > fitWidth) {
        mutableRect.width = fitWidth - mutableRect.x - 1;
    }
    if (mutableRect.y + mutableRect.height > fitHeight) {
        mutableRect.height = fitHeight - mutableRect.y - 1;
    }
}


void LiveDemo2() {
    VideoCapture cam("./camera0_007.mp4");
    //VideoCapture cam(0);


    float cWidth = cam.get(cv::CAP_PROP_FRAME_WIDTH);
    float cHeight = cam.get(cv::CAP_PROP_FRAME_HEIGHT);

    DnnBoxDetectorCUDA boxDetector(L"dnn_models/yolov5s_640x640.onnx");
    DnnPoseDetectorCUDA dnnDetector(L"dnn_models/pose_hrnet_1x3x256x192.onnx");

    float scaleX = cWidth / boxDetector.width;
    float scaleY = cHeight / boxDetector.height;

    String window_name = "My First Video";
    namedWindow(window_name, WINDOW_NORMAL);

    while (true)
    {
        Mat img;
        bool isSucces = cam.read(img);
        if (isSucces) {

            boxDetector.Forward(img);
            cv::Rect rect;
            bool isHumanFinded = boxDetector.TryGetHumanBox(rect);

            if (isHumanFinded) {
                Rect mutableRect(rect);

                mutableRect.height = mutableRect.height * scaleY;
                mutableRect.width = mutableRect.width * scaleX;
                mutableRect.x = mutableRect.x * scaleX;
                mutableRect.y = mutableRect.y * scaleY;
                if (mutableRect.x < 0) {
                    mutableRect.x = 0;
                }
                if (mutableRect.y < 0) {
                    mutableRect.y = 0;
                }
                if (mutableRect.x + mutableRect.width > cWidth) {
                    mutableRect.width = cWidth - mutableRect.x - 1;
                }
                if (mutableRect.y + mutableRect.height > cHeight) {
                    mutableRect.height = cHeight - mutableRect.y - 1;
                }

                Mat croped(img, mutableRect);
                dnnDetector.Forward(croped);
                cv::rectangle(img, mutableRect, Scalar(255, 255, 255), 4);
                dnnDetector.drawKeyPoints(img, 1, 1, mutableRect.x, mutableRect.y);
            }
            cv::imshow(window_name, img);
            if (cv::waitKey(10) == 27)
            {
                std::cout << "Esc key is pressed by user. Stoppig the video" << endl;
                break;
            }
        }
    }
}

