//
//  birdEye.cpp
//  birdEye
//
//  Created by 罗媚 on 2019/5/21.
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

int initH(string filename, int order, bool show=true)
{
    
    Size image_size;   /* 图像的尺寸 */
    int board_w = 8, board_h = 6;
    int board_num = board_h * board_w;
    Size board_size = Size(board_w,board_h);          /* 标定板上每行、列的角点数 */
    Mat intrinsic;
    Mat distortion;
    
    FileStorage fs("pre_paras/Intrinsics.xml", FileStorage::READ);
    read(fs["intrisics_matrix"], intrinsic);
    cout << "intrisics_matrix : \n" << intrinsic << endl;
    FileStorage fd("pre_paras/Distortion.xml", FileStorage::READ);
    read(fd["distortion_matrix"], distortion);
    cout << "distortion_matrix: \n" << distortion << endl;
    
    vector<Point2f> image_points_buf;   /* 缓存每幅图像上检测到的角点 */
    vector<vector<Point2f>> image_points_seq; /* 保存检测到的所有角点 */
//    string filename = "Image1/Image15.jpg";
    
    Mat image = imread(filename);
    image_size.width = image.cols;
    image_size.height = image.rows;
    
    Mat mapx = Mat(image_size,CV_32FC1);
    Mat mapy = Mat(image_size,CV_32FC1);
    Mat R = Mat::eye(3,3,CV_32F);
    
    initUndistortRectifyMap(intrinsic,distortion,R,intrinsic,image_size,CV_32FC1,mapx,mapy);
    Mat newimage = image.clone();
    
    //另一种不需要转换矩阵的方式
    remap(newimage,image,mapx,mapy, INTER_LINEAR);
   
        /* 提取角点 */
    if(0 == findChessboardCorners(image,board_size,image_points_buf)){
        cout<<"cannot find chessboard corners!\n"; //找不到角点
        exit(1);
    }
    Mat view_gray;
    cvtColor(image,view_gray,CV_RGB2GRAY);
    /* 亚像素精确化 */
//    find4QuadCornerSubpix(view_gray,image_points_buf,Size(5,5));//对粗提取的角点进行精确化
    cornerSubPix(view_gray,image_points_buf,Size(5,5),Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,0.1));
    image_points_seq.push_back(image_points_buf);    //保存亚像素角点
    
    Point2f object_points[4], image_points[4];
    object_points[0].x = 0; object_points[0].y = 0;
    object_points[1].x = board_w - 1; object_points[1].y = 0;
    object_points[2].x = 0; object_points[2].y = board_h - 1;
    object_points[3].x = board_w - 1; object_points[3].y = board_h - 1;
    image_points[0] = image_points_buf[0];
    image_points[1] = image_points_buf[board_w - 1];
    image_points[2] = image_points_buf[(board_h - 1) * board_w];
    image_points[3] = image_points_buf[board_h * board_w - 1];
    
    /* 显示四角，颜色顺序为：蓝，绿，红，黄*/
    circle(image, image_points[0], 9, CV_RGB(0, 0, 255));
    circle(image, image_points[1], 9, CV_RGB(0, 255, 0));
    circle(image, image_points[2], 9, CV_RGB(255, 0, 0));
    circle(image, image_points[3], 9, CV_RGB(255, 255, 0));
    
    /* 在图像上显示角点位置 */
    drawChessboardCorners(image,board_size,image_points_buf,false);//用于在图片中标记角点
    if(show){
        imshow("chessboard",image);//显示图片
        waitKey(500);//暂停0.5S
    }
    
    /* 求解单应矩阵*/
    Mat H = Mat(3, 3, CV_32FC1, Scalar::all(0));
    H = getPerspectiveTransform(object_points, image_points);
    cout << "H: \n" << H << endl;
    
    float z = 17;
    int key = 0;
    Mat bird_image = image.clone();
    
    std::stringstream StrStm;
    
