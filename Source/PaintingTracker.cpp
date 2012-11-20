/**
 * This file is part of PaintingRegistration.
 *
 * Created by Jonathan Hook (jonathan.hook@ncl.ac.uk)
 * Copyright (c) 2012 Jonathan Hook. All rights reserved.
 *
 * PaintingRegistration is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PaintingRegistration is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PaintingRegistration.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "JDHUtility/CrossPlatformTime.h"
#include "JDHUtility/FileLocationUtility.h"
#include "JDHUtility/Ndelete.h"
#include "JDHUtility/OpenGL.h"
#include "JDHUtility/Point2f.h"
#include "PaintingTracker.h"

using namespace JDHUtility;

namespace PaintingRegistration
{
    /* Public */
    PaintingTracker::PaintingTracker(const std::string image)
    {
        cap = new cv::VideoCapture(0);
        hasTarget = false;
        matcher = new cv::BFMatcher(cv::NORM_L2, false);
        vertices = new Point2f[VERTEX_COUNT];
        textureHandle = NULL;

        detector = new cv::SiftFeatureDetector();
        extractor = new cv::SiftDescriptorExtractor();

        targetCorners.resize(4);
        initTextureHandle();
        
        if(image != "")
        {
            train(image);
        }
    }
    
    PaintingTracker::~PaintingTracker(void)
    {
        NDELETE(cap);
        NDELETE(detector);
        NDELETE(extractor);
        NDELETE(matcher);
        NDELETE_ARRAY(vertices);
    }
    
    void PaintingTracker::capture(void)
    {
        cap->read(cameraImage);
    }
    
    bool PaintingTracker::compute(void)
    {
        cv::cvtColor(cameraImage, greyImage, CV_RGB2GRAY);
        
        detector->detect(greyImage, cameraKeyPoints);
        extractor->compute(greyImage, cameraKeyPoints, cameraDescriptors);
        matcher->match(targetDescriptors, cameraDescriptors, matches);

        maxDist = 0;
        minDist = 100;
        for(int i = 0; i < targetDescriptors.rows; i++)
        {
            double dist = matches[i].distance;
            if(dist < minDist)
            {
                minDist = dist;
            }
            
            if(dist > maxDist)
            {
                maxDist = dist;
            }
        }
        
        for(int i = 0; i < targetDescriptors.rows; i++)
        {
            if(matches[i].distance < 3 * minDist)
            {
                goodMatches.push_back(matches[i]);
            }
        }
    
        for(int i = 0; i < goodMatches.size(); i++)
        {
            t.push_back(targetKeyPoints[goodMatches[i].queryIdx].pt);
            f.push_back(cameraKeyPoints[goodMatches[i].trainIdx].pt);
        }
        
        if(t.size() >= 25 && f.size() >= 25)
        {
            homography = cv::findHomography(t, f, CV_RANSAC);
            hasTarget = true;

            targetCorners[0] = cvPoint(0,0);
            targetCorners[1] = cvPoint(targetImage.cols, 0);
            targetCorners[2] = cvPoint(targetImage.cols, targetImage.rows);
            targetCorners[3] = cvPoint(0, targetImage.rows);
            
            cv::perspectiveTransform(targetCorners, cameraCorners, homography);
            
            vertices[0].setPosition(cameraCorners[0].x / (float)cameraImage.cols, cameraCorners[0].y / (float)cameraImage.rows);
            vertices[1].setPosition(cameraCorners[1].x / (float)cameraImage.cols, cameraCorners[1].y / (float)cameraImage.rows);
            vertices[2].setPosition(cameraCorners[2].x / (float)cameraImage.cols, cameraCorners[2].y / (float)cameraImage.rows);
            vertices[3].setPosition(cameraCorners[3].x / (float)cameraImage.cols, cameraCorners[3].y / (float)cameraImage.rows);
            
#if defined(DEBUG) && defined(GLUT_WINDOWING)
            cv::line(cameraImage, cameraCorners[0], cameraCorners[1], cv::Scalar(0, 255, 0), 4);
            cv::line(cameraImage, cameraCorners[1], cameraCorners[2], cv::Scalar(0, 255, 0), 4);
            cv::line(cameraImage, cameraCorners[2], cameraCorners[3], cv::Scalar(0, 255, 0), 4);
            cv::line(cameraImage, cameraCorners[3], cameraCorners[0], cv::Scalar(0, 255, 0), 4);
#endif
        }
        else
        {
            hasTarget = false;
        }
        
#if defined(DEBUG) && defined(GLUT_WINDOWING)
        cv::imshow( "Good Matches & Object detection", cameraImage);  
#endif
        
        cameraKeyPoints.clear();
        f.clear();
        goodMatches.clear();
        matches.clear();
        t.clear();
        
        return hasTarget;
    }
    
    bool PaintingTracker::getHasVertices(void) const
    {
        return hasTarget;
    }
    
    GLuint PaintingTracker::getTextureHandle(void) const
    {
        updateTexture();
        return textureHandle;
    }
    
    const Point2f *PaintingTracker::getVertices(void) const
    {
        return vertices;
    }
    
    void PaintingTracker::train(const std::string image)
    {
        std::string localPath = FileLocationUtility::getFileInResourcePath(image);
        targetImage = cv::imread(localPath, CV_LOAD_IMAGE_GRAYSCALE);
        
        detector->detect(targetImage, targetKeyPoints);
        extractor->compute(targetImage, targetKeyPoints, targetDescriptors);
    }
    
    /* Private */
    void PaintingTracker::initTextureHandle(void)
    {
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &textureHandle);
        glDisable(GL_TEXTURE_2D);
    }
    
    void PaintingTracker::updateTexture(void) const
    {
        cv::Mat img;
        cv::cvtColor(cameraImage, img, CV_RGB2BGR);
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data);
        glDisable(GL_TEXTURE_2D);
    }
}