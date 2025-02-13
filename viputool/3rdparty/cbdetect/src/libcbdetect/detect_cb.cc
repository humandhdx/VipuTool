#include "libcbdetect/detect_cb.h"

double triangleWithMask(const cv::Mat src, const cv::Mat& mask)
{
    const int N = 256;
    int i, j, h[N] = {0};

    for (i = 0; i < src.rows; i++)
    {
        const uchar* psrc = src.ptr(i);
        const uchar* pmask = mask.ptr(i);
        
        for (j = 0; j < src.cols; j++){

            if (pmask[j])
            {
                h[psrc[j]]++;
            }
        }
    }

    int left_bound = 0, right_bound = 0, max_ind = 0, max = 0;
    int temp;
    bool isflipped = false;


    for( i = 0; i < N; i++ )
    {
        if( h[i] > 0 )
        {
            left_bound = i;
            break;
        }
    }
    if( left_bound > 0 )
        left_bound--;

    for( i = N-1; i > 0; i-- )
    {
        if( h[i] > 0 )
        {
            right_bound = i;
            break;
        }
    }
    if( right_bound < N-1 )
        right_bound++;

    for( i = 0; i < N; i++ )
    {
        if( h[i] > max)
        {
            max = h[i];
            max_ind = i;
        }
    }

    if( max_ind-left_bound < right_bound-max_ind)
    {
        isflipped = true;
        i = 0, j = N-1;
        while( i < j )
        {
            temp = h[i]; h[i] = h[j]; h[j] = temp;
            i++; j--;
        }
        left_bound = N-1-right_bound;
        max_ind = N-1-max_ind;
    }

    double thresh = left_bound;
    double a, b, dist = 0, tempdist;

    a = max; b = left_bound-max_ind;
    for( i = left_bound+1; i <= max_ind; i++ )
    {
        tempdist = a*i + b*h[i];
        if( tempdist > dist)
        {
            dist = tempdist;
            thresh = i;
        }
    }
    thresh--;

    if( isflipped )
        thresh = N-1-thresh;

    return thresh;
}


bool isWhiteBlock(const cv::Mat &binary, std::vector<cv::Point> &boundary_points){
    cv::Mat mask = cv::Mat::zeros(binary.size(), CV_8UC1);
    cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{boundary_points}, 255);

    int black_num = 0;
    int white_num = 0;
    cv::Rect rect = cv::boundingRect(boundary_points);
    for(int i = rect.tl().x ; i < rect.br().x ; i++){
        for(int j = rect.tl().y ; j < rect.br().y ; j++){
            if(mask.at<uchar>(j,i)){
                if(binary.at<uchar>(j,i)){
                    white_num++;
                }
                else{
                    black_num++;
                }
            }
        }
    }

    if(white_num > black_num){
        return true;
    }

    return false;
}

bool isClockwise(const std::vector<cv::Point> &points) {
    double area = 0.0;
    int n = points.size();
    
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n; // Next vertex index
        area += points[i].x * points[j].y;
        area -= points[j].x * points[i].y;
    }
    
    return area < 0; // Clockwise if area is negative
}


void reorder(const cv::Mat &image, std::vector<cv::Point2f> &points, const cv::Size &pattern_size){
    cv::Mat gray_image;
    cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);

    std::vector<std::vector<cv::Point2f>> points_container(pattern_size.height, std::vector<cv::Point2f>(pattern_size.width));
    for(int i = 0 ; i < pattern_size.height * pattern_size.width ; i++){
        int col = i % pattern_size.width;
        int row = i / pattern_size.width;
        points_container[row][col] = points[i];
    }

    cv::Point2f tl = points_container[0][0];
    cv::Point2f tr = points_container[0][pattern_size.width-1];
    cv::Point2f bl = points_container[pattern_size.height-1][0];
    cv::Point2f br = points_container[pattern_size.height-1][pattern_size.width-1];

    std::vector<cv::Point> boundary_points{tl,tr,bl,br};
    cv::Mat mask = cv::Mat::zeros(image.size(), CV_8UC1);
    cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{boundary_points}, 255);
    double thresh = triangleWithMask(gray_image, mask);
    cv::Mat binary;
    cv::threshold(gray_image, binary, thresh, 255, cv::THRESH_BINARY);

    std::vector<cv::Point> tl_region{tl, points_container[0][1], points_container[1][1], points_container[1][0]};
    std::vector<cv::Point> tr_region{tr, points_container[1][pattern_size.width-1], points_container[1][pattern_size.width-2], points_container[0][pattern_size.width-2]};
    std::vector<cv::Point> bl_region{bl, points_container[pattern_size.height-2][0], points_container[pattern_size.height-2][1], points_container[pattern_size.height-1][1]};
    std::vector<cv::Point> br_region{br, points_container[pattern_size.height-1][pattern_size.width-2], points_container[pattern_size.height-2][pattern_size.width-2], points_container[pattern_size.height-2][pattern_size.width-1]};

    bool tl_white = isWhiteBlock(binary, tl_region);
    bool tr_white = isWhiteBlock(binary, tr_region);
    bool br_white = isWhiteBlock(binary, br_region);
    bool bl_white = isWhiteBlock(binary, bl_region);
    
    // row reverse
    if(!tl_white){
        for(auto &row_vec:points_container){
            std::reverse(row_vec.begin(), row_vec.end());
        }
    }

    // col reverse
    tl_region = std::vector<cv::Point>{points_container[0][0], points_container[0][1], points_container[1][1], points_container[1][0]};
    if(isClockwise(tl_region)){
        std::reverse(points_container.begin(), points_container.end());
    }

    points.clear();
    for(const auto &row_vec:points_container){
        for(const auto &pt:row_vec){
            points.push_back(pt);
        }
    }
}

bool detectCB(const cv::Mat &img, std::vector<cv::Point2f> &points, const cv::Size &pattern_size)
{
    cbdetect::Corner corners;
    std::vector<cbdetect::Board> boards;
    cbdetect::Params params;
    params.corner_type = cbdetect::SaddlePoint;

    cbdetect::find_corners(img, corners, params);
    if(corners.p.size() == 0){
        return false;
    }
    cbdetect::boards_from_corners(img, corners, boards, params);
    // cbdetect::plot_boards(img, corners, boards, params);

    std::vector<std::vector<cv::Point2f>> all_points;
    for(int j = 0 ; j < boards.size() ; j++){
        std::vector<cv::Point2f> cur_points;
        for (int i = 1; i < boards[j].idx.size() * boards[j].idx[0].size(); ++i){
            int row = i / boards[j].idx[0].size();
            int col = i % boards[j].idx[0].size();

            int index = boards[j].idx[row][col];
            // reorder, ensure long side is x
            if(boards[j].idx.size() > boards[j].idx[0].size()){
                int col = i / boards[j].idx.size();
                int row = i % boards[j].idx.size();
                index = boards[j].idx[row][col];
            }

            if (index < 0){
                continue;
            }

            cur_points.push_back(corners.p[index]);
        }
        all_points.push_back(cur_points);
    }

    std::vector<int> valid_index;

    for(int i = 0 ; i < all_points.size() ; i++){
        if(all_points[i].size() == pattern_size.height * pattern_size.width){
            valid_index.push_back(i);
        }
    }

    if (valid_index.size() != 1 || all_points[valid_index[0]].size() != pattern_size.height * pattern_size.width){
        return false;
    }

    points = all_points[valid_index[0]];

    reorder(img, points, pattern_size);

    return true;
}