//    while(key != 27){
//        CvMat cvH = H;
//        CV_MAT_ELEM(cvH, float, 2, 2) = z;
//        H = Mat(cvH.rows, cvH.cols, cvH.type, cvH.data.fl);
        H.at<double>(2, 2) = z;
        warpPerspective(image,
                        bird_image,
                        H,
                        image_size,
                        CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);
    if(show){
        imshow("Bird's Eye", bird_image);
        key = waitKey(1000);
    }
    
        if(key == 'u') z += 0.5;
        if(key == 'd') z -= 0.5;
        
//        string imageFileName = "";
//        StrStm<<order;
//        StrStm>>imageFileName;
//
//        imageFileName = "Image1/Image" + imageFileName + "_r.jpg";
//        imwrite(imageFileName, bird_image);
//    }
    
    FileStorage fh("H.xml", FileStorage::WRITE);
    fh << "H" << H;
    fh.release();
    std::cout<<"保存结束"<<endl;
    return 0;
}

Mat initH(Mat image, bool show=true)
{
    
    Size image_size;   /* 图像的尺寸 */
    int board_w = 8, board_h = 6;
    int board_num = board_h * board_w;
    Size board_size = Size(board_w,board_h);          /* 标定板上每行、列的角点数 */
    Mat intrinsic;
    Mat distortion;
    
    FileStorage fs("Intrinsics.xml", FileStorage::READ);
    read(fs["intrisics_matrix"], intrinsic);
    cout << "intrisics_matrix : \n" << intrinsic << endl;
    FileStorage fd("Distortion.xml", FileStorage::READ);
    read(fd["distortion_matrix"], distortion);
    cout << "distortion_matrix: \n" << distortion << endl;
    
    vector<Point2f> image_points_buf;   /* 缓存每幅图像上检测到的角点 */
    vector<vector<Point2f>> image_points_seq; /* 保存检测到的所有角点 */
    //    string filename = "Image1/Image15.jpg";
    
//    Mat image = imread(filename);
    image_size.width = image.cols;
    image_size.height = image.rows;
    
    Mat mapx = Mat(image_size,CV_32FC1);
    Mat mapy = Mat(image_size,CV_32FC1);
    Mat R = Mat::eye(3,3,CV_32F);
    
    initUndistortRectifyMap(intrinsic,distortion,R,intrinsic,image_size,CV_32FC1,mapx,mapy);
    Mat newimage = image.clone();
    
    //另一种不需要转换矩阵的方式
    remap(newimage,image,mapx,mapy, INTER_LINEAR);
    
    /* 提取角点 */
    if(0 == findChessboardCorners(image,board_size,image_points_buf)){
        cout<<"cannot find chessboard corners!\n"; //找不到角点
        return image;
    }
    Mat view_gray;
    cvtColor(image,view_gray,CV_RGB2GRAY);
    /* 亚像素精确化 */
    //    find4QuadCornerSubpix(view_gray,image_points_buf,Size(5,5));//对粗提取的角点进行精确化
    cornerSubPix(view_gray,image_points_buf,Size(5,5),Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,0.1));
    image_points_seq.push_back(image_points_buf);    //保存亚像素角点
    
    Point2f object_points[4], image_points[4];
    object_points[0].x = 0; object_points[0].y = 0;
    object_points[1].x = board_w - 1; object_points[1].y = 0;
    object_points[2].x = 0; object_points[2].y = board_h - 1;
    object_points[3].x = board_w - 1; object_points[3].y = board_h - 1;
    image_points[0] = image_points_buf[0];
    image_points[1] = image_points_buf[board_w - 1];
    image_points[2] = image_points_buf[(board_h - 1) * board_w];
    image_points[3] = image_points_buf[board_h * board_w - 1];
    
    /* 显示四角，颜色顺序为：蓝，绿，红，黄*/
    circle(image, image_points[0], 9, CV_RGB(0, 0, 255));
    circle(image, image_points[1], 9, CV_RGB(0, 255, 0));
    circle(image, image_points[2], 9, CV_RGB(255, 0, 0));
    circle(image, image_points[3], 9, CV_RGB(255, 255, 0));
    
    /* 在图像上显示角点位置 */
    drawChessboardCorners(image,board_size,image_points_buf,false);//用于在图片中标记角点
    if(show){
        imshow("chessboard",image);//显示图片
        waitKey(500);//暂停0.5S
    }
    
    /* 求解单应矩阵*/
    Mat H = Mat(3, 3, CV_32FC1, Scalar::all(0));
    H = getPerspectiveTransform(object_points, image_points);
    cout << "H: \n" << H << endl;
    
    float z = 17;
    int key = 0;
    Mat bird_image = image.clone();
    
    std::stringstream StrStm;
    
    //    while(key != 27){
    //        CvMat cvH = H;
    //        CV_MAT_ELEM(cvH, float, 2, 2) = z;
    //        H = Mat(cvH.rows, cvH.cols, cvH.type, cvH.data.fl);
    H.at<double>(2, 2) = z;
    warpPerspective(image,
                    bird_image,
                    H,
                    image_size,
                    CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);
    if(show){
        imshow("Bird's Eye", bird_image);
        key = waitKey();
    }
    
    if(key == 'u') z += 0.5;
    if(key == 'd') z -= 0.5;
    
    //        string imageFileName = "";
    //        StrStm<<order;
    //        StrStm>>imageFileName;
    //
    //        imageFileName = "Image1/Image" + imageFileName + "_r.jpg";
    //        imwrite(imageFileName, bird_image);
    //    }
    
    FileStorage fh("H.xml", FileStorage::WRITE);
    fh << "H" << H;
    fh.release();
    std::cout<<"保存结束"<<endl;
    return bird_image;
}

