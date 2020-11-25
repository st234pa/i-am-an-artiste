#include <iostream>

#include "a9.h"

using namespace std;

void testSingleScale()
{
  Image Liz("./Input/liz.png");
  Image out(Liz.width(), Liz.height(), Liz.channels());
  Image importance(Liz.width(), Liz.height(), Liz.channels());
  for (int x = 0; x < importance.number_of_elements(); x++)
  {
    importance(x) = 1.0f;
  }
  Image texture("./Input/brush.png");
  singleScalePaint(Liz, out, importance, texture);
  out.write("./Output/liz_single_scale.png");
};

void testPainterly()
{
  Image villeperdue("./Input/villeperdue.png");
  Image texture("./Input/brush.png");
  Image out = painterly(villeperdue, texture);
  out.write("./Output/villeperdue_painterly.png");
}

void testAngle()
{
  Vec2f v;
  v << 1.0f, 0.0f;
  cout << horizontalAngle(v) << endl;
  v << 0.0f, 1.0f;
  cout << horizontalAngle(v) << endl;
  v << -1.0f, -0.0f;
  cout << horizontalAngle(v) << endl;
  v << 0.0f, -1.0f;
  cout << horizontalAngle(v) << endl;
}

void testRotate()
{
  Image texture("./Input/brush.png");
  Image out = rotate(texture, 3.0f);
  out.write("./Output/brush_rotated.png");
}

void testComputeAngles()
{
  Image villePerdue("./Input/villeperdue.png");
  Image out(villePerdue.width(), villePerdue.height(), villePerdue.channels());
  Image importance(villePerdue.width(), villePerdue.height(), villePerdue.channels());
  for (int x = 0; x < importance.number_of_elements(); x++)
  {
    importance(x) = 1.0f;
  }
  Image texture("./Input/longBrush2.png");
  Image thetas = computeAngles(villePerdue);
  for (int p = 0; p < thetas.number_of_elements(); p++)
  {
    if (thetas(p) != 0)
    {
      cout << thetas(p) << endl;
    }
  }
}

void testSingleScaleOriented()
{
  Image villeperdue("./Input/villeperdue.png");
  Image out(villeperdue.width(), villeperdue.height(), villeperdue.channels());
  Image importance(villeperdue.width(), villeperdue.height(), villeperdue.channels());
  for (int x = 0; x < importance.number_of_elements(); x++)
  {
    importance(x) = 1.0f;
  }
  Image texture("./Input/longBrush2.png");
  Image thetas = computeAngles(villeperdue);
  singleScaleOrientedPaint(villeperdue, out, thetas, importance, texture, 50, 10000, 0.3f);
  out.write("./Output/villeperdue_single_scale_oriented.png");
}

void testOriented(string in, string out, int size = 50, int N = 7000, float noise = 0.3f)
{
  Image texture("./Input/longBrush2.png");
  Image oriented = orientedPaint(Image(in), texture, size, N, noise);
  oriented.write(out);
}

int main()
{
  testOriented("./Input/linda2.png", "./Output/linda2_painted.png", 50, 80000, 0.3f);
  return EXIT_SUCCESS;
}
