# Image Processing and Analysis in C++

The goal of this work is to process cancer cell photographs with classic image processing techniques.

## Notes

From the given dataset, `super45.BMP` appears to be corrupted. I simply removed this image from the dataset.

## Installation

Install [OpenCV](https://opencv.org/) and [Boost](https://www.boost.org/) on your computer (depends on Operating System).

## Build project 

```
cmake --build ./cmake-build-debug --target cmsc603 -- -j 4
```

## Configuration

The file `batch.yml` contains the list of input and output directories and filters to be run. 

## Run

```
./cmake-build-debug/cmsc603 ./batch.p2.yml
```
