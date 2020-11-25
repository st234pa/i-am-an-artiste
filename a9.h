#ifndef A9_H_PHUDVTKB
#define A9_H_PHUDVTKB

#include "Image.h"
#include "basicImageManipulation.h"
#include "filtering.h"
#include "matrix.h"
#include <random>

// Write your declarations here, or extend the Makefile if you add source
// files

// 2) Paintbrush splatting
void brush(Image &out, int y, int x, Vec3f color, const Image &texture);

// 3.1-2) Painterly rendering - single scale
void singleScalePaint(const Image &im, Image &out, const Image &importance, const Image &texture, int size = 10, int N = 1000, float noise = 0.3f);

// 3.3) Painterly rendering - two scale
Image painterly(const Image &im, const Image &texture, int N = 10000, int size = 50, float noise = 0.3f);

// 4.1) Orientation extraction
Image computeAngles(const Image &im);

// 4.2) Oriented painterly rendering - single scale
void singleScaleOrientedPaint(const Image &im, Image &out, const Image &thetas, const Image &importance, const Image &texture, int size, int N, float noise, int nAngles = 36);

// 4.3) Oriented painterly rendering - two scale
Image orientedPaint(const Image &im, const Image &texture, int size = 50, int N = 7000, float noise = 0.3f);

// Helper functions
Image sharpnessMap(const Image &im, float sigma = 1.0f);
Image getBlurredLumi(const Image &im, float sigmaG);
Image computeTensor(const Image &im, float sigmaG = 3.0f, float factorSigma = 5.0f);
Matrix tensorImageToMatrix(const Image &tensor, int x, int y);
float horizontalAngle(Vec2f v);
Vec2f smallestEigenvector(Matrix eigenVectors);

#endif /* end of include guard: A9_H_PHUDVTKB */
