/*
 * Copyright (C) 2017 Alexandre Marquet.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "segmentation.h"

using namespace std;
using namespace cv;

template<typename T>
segmentation<T>::segmentation(Mat img): m_img(img)
{
    //Initializations
    m_img_ptr=0;
    m_already_seen_pxs.resize(m_img.rows*m_img.cols, 0);
}

template<typename T>
vector<int> segmentation<T>::lin_to_xy(size_t ptr)
{
    vector<int> out;

    out.reserve(2);


    out.push_back(ptr/m_img.cols);  //nb_columns
    out.push_back(ptr%m_img.cols);  //nb_lines

    return out;
}

template<typename T>
size_t segmentation<T>::xy_to_lin(int x, int y)
{
    return y + x*m_img.cols;
}

template<typename T>
vector<size_t> segmentation<T>::find_neighbors(size_t ptr)
{
    vector<size_t> neighbors;
    vector<int> ptr_xy = lin_to_xy(ptr);
    size_t ptr_0=0;

    //Max number of neighbor is 8, including self leads to 9
    neighbors.reserve(9);

    for(int x_0=ptr_xy[0]-1 ; x_0 <= ptr_xy[0]+1 ; ++x_0) {
        for(int y_0=ptr_xy[1]-1 ; y_0 <= ptr_xy[1]+1 ; ++y_0) {
            //Neighbor pixel outside image size
            if ((x_0 >= m_img.rows) or (x_0 < 0) or (y_0 >= m_img.cols) or (y_0 < 0)){
                continue;
            }

            ptr_0 = xy_to_lin(x_0, y_0);

            //Neighbor pixel has already been examined
            if ((ptr_0 <= m_img_ptr) || m_already_seen_pxs[ptr_0]) {
                continue;
            }

            //Pixel is a non-examined neighbor
            neighbors.push_back(ptr_0);
        }
    }

    return neighbors;
}

template<typename T>
vector<size_t> segmentation<T>::find_segment(unsigned int dist)
{
    //List of pixels of the segment
    vector<size_t> seg;
    //List ofneighbors for a particular pixel
    vector<size_t> neighbors;
    //List of pixels (and related score) to be examined
    vector<size_t> already_seen_px;
    vector<unsigned int> already_seen_score;

    size_t ptr_0 = 0;
    vector<int> neighbor_xy;
    unsigned int score_0 = 0;

    already_seen_px.push_back(m_img_ptr);
    already_seen_score.push_back(dist);
    seg.push_back(m_img_ptr);

    //While there exist pixels to examine
    while (already_seen_px.size() > 0){
        //Pick a pixel (and score) to be examined
        ptr_0 = already_seen_px.back();
        already_seen_px.pop_back();
        score_0 = already_seen_score.back();
        already_seen_score.pop_back();

        //Find its neighbors
        neighbors = find_neighbors(ptr_0);
        for (size_t i=0 ; i < neighbors.size(); ++i){
            m_already_seen_pxs[neighbors[i]]=1;

            neighbor_xy = lin_to_xy(neighbors[i]);

            //if pixel is non-zero
            if (m_img.at<T>(neighbor_xy[0], neighbor_xy[1]) != 0){
                already_seen_px.push_back(neighbors[i]);
                already_seen_score.push_back(dist);
                seg.push_back(neighbors[i]);
            }
            else{
                if((score_0 - 1) > 0){
                    already_seen_px.push_back(neighbors[i]);
                    already_seen_score.push_back(score_0 - 1);
                }
            }
        }
    }

    return seg;
}

template<typename T>
void segmentation<T>::find_all_segments(unsigned int dist)
{
    vector<int> px_xy;

    for(m_img_ptr = 0; m_img_ptr < (size_t)m_img.cols*(size_t)m_img.rows; ++m_img_ptr){
        px_xy = lin_to_xy(m_img_ptr);

        if(m_img.at<uchar>(px_xy[0], px_xy[1]) != 0) {
            m_segments.push_back(find_segment(dist));
        }
    }
}


template<typename T>
void segmentation<T>::convert_segments_to_img(size_t minsize)
{
    vector<size_t> segment;
    Mat img_segment;
    vector<int> px_xy;

    for (size_t i = 0; i < m_segments.size(); ++i){
        segment = m_segments[i];
        if (segment.size() < minsize){
            continue;
        }

        Mat img_segment = Mat::zeros(m_img.rows, m_img.cols, m_img.depth());

        for (size_t j = 0; j < segment.size() ; ++j ){
            px_xy = lin_to_xy(segment[j]);
            //cout << px_xy[0] << " " << px_xy[1] << " " << (int)m_img.at<uchar>(px_xy[0], px_xy[1]) << endl;
            img_segment.at<T>(px_xy[0], px_xy[1]) = m_img.at<T>(px_xy[0], px_xy[1]);
        }

        m_img_segments.push_back(img_segment);
    }
}

template<typename T>
vector<Mat> segmentation<T>::get_segments_img()
{
    return m_img_segments;
}

template<typename T>
void segmentation<T>::show_img()
{
    //namedWindow( "Original image", WINDOW_AUTOSIZE );
    imshow( "Original image", m_img);
}

template class segmentation<uchar>; //For CV_8U
template class segmentation<char>;  //For CV_8S
template class segmentation<unsigned int>;  //For CV_16U
template class segmentation<int>;  //For CV_16S
template class segmentation<long>;  //For CV_32S
template class segmentation<float>;  //For CV_32F
template class segmentation<double>;  //For CV_64F
