//
//  CameraCalibrator.cpp
//  birdEye
//
//  Created by 罗媚 on 2019/5/20.
//  Copyright © 2019 罗媚. All rights reserved.
//
/*------------------------------------------------------------------------------------------*\
 This file contains material supporting chapter 11 of the book:
 OpenCV3 Computer Vision Application Programming Cookbook
 Third Edition
 by Robert Laganiere, Packt Publishing, 2016.
 
 This program is free software; permission is hereby granted to use, copy, modify,
 and distribute this source code, or portions thereof, for any purpose, without fee,
 subject to the restriction that the copyright notice may not be removed
 or altered from any source or altered source distribution.
 The software is released on an as-is basis and without any warranties of any kind.
 In particular, the software is not guaranteed to be fault-tolerant or free from failure.
 The author disclaims all warranties with regard to this software, any use,
 and any consequent failure, is purely the responsibility of the user.
 
 Copyright (C) 2016 Robert Laganiere, www.laganiere.name
 \*------------------------------------------------------------------------------------------*/


#include "CameraCalibrator.h"
const float CHESS_LEN = 0.025;

// Open chessboard images and extract corner points
int CameraCalibrator::addChessboardPoints(
                                          const std::vector<std::string>& filelist, // list of filenames containing board images
                                          cv::Size & boardSize,                     // size of the board
                                          std::string windowName) {                 // name of window to display results
    // if null, no display shown
    // the points on the chessboard
    std::vector<cv::Point2f> imageCorners;
    std::vector<cv::Point3f> objectCorners;
    
    // 3D Scene Points:
    // Initialize the chessboard corners
    // in the chessboard reference frame
    // The corners are at 3D location (X,Y,Z)= (i,j,0)
    for (int i=0; i<boardSize.height; i++) {
        for (int j=0; j<boardSize.width; j++) {
            
            objectCorners.push_back(cv::Point3f(i * CHESS_LEN, j * CHESS_LEN, 0.0f));
        }
    }
    
    // 2D Image points:
    cv::Mat image; // to contain chessboard image
    int successes = 0;
    // for all viewpoints
    for (int i=0; i<filelist.size(); i++) {
        
        // Open the image
        image = cv::imread(filelist[i],0);
        
        // Get the chessboard corners
        bool found = cv::findChessboardCorners(image,         // image of chessboard pattern
                                               boardSize,     // size of pattern
                                               imageCorners); // list of detected corners
        
        // Get subpixel accuracy on the corners
        if (found) {
            cv::cornerSubPix(image, imageCorners,
                             cv::Size(5, 5), // half size of serach window
                             cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::MAX_ITER +
                                              cv::TermCriteria::EPS,
                                              30,     // max number of iterations
                                              0.1));  // min accuracy
            
            // If we have a good board, add it to our data
            if (imageCorners.size() == boardSize.area()) {
                
                // Add image and scene points from one view
                addPoints(imageCorners, objectCorners);
                successes++;
            }
        }
        
        if (windowName.length()>0 && imageCorners.size() == boardSize.area()) {
            
            //Draw the corners
            cv::drawChessboardCorners(image, boardSize, imageCorners, found);
            cv::imshow(windowName, image);
            cv::waitKey(500);
        }
    }
    
    return successes;
}

// Add scene points and corresponding image points
void CameraCalibrator::addPoints(const std::vector<cv::Point2f>& imageCorners, const std::vector<cv::Point3f>& objectCorners) {
    
    // 2D image points from one view
    imagePoints.push_back(imageCorners);
    // corresponding 3D scene points
    objectPoints.push_back(objectCorners);
}

// Calibrate the camera
// returns the re-projection error
double CameraCalibrator::calibrate(const cv::Size imageSize)
{
    // undistorter must be reinitialized
    mustInitUndistort= true;
    
    //Output rotations and translations
    std::vector<cv::Mat> rvecs, tvecs;
    
    // start calibration
    return
    cv::calibrateCamera(objectPoints, // the 3D points
                        imagePoints,  // the image points
                        imageSize,    // image size
                        cameraMatrix, // output camera matrix
                        distCoeffs,   // output distortion matrix
                        rvecs, tvecs, // Rs, Ts
                        CV_CALIB_USE_INTRINSIC_GUESS  );        // set options
    //                  ,CV_CALIB_USE_INTRINSIC_GUESS);
    
    
    
}

// remove distortion in an image (after calibration)
cv::Mat CameraCalibrator::remap(const cv::Mat &image, cv::Size &outputSize) {
    
    cv::Mat undistorted;
    
    if (outputSize.height == -1)
        outputSize = image.size();
    
    if (mustInitUndistort) { // called once per calibration
        
        
        // cv::Mat dist = (cv::Mat_<double>(1, 14) << -107.8067046233813, 3606.394522865697, -0.003208480233032964, 0.00212257161649465, -706.3301131023582, -107.3590793091425, 3559.830030448713, 855.5629043718579, 0, 0, 0, 0, 0, 0);
        // dist = distCoeffs;
        
        cv::initUndistortRectifyMap(
                                    cameraMatrix,  // computed camera matrix
                                    distCoeffs,    // computed distortion matrix
                                    cv::Mat(),     // optional rectification (none)
                                    cv::Mat(),     // camera matrix to generate undistorted
                                    outputSize,    // size of undistorted
                                    CV_32FC1,      // type of output map
                                    map1, map2);   // the x and y mapping functions
        
        mustInitUndistort= false;
    }
    
    // Apply mapping functions
    cv::remap(image, undistorted, map1, map2,
              cv::INTER_LINEAR); // interpolation type
    
    return undistorted;
}


// Set the calibration options
// 8radialCoeffEnabled should be true if 8 radial coefficients are required (5 is default)
// tangentialParamEnabled should be true if tangeantial distortion is present
void CameraCalibrator::setCalibrationFlag(bool radial8CoeffEnabled, bool tangentialParamEnabled) {
    
    // Set the flag used in cv::calibrateCamera()
    flag = 0;
    if (!tangentialParamEnabled) flag += CV_CALIB_ZERO_TANGENT_DIST;
    if (radial8CoeffEnabled) flag += CV_CALIB_RATIONAL_MODEL;
}
