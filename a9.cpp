#include <iostream>

#include "a9.h"

using namespace std;

// 2) Paintbrush splatting

void brush(Image &out, int x, int y, Vec3f color, const Image &texture)
{
    for (int c = 0; c < texture.channels(); c++)
    {
        for (int j = 0; j < texture.height(); j++)
        {
            for (int i = 0; i < texture.width(); i++)
            {
                int xi = x + i - texture.width() / 2;
                int yj = y + j - texture.height() / 2;
                if (xi >= 0 && xi < out.width() && yj >= 0 && yj < out.height())
                {
                    out(xi, yj, c) = texture(i, j, c) * color(c) + (1.0f - texture(i, j, c)) * out(xi, yj, c);
                }
            }
        }
    }
}

// 3.1-2) Painterly rendering - single scale

void singleScalePaint(const Image &im, Image &out, const Image &importance, const Image &texture, int size, int N, float noise)
{
    float scale = (float)size / (float)std::max(texture.width(), texture.height());
    Image scaledTexture = scaleNN(texture, scale);

    std::random_device rt;
    std::mt19937 mtX(rt());
    std::uniform_int_distribution<int> xDistribution(0, im.width() - 1);
    std::mt19937 mtY(rt());
    std::uniform_int_distribution<int> yDistribution(0, im.height() - 1);
    std::mt19937 mtR(rt());
    std::uniform_real_distribution<float> importanceDistribution(0.0f, 1.0f);
    std::mt19937 mtN(rt());
    std::uniform_real_distribution<float> noiseDistribution(0.0f, 1.0f);

    int numStrokes = 0;

    while (numStrokes < N)
    {
        int y = yDistribution(mtY);
        int x = xDistribution(mtX);

        bool xBound = x >= scaledTexture.width() / 2 && x < out.width() - scaledTexture.width() / 2 - 1;
        bool yBound = y >= scaledTexture.height() / 2 && y < out.height() - scaledTexture.height() / 2 - 1;

        if (xBound && yBound)
        {
            float i = importanceDistribution(mtR);
            if (i <= importance(x, y))
            {
                numStrokes++;
                Vec3f color;
                color(0) = im(x, y, 0) * (1.0f - noise / 2.0f + noise * noiseDistribution(mtN));
                color(1) = im(x, y, 1) * (1.0f - noise / 2.0f + noise * noiseDistribution(mtN));
                color(2) = im(x, y, 2) * (1.0f - noise / 2.0f + noise * noiseDistribution(mtN));
                brush(out, x, y, color, scaledTexture);
            }
        }
    }
}

// 3.3) Painterly rendering - two scale

Image sharpnessMap(const Image &im, float sigma)
{
    Image L(im.width(), im.height(), im.channels());
    for (int y = 0; y < im.height(); y++)
    {
        for (int x = 0; x < im.width(); x++)
        {
            float w = im(x, y, 0) * 0.3f + im(x, y, 1) * 0.6f + im(x, y, 2) * 0.1f;
            L(x, y, 0) = w;
            L(x, y, 1) = w;
            L(x, y, 2) = w;
        }
    }
    Image blur = gaussianBlur_separable(L, sigma);
    Image high = L - blur;
    Image energy = high * high;
    Image sharpness = gaussianBlur_separable(energy, 4.0f * sigma);
    sharpness = sharpness / sharpness.max();
    return sharpness;
}

Image painterly(const Image &im, const Image &texture, int N, int size, float noise)
{
    Image out(im.width(), im.height(), im.channels());
    Image constantImportance(im.width(), im.height(), im.channels());
    for (int x = 0; x < constantImportance.number_of_elements(); x++)
    {
        constantImportance(x) = 1.0f;
    }
    singleScalePaint(im, out, constantImportance, texture, size, N, noise);
    Image importance = sharpnessMap(im);
    singleScalePaint(im, out, importance, texture, size / 4.0f, N, noise);
    return out;
}

// 4.1) Orientation extraction

Image getBlurredLumi(const Image &im, float sigmaG)
{
    Image lumi = lumiChromi(im)[0];
    return gaussianBlur_separable(lumi, sigmaG);
}

Image computeTensor(const Image &im, float sigmaG, float factorSigma)
{
    Image blurredLumi = getBlurredLumi(im, sigmaG);
    Image Ix = gradientX(blurredLumi);
    Image Iy = gradientY(blurredLumi);
    Image M(im.width(), im.height(), 3);
    for (int y = 0; y < im.height(); y++)
    {
        for (int x = 0; x < im.width(); x++)
        {
            M(x, y, 0) = Ix(x, y) * Ix(x, y);
            M(x, y, 1) = Ix(x, y) * Iy(x, y);
            M(x, y, 2) = Iy(x, y) * Iy(x, y);
        }
    }
    return gaussianBlur_separable(M, sigmaG * factorSigma);
}

