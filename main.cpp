// Author: Dev Suri
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <math.h>
#include "opencv2/opencv.hpp"


using namespace std;
using namespace cv;

#define PI 3.14159265

//three dimensional point
struct TDP {
    int x;
    int y;
    int z;
} ;

//returns rotated point given angle of rotation
//rotates along z-axis
TDP rotate_z(TDP p, int angle){
    double rad = angle * PI / 180.0;
    int newx = (cos(rad)*p.x) + (-sin(rad)*p.y) + (0*p.z);
    int newy = (sin(rad)*p.x) + (cos(rad)*p.y) + (0*p.z);
    int newz = p.z;
    TDP toreturn = {newx, newy, newz};
    return toreturn;
}
//helper method to draw line
void DrawLine(Point p1, Point p2, const Mat& display, Scalar k){
    int lineThickness = 8;
    line(display, p1, p2, k, lineThickness);
}
//method where I run everything
int Draw(const Mat& src_display)
{

    using namespace std;
    using namespace cv;


    vector<TDP> rotatedpoints;
    VideoCapture cap("withChessBoard.mov");
    if(!cap.isOpened()){
        cout << "Error opening video stream or file" << endl;
    }
    int frames = 0;
    Mat frame;
    cap.set(CAP_PROP_POS_MSEC, 300);
    double fps = cap.get(CAP_PROP_FPS);
    vector<Point> points;
    cap >> frame;
    VideoWriter record("result.avi", cap.get(CAP_PROP_FOURCC), 60, frame.size(), true);
    if( !record.isOpened() ) {
        printf("VideoWriter failed to open!\n");
    }
    cout << "Input Video FPS is : " << fps << endl;

    for ( ; ; ) {
        //
        points.clear();
        //
        cap >> frame; // read a new frame from video
        // flip the frame, since converting from MOV TO AVI results in upside down image
        Mat new_frame;
        flip(frame, new_frame, -1);
        frame = new_frame.clone(); // save the flipped frame
        //continue
        if(frame.empty()){
            cout << "End of Video" << endl;
            break;
        }
        Size patternsize(7,7); //board size
        vector<Point2f> centers;

        bool patternfound = findChessboardCorners(frame,patternsize,centers);

        if(!patternfound){
            cout << "skipping some frames since chessboard is obstructed by object" << endl;
            for(int k=0;k<20; k++){
                cap >> frame; //skip twenty frames
            }
            continue;
        }

        //
        DrawLine(centers[30], centers[32], frame, Scalar(0,0,255));
        DrawLine(centers[18], centers[16], frame, Scalar(0,0,255));
        DrawLine(centers[16], centers[30], frame, Scalar(0,0,255));
        DrawLine(centers[18], centers[32], frame, Scalar(0,0,255));
        //
        circle(frame, centers[30], 10, Scalar(0, 255, 0), 4, 8, 0);
        circle(frame, centers[18], 10, Scalar(0, 255, 0), 4, 8, 0);
        circle(frame, centers[32], 10, Scalar(0, 255, 0), 4, 8, 0);
        circle(frame, centers[16], 10, Scalar(0, 255, 0), 4, 8, 0);
        //
        points.push_back(centers[30]);
        points.push_back(centers[32]);
        points.push_back(centers[18]);
        points.push_back(centers[16]);
        //
        Point interest = centers[centers.size()/2];
        TDP k = {interest.x,interest.y, 140};
        int factor = 1500;
        double t = double(100-factor) / double(k.x - factor);
        int u = int(t * k.y);
        int v = int(t * k.z);
        points.push_back(Point(interest.x, interest.y-100));
        //

        DrawLine(points[0], points[1], frame, Scalar(255, 0, 0));
        DrawLine(points[2], points[3], frame, Scalar(255, 0, 0));
        DrawLine(points[0], points[3], frame, Scalar(255, 0, 0));
        DrawLine(points[1], points[2], frame, Scalar(255, 0, 0));
        DrawLine(points[0], points[4], frame, Scalar(255, 0, 0));
        DrawLine(points[1], points[4], frame, Scalar(255, 0, 0));
        DrawLine(points[2], points[4], frame, Scalar(255, 0, 0));
        DrawLine(points[3], points[4], frame, Scalar(255, 0, 0));

        drawChessboardCorners(frame, patternsize, Mat(centers), patternfound);
        Mat display = frame.clone();
        record << display;

        //frame stats
        frames += 1;
        if (frames % 20 == 0) {
            cout << "Percent of Video Processed: " << frames * 100 / cap.get(CAP_PROP_FRAME_COUNT) << "%" << endl;
        }
    }
    record.release();
    cap.release();
    return 1;
}

int main(int argc, char** argv)
{
    Mat src(1000, 1000, CV_8UC3, Scalar(0, 0, 0));
    if(Draw(src)==1){
        cout << "Successfully created video named result.avi" << endl;
    }
    return 0;
}
