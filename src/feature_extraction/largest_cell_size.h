#include "_segment_flood_fill.cpp"

int largest_cell_size(Mat *src) {
    return _segment_flood_fill(src).max_segment_size;
}