Matrix tensorImageToMatrix(const Image &tensor, int x, int y)
{
    Matrix ret(2, 2);
    ret << tensor(x, y, 0), tensor(x, y, 1), tensor(x, y, 1), tensor(x, y, 2);
    return ret;
}

float horizontalAngle(Vec2f v)
{
    float angle = std::atan2(sin(v.y()), cos(v.x()));
    angle = std::fmod(angle, M_2_PI);
    return angle;
}

Vec2f smallestEigenvector(Matrix eigenVectors)
{
    Vec2f ret;
    float smallestNorm;
    for (int c = 0; c < eigenVectors.cols(); c++)
    {
        Vec2f eigenVector = eigenVectors.col(c);
        float eigenvectorNorm = eigenVector.norm();
        if (c == 0 || eigenvectorNorm < smallestNorm)
        {
            smallestNorm = eigenvectorNorm;
            ret = eigenVector;
        }
    }
    return ret;
}

Image computeAngles(const Image &im)
{
    Image ret(im.width(), im.height(), im.channels());
    Image tensorImage = computeTensor(im);
    Vec2f horizontalVector;
    for (int y = 0; y < im.height(); y++)
    {
        for (int x = 0; x < im.width(); x++)
        {
            Matrix structureTensor = tensorImageToMatrix(tensorImage, x, y);
            Eigen::EigenSolver<Matrix> solver(structureTensor);
            Matrix eigenVectors = solver.eigenvectors().real();
            Vec2f smallest = smallestEigenvector(eigenVectors);
            float angle = horizontalAngle(smallest);
            for (int c = 0; c < im.channels(); c++)
            {
                ret(x, y, c) = angle / M_2_PI;
            }
        }
    }
    return ret;
}

// 4.2) Oriented painterly rendering - single scale

void singleScaleOrientedPaint(const Image &im, Image &out, const Image &thetas, const Image &importance, const Image &texture, int size, int N, float noise, int nAngles)
{
    float scale = (float)size / (float)std::max(texture.width(), texture.height());
    Image scaledTexture = scaleNN(texture, scale);

    std::random_device rt;
    std::mt19937 mtX(rt());
    std::uniform_int_distribution<int> xDistribution(0, im.width() - 1);
    std::mt19937 mtY(rt());
    std::uniform_int_distribution<int> yDistribution(0, im.height() - 1);
    std::mt19937 mtR(rt());
    std::uniform_real_distribution<float> importanceDistribution(0.0f, 1.0f);
    std::mt19937 mtN(rt());
    std::uniform_real_distribution<float> noiseDistribution(0.0f, 1.0f);

    int numStrokes = 0;

    while (numStrokes < N)
    {
        int y = yDistribution(mtY);
        int x = xDistribution(mtX);

        bool xBound = x >= scaledTexture.width() / 2 && x < out.width() - scaledTexture.width() / 2 - 1;
        bool yBound = y >= scaledTexture.height() / 2 && y < out.height() - scaledTexture.height() / 2 - 1;

        if (xBound && yBound)
        {
            float i = importanceDistribution(mtR);
            if (i <= importance(x, y))
            {
                numStrokes++;
                Vec3f color;
                color(0) = im(x, y, 0) * (1.0f - noise / 2.0f + noise * noiseDistribution(mtN));
                color(1) = im(x, y, 1) * (1.0f - noise / 2.0f + noise * noiseDistribution(mtN));
                color(2) = im(x, y, 2) * (1.0f - noise / 2.0f + noise * noiseDistribution(mtN));
                float theta = thetas(x, y);
                Image rotatedTexture = rotate(scaledTexture, theta * M_2_PI);
                brush(out, x, y, color, rotatedTexture);
            }
        }
    }
}

// 4.3) Oriented painterly rendering - two scale

Image orientedPaint(const Image &im, const Image &texture, int size, int N, float noise)
{
    Image out(im.width(), im.height(), im.channels());
    Image constantImportance(im.width(), im.height(), im.channels());
    for (int x = 0; x < constantImportance.number_of_elements(); x++)
    {
        constantImportance(x) = 1.0f;
    }
    Image thetas = computeAngles(im);
    singleScaleOrientedPaint(im, out, thetas, constantImportance, texture, size, N, noise);
    Image importance = sharpnessMap(im);
    singleScaleOrientedPaint(im, out, thetas, importance, texture, size / 4.0f, N, noise);
    return out;
}
