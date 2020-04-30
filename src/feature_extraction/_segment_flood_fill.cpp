#ifndef CMSC630_SEGMENT_FLOOD_FILL_H
#define CMSC630_SEGMENT_FLOOD_FILL_H

#include <queue>

struct coord {
    int x;
    int y;
} ;

struct segment_statistics {
    int number_of_segments;
    double average_segment_size;
    int max_segment_size;
    int min_segment_size;
};

/*
 * Because we are calculating all segment statistics in one function, it would be highly useful to just return all results at once
 * OR cache the results so that the logic doesn't need to run so many times per image.
 */
segment_statistics _segment_flood_fill(Mat *src) {
    Mat mem = Mat::zeros(src->rows, src->cols, CV_8U);

    int num_segments = 0;
    int segment_size_sum = 0;
    int max_segment_size = -1;
    int min_segment_size = 999999999;

    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            bool is_segment = src->at<uint8_t>(i, j) == 0;
            bool already_evaluated = mem.at<uint8_t>(i, j) != 0;
            if (is_segment and !already_evaluated) {
                // 8-direction flood fill
                std::queue<coord> q;

                q.push(coord{i,j});
                mem.at<u_int8_t>(i,j) = num_segments;
                int segment_size = 1;

                while (q.size() > 0) {
                    struct coord c = q.front();
                    q.pop();

                    for (int m = -1; m <= 1; m++) {
                        for (int n = -1; n <= 1; n++) {
                            int x_coord = c.x + m;
                            int y_coord = c.y + n;
                            if (0 <= x_coord && x_coord < src->rows && 0 <= y_coord && y_coord < src->cols) {
                                if (mem.at<u_int8_t>(x_coord, y_coord) == 0 &&
                                    src->at<uint8_t>(x_coord, y_coord) == 0) {
                                    q.push(coord{x_coord, y_coord});
                                    segment_size++;
                                    mem.at<u_int8_t>(x_coord, y_coord) = (num_segments % 250) + 1; // just for fun
                                }
                            }
                        }
                    }
                }

                // Only consider segments over 64 pixels
                if (segment_size > 64) {
                    num_segments++;
                    segment_size_sum += segment_size;

                    if (segment_size > max_segment_size) {
                        max_segment_size = segment_size;
                    }

                    if (segment_size < min_segment_size) {
                        min_segment_size = segment_size;
                    }
                }
            }
        }
    }

    segment_statistics out = {
            .number_of_segments = num_segments,
            .average_segment_size = ((double) segment_size_sum) / ((double) num_segments),
            .max_segment_size = max_segment_size,
            .min_segment_size = min_segment_size
    };
    return out;
}

#endif //CMSC630_SEGMENT_FLOOD_FILL_H
