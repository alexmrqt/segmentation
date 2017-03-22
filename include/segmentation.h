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

#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

template<typename T>
class segmentation
{
    private:
        Mat m_img;
        size_t m_img_ptr;
        vector<bool> m_already_seen_pxs;
        vector<vector<size_t> > m_segments;
        vector<Mat> m_img_segments;

        vector<int> lin_to_xy(size_t ptr);
        size_t xy_to_lin(int x, int y);
        vector<size_t> find_neighbors(size_t ptr);
        vector<size_t> find_segment(unsigned int dist);

    public:
        segmentation(Mat img);
        void show_img();
        void find_all_segments(unsigned int dist);
        void convert_segments_to_img(size_t minsize);
        vector<Mat> get_segments_img();
};

#endif // SEGMENTATION_H
