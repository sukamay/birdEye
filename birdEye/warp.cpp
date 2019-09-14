//
//  warp.cpp
//  birdEye
//
//  Created by 罗媚 on 2019/5/29.
//  Copyright © 2019 罗媚. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>


using namespace cv;
using namespace std;


cv::Mat get_perspective_mat()
{
//    cv::Point2f src_points[] = {
//        cv::Point2f(165, 270),
//        cv::Point2f(835, 270),
//        cv::Point2f(360, 125),
//        cv::Point2f(615, 125) };
//
//    cv::Point2f dst_points[] = {
//        cv::Point2f(165, 270),
//        cv::Point2f(835, 270),
//        cv::Point2f(165, 30),
//        cv::Point2f(835, 30) };
    cv::Point2f src_points[] = {
        cv::Point2f(1095, 1020),
        cv::Point2f(1450, 1011),
        cv::Point2f(1319, 733),
        cv::Point2f(1062, 736) };
    
    cv::Point2f dst_points[] = {
        cv::Point2f(1064, 274),
        cv::Point2f(1366, 289),
        cv::Point2f(1341, 25),
        cv::Point2f(1061, 10) };
    
    cv::Mat M = cv::getPerspectiveTransform(src_points, dst_points);
    
    return M;
}

//int main(){
int warp(){
    Mat image = imread("/Users/luomei/Documents/MATLAB/img3.jpg");
    Mat M = get_perspective_mat();
    cout << "H " << endl << M << endl;
    cv::Mat perspective;
    cv::warpPerspective(image, perspective, M, cv::Size(960, 270), cv::INTER_LINEAR);
    imshow("warp res", perspective);
    imwrite("output_img1_res.jpg", perspective);
    waitKey();
    return 0;
}
