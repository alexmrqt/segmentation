#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include "include/segmentation.h"

using namespace cv;
using namespace std;

string int_to_str(int a)
{
    ostringstream temp;
    temp<<a;
    return temp.str();
}

int main(int argc, char* argv[])
{
    //Parameters
    unsigned int dist=10;
    size_t minsize=100;
    Mat src_img;
    vector<Mat> segments_img;

    if( argc != 2)
    {
        cout <<" Usage: " << argv[0] << " path_to_image.bmp" << endl;
        return -1;
    }

    //Load img
    cout << "Loading image" << endl;
    src_img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    if(src_img.data == NULL) {
        cout << "Cannot read image." << endl;
        return -1;
    }
    if((src_img.rows == -1) || (src_img.cols == -1)) {
        cout << "Not a 2D image." << endl;
        return -1;
    }
    if(src_img.depth() != CV_8U)  {
        cout << "Not implemented" << endl;
        return -1;
    }

    //Instanciate the right segment object
    //depending on the type of the image
    segmentation<uchar> seg = segmentation<uchar>(src_img);

    //Find segments
    cout << "Finding segments...";
    seg.find_all_segments(dist);
    cout << "End." << endl;

    //Return segments
    cout << "Post processing segments...";
    seg.convert_segments_to_img(minsize);
    segments_img = seg.get_segments_img();
    cout << "End." << endl;

    //Display found segments
    cout << "Found " << segments_img.size() << " segments!" << endl;
    seg.show_img();
    for (size_t i = 0; i < segments_img.size(); ++i){
        imshow("Segment " + int_to_str(i+1), segments_img[i]);
    }

    waitKey(0);
    return 0;
}
