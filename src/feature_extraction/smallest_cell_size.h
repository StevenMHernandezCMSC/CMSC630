#include "_segment_flood_fill.cpp"

int smallest_cell_size(Mat *src) {
    return _segment_flood_fill(src).min_segment_size;
}
