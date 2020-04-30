#include "_segment_flood_fill.cpp"

int number_of_cells(Mat *src) {
    return _segment_flood_fill(src).number_of_segments;
}
