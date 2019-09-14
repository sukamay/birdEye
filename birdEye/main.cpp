////
////  main.cpp
////  birdEye
////
////  Created by 罗媚 on 2019/5/20.
////  Copyright © 2019 罗媚. All rights reserved.
////
//
//#include <iostream>
//#include <iomanip>
//#include <vector>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/videoio/videoio.hpp>
//#include <opencv2/imgcodecs/imgcodecs.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/opencv.hpp>
//#include <opencv2/features2d.hpp>
//
//#include "CameraCalibrator.h"
//
//using namespace std;
//using namespace cv;
////
////int main(int argc, const char * argv[])
////{
////    Mat image;
////    image = imread("/Users/luomei/Desktop/source/pic/DesktopBG/IMG_6118.JPG",-1);
////    imshow("origin", InputArray(image));
////    waitKey(0);
////    return 0;
////}
////
//#include <iostream>
//#include <iomanip>
//#include <vector>
//#include <opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/features2d.hpp>
//
//using namespace std;
//using namespace cv;

////#include "CameraCalibrator.h"
//
//#define PATH "/Users/luomei/Documents/MATLAB/Image/Image"
//#define PICS_NUM 20
//
//int main()
//{
//    cv::Mat image;
//    std::vector<std::string> filelist;
//
//    // generate list of chessboard image filename
//    // named chessboard01 to chessboard27 in chessboard sub-dir
//    for (int i=1; i<=PICS_NUM; i++) {
//
//        std::stringstream str;
////        str << PATH << std::setw(2) << std::setfill('0') << i << ".png";
//        str << PATH << i << ".png";
//        std::cout << str.str() << std::endl;
//
//        filelist.push_back(str.str());
//        image= cv::imread(str.str(),0);
//
//        //   cv::imshow("Board Image",image);
//        //  cv::waitKey(500);
//    }
//
//    // Create calibrator object
//    CameraCalibrator cameraCalibrator;
//    // add the corners from the chessboard
//    cv::Size boardSize(8,6);
//    cameraCalibrator.addChessboardPoints(
//                                         filelist,   // filenames of chessboard image
//                                         boardSize, "Detected points");  // size of chessboard
//
//    // calibrate the camera
//    cameraCalibrator.setCalibrationFlag(true,true);
//    cameraCalibrator.calibrate(image.size());
//
//    // Exampple of Image Undistortion
//    std::cout << filelist[5] << std::endl;
//    image = cv::imread(filelist[5],0);
//    cv::Size newSize(static_cast<int>(image.cols*1.5), static_cast<int>(image.rows*1.5));
//    cv::Mat uImage= cameraCalibrator.remap(image, newSize);
//
//    // display camera matrix
//    cv::Mat cameraMatrix= cameraCalibrator.getCameraMatrix();
//    cv::Mat distCoeffs=cameraCalibrator.getDistCoeffs();
//    std::cout << " Camera intrinsic: " << cameraMatrix.rows << "x" << cameraMatrix.cols <<  std::endl;
//    std::cout << cameraMatrix.at<double>(0,0) << " " << cameraMatrix.at<double>(0,1) << " " << cameraMatrix.at<double>(0,2) << std::endl;
//    std::cout << cameraMatrix.at<double>(1,0) << " " << cameraMatrix.at<double>(1,1) << " " << cameraMatrix.at<double>(1,2) << std::endl;
//    std::cout << cameraMatrix.at<double>(2,0) << " " << cameraMatrix.at<double>(2,1) << " " << cameraMatrix.at<double>(2,2) << std::endl;
//    std::cout << distCoeffs.rows << "x" <<distCoeffs.cols << std::endl;
//    std::cout << distCoeffs << std::endl;
//    for(int i = 0;i < distCoeffs.cols;i++)
//    {
//        std::cout << distCoeffs.at<double>(0,i) << " " ;
//    }
//    std::cout <<std::endl;
//
//    cv::namedWindow("Original Image");
//    cv::imshow("Original Image", image);
//    cv::namedWindow("Undistorted Image");
//    cv::imshow("Undistorted Image", uImage);
//
//    // Store everything in a xml file
//    cv::FileStorage fs("calib.xml", cv::FileStorage::WRITE);
//    fs << "Intrinsic" << cameraMatrix;
//    fs << "Distortion" << cameraCalibrator.getDistCoeffs();
//
//    cv::waitKey();
//    return 0;
//}