void temp(){
    string filename = "cv_res/Image_d/Image21_d.jpg";
    Mat image = imread(filename);
    Size image_size;
    image_size.width = image.cols;
    image_size.height = image.rows;
    
    Point2f object_points[4], image_points[4];
    object_points[0].x = 60; object_points[0].y = 176;
    object_points[1].x = 60; object_points[1].y = 226;
    object_points[2].x = -90; object_points[2].y = 176;
    object_points[3].x = -90; object_points[3].y = 226;
//    1198 1001
//    619 998
//    794 779
//    1108 780
    image_points[0].x = 1198; image_points[0].y = 1001;
    image_points[1].x = 1108; image_points[1].y = 780;
    image_points[2].x = 619; image_points[2].y = 998;
    image_points[3].x = 794; image_points[3].y = 779;
    
    circle(image, image_points[0], 9, CV_RGB(0, 0, 255));
    circle(image, image_points[1], 9, CV_RGB(0, 255, 0));
    circle(image, image_points[2], 9, CV_RGB(255, 0, 0));
    circle(image, image_points[3], 9, CV_RGB(255, 255, 0));
    
//    imshow("origin", image);
//    waitKey(1000);
    
    /* 求解单应矩阵*/
    Mat H = Mat(3, 3, CV_32FC1, Scalar::all(0));
    H = getPerspectiveTransform(object_points, image_points);
    cout << "H: \n" << H << endl;
    
    float z = 7;
    int key = 0;
    Mat bird_image = image.clone();
    
    std::stringstream StrStm;
    
    //    while(key != 27){
    //        CvMat cvH = H;
    //        CV_MAT_ELEM(cvH, float, 2, 2) = z;
    //        H = Mat(cvH.rows, cvH.cols, cvH.type, cvH.data.fl);
    bool show =  false;
    H.at<double>(2, 2) = z;
    warpPerspective(image,
                    bird_image,
                    H,
                    image_size,
                    CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);
    if(show){
        imshow("Bird's Eye", bird_image);
        key = waitKey();
    }
    
        VideoCapture cap;
        string videoName = "cv_res/test1.mp4";
        cap.open(videoName);
        if(!cap.isOpened()){
            cout << " fail to open the video: " << videoName << endl;
            return;
        }
        Size videoSize = Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        VideoWriter videoWriter("cv_res/test1_res.avi", -1, cap.get(CV_CAP_PROP_FPS), videoSize, true);
        Mat frame;
        Mat bird_frame;
        while (true) {
            cap>>frame;
            if(frame.empty()){
                break;
            }
    //        imwrite("input_frame0.jpg", frame);
            imshow("input frame", frame);
            waitKey(500);
//            bird_frame = initH(frame);
            warpPerspective(frame,
                            bird_frame,
                            H,
                            videoSize,
                            CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);
            imshow("bird eye", bird_frame);
            waitKey(500);
            videoWriter<<bird_frame;
        }
        cap.release();
 
    FileStorage fh("tempH.xml", FileStorage::WRITE);
    fh << "tempH" << H;
    fh.release();
    std::cout<<"保存结束"<<endl;
}


