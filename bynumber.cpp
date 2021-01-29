#include <iostream>

#include "bynumber.h"

using namespace std;

const float channelSquaredDifference(float c1, float c2)
{
  return pow(std::abs(abs(c1 - c2)), 2);
}

const float colorDifference(const Image &im, const int x1, const int y1, const int x2, const int y2)
{
  return sqrt(channelSquaredDifference(im(x1, y1, 0), im(x2, y2, 0)) + channelSquaredDifference(im(x1, y1, 1), im(x2, y2, 1)) + channelSquaredDifference(im(x1, y1, 2), im(x2, y2, 2)));
}

const vector<Vec2f> getNeighbors(Vec2f coords, int width, int height)
{
  vector<Vec2f> allDirections = {Vec2f(coords.x() - 1, coords.y()), Vec2f(coords.x() + 1, coords.y()), Vec2f(coords.x(), coords.y() - 1), Vec2f(coords.x(), coords.y() + 1)};
  vector<Vec2f> ret;
  for (int i = 0; i < allDirections.size(); i++)
  {
    if (0 <= allDirections[i].x() && allDirections[i].x() <= width)
    {
      if (0 <= allDirections[i].y() && allDirections[i].y() <= height)
      {
        ret.push_back(allDirections[i]);
      }
    }
  }
  return ret;
}

const set<Vec2f> createCell(Vec2f startCoords, const Image &im, const float tolerance)
{
  set<Vec2f> toClassify;
  for (int x = 0; x < im.width(); x++)
  {
    for (int y = 0; y < im.height(); y++)
    {
      toClassify.insert(Vec2f(x, y));
    }
  }
  set<Vec2f> toVisit = {startCoords};
  set<Vec2f> visited;
  set<Vec2f> ret;
  while (!toVisit.empty())
  {
    Vec2f coords;
    if (colorDifference(im, startCoords.x(), startCoords.y(), coords.x(), coords.y()) < tolerance)
    {
      // add coords to ret
      // remove coords from toClassify
      vector<Vec2f> neighbors = getNeighbors(coords, im.width(), im.height());
      for (int i = 0; i < neighbors.size(); i++)
      {
        Vec2f neighbor = neighbors[i];
        if (true)
        {
        }
      }
    }
    else
    {
      // add coords to visited
    }
  }
  return ret;
}

Image paintByNumber(Image &im, const int numColors)
{
  Image out(im.width(), im.height(), im.channels());
  return out;
}