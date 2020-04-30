#include "_segment_flood_fill.cpp"

double average_cell_size(Mat *src) {
    return _segment_flood_fill(src).average_segment_size;
}