Mat initH(Mat image1, Mat image2, bool show=true)
{
    
    Size image_size;   /* 图像的尺寸 */
    int board_w = 8, board_h = 6;
    int board_num = board_h * board_w;
    Size board_size = Size(board_w,board_h);          /* 标定板上每行、列的角点数 */
    Mat intrinsic;
    Mat distortion;
    
    FileStorage fs("Intrinsics.xml", FileStorage::READ);
    read(fs["intrisics_matrix"], intrinsic);
    cout << "intrisics_matrix : \n" << intrinsic << endl;
    FileStorage fd("Distortion.xml", FileStorage::READ);
    read(fd["distortion_matrix"], distortion);
    cout << "distortion_matrix: \n" << distortion << endl;
    
    vector<Point2f> image_points_buf;   /* 缓存每幅图像上检测到的角点 */
    vector<vector<Point2f>> image_points_seq; /* 保存检测到的所有角点 */
    //    string filename = "Image1/Image15.jpg";
    
    //    Mat image = imread(filename);
    image_size.width = image1.cols;
    image_size.height = image1.rows;
    
    Mat mapx = Mat(image_size,CV_32FC1);
    Mat mapy = Mat(image_size,CV_32FC1);
    Mat R = Mat::eye(3,3,CV_32F);
    
    initUndistortRectifyMap(intrinsic,distortion,R,intrinsic,image_size,CV_32FC1,mapx,mapy);
    Mat newimage = image1.clone();
    
    //另一种不需要转换矩阵的方式
    remap(newimage,image1,mapx,mapy, INTER_LINEAR);
    
    newimage = image2.clone();
    
    remap(newimage, image2, mapx, mapy, INTER_LINEAR);
    
    if(show){
        imshow("chessboard",image1);//显示图片
        waitKey(500);//暂停0.5S
        imshow("chessboard image2", image2);
        waitKey(500);
    }
    
    imwrite("output_img1-1.jpg", image1);
    imwrite("output_img2-1.jpg", image2);
    
    /* 提取角点 */
    if(0 == findChessboardCorners(image1,board_size,image_points_buf)){
        cout<<"cannot find chessboard corners of first image!\n"; //找不到角点
        image_points_seq.push_back(image_points_buf);
        return image1;
    }else if(0 == findChessboardCorners(image2, board_size, image_points_buf)){
        cout << "cannot find chessboard corners of second image! \n";
        image_points_seq.push_back(image_points_buf);
        return image2;
    }
    Mat view_gray;
    cvtColor(image1,view_gray,CV_RGB2GRAY);
    /* 亚像素精确化 */
    //    find4QuadCornerSubpix(view_gray,image_points_buf,Size(5,5));//对粗提取的角点进行精确化
    cornerSubPix(view_gray,image_points_buf,Size(5,5),Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,0.1));
    image_points_seq.push_back(image_points_buf);    //保存亚像素角点
    
    cvtColor(image2,view_gray,CV_RGB2GRAY);
    /* 亚像素精确化 */
    //    find4QuadCornerSubpix(view_gray,image_points_buf,Size(5,5));//对粗提取的角点进行精确化
    cornerSubPix(view_gray,image_points_buf,Size(5,5),Size(-1,-1),TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,0.1));
    image_points_seq.push_back(image_points_buf);    //保存亚像素角点
    
    
    Point2f object_points[4], image_points[4];
