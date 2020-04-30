int calculate_area(int **histogram_buckets) {
    return ((double) (*histogram_buckets)[255]) / ((double) (*histogram_buckets)[0]);
}