//    object_points[0].x = 0; object_points[0].y = 0;
//    object_points[1].x = board_w - 1; object_points[1].y = 0;
//    object_points[2].x = 0; object_points[2].y = board_h - 1;
//    object_points[3].x = board_w - 1; object_points[3].y = board_h - 1;
    image_points[0] = image_points_seq[0][0];
    image_points[1] = image_points_seq[0][board_w - 1];
    image_points[2] = image_points_seq[0][(board_h - 1) * board_w];
    image_points[3] = image_points_seq[0][board_h * board_w - 1];
    object_points[0] = image_points_seq[1][0];
    object_points[1] = image_points_seq[1][board_w - 1];
    object_points[2] = image_points_seq[1][(board_h - 1) * board_w];
    object_points[3] = image_points_seq[1][board_h * board_w - 1];
    
    /* 显示四角，颜色顺序为：蓝，绿，红，黄*/
    circle(image1, image_points[0], 9, CV_RGB(0, 0, 255));
    circle(image1, image_points[1], 9, CV_RGB(0, 255, 0));
    circle(image1, image_points[2], 9, CV_RGB(255, 0, 0));
    circle(image1, image_points[3], 9, CV_RGB(255, 255, 0));
    
    /* 在图像上显示角点位置 */
    drawChessboardCorners(image1,board_size,image_points_buf,false);//用于在图片中标记角点
    if(show){
        imshow("chessboard",image1);//显示图片
        waitKey(500);//暂停0.5S
        imshow("chessboard image2", image2);
        waitKey(500);
    }
    
    /* 求解单应矩阵*/
    Mat H = Mat(3, 3, CV_32FC1, Scalar::all(0));
    H = getPerspectiveTransform(object_points, image_points);
    cout << "H: \n" << H << endl;
    
    float z = 30;
    int key = 0;
    Mat bird_image = image1.clone();
    
    std::stringstream StrStm;
    
    //    while(key != 27){
    //        CvMat cvH = H;
    //        CV_MAT_ELEM(cvH, float, 2, 2) = z;
    //        H = Mat(cvH.rows, cvH.cols, cvH.type, cvH.data.fl);
    H.at<double>(2, 2) = z;
    warpPerspective(image1,
                    bird_image,
                    H,
                    image_size,
                    CV_INTER_LINEAR + CV_WARP_INVERSE_MAP);
    if(show){
        imshow("Bird's Eye", bird_image);
        key = waitKey();
    }
    
    if(key == 'u') z += 0.5;
    if(key == 'd') z -= 0.5;
    
    //        string imageFileName = "";
    //        StrStm<<order;
    //        StrStm>>imageFileName;
    //
    //        imageFileName = "Image1/Image" + imageFileName + "_r.jpg";
    //        imwrite(imageFileName, bird_image);
    //    }
    
    FileStorage fh("H.xml", FileStorage::WRITE);
    fh << "H" << H;
    fh.release();
    std::cout<<"保存结束"<<endl;
    return bird_image;
}

int main(){
//int birdEye(){
    string filename = "cv_res/Image21.jpg";
    clock_t start = clock();
//    for(int order = 1; order < 22; order++){
////        filename = "Image1/Image";
//        filename = "";
//        std::stringstream StrStm;
//        StrStm<<order;
//        StrStm>>filename;
//        filename = "Image1/Image" + filename + ".jpg";
//        initH(filename, order, false);
//    }
//    VideoCapture cap;
//    string videoName = "test/road2.mp4";
//    cap.open(videoName);
//    if(!cap.isOpened()){
//        cout << " fail to open the video: " << videoName << endl;
//    }
//    Size videoSize = Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
//    VideoWriter videoWriter("test/road1_res.mp4", -1, cap.get(CV_CAP_PROP_FPS), videoSize, false);
//    Mat frame;
//    Mat bird_frame;
//    while (true) {
//        cap>>frame;
//        if(frame.empty()){
//            break;
//        }
////        imwrite("input_frame0.jpg", frame);
//        imshow("input frame", frame);
//        waitKey(500);
//        bird_frame = initH(frame);
//        videoWriter<<bird_frame;
//    }
//    cap.release();
//    Mat image = imread("cv_res/image21.jpg");
//    Mat res = initH(image);
//    imwrite("image21_res.jpg", res);
//    temp();
    Mat image1 = imread("/Users/luomei/Documents/MATLAB/img1.jpg");
    Mat image2 = imread("/Users/luomei/Documents/MATLAB/img2.jpg");
    initH(image1,image2);
    clock_t end = clock();
    cout<< " it takes "<< (end - start)/(20 * CLK_TCK) << " ms "<< endl;
//    initH(filename, order);
    return 0;
}
