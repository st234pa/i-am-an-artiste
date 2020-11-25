#include <iostream>

#include "a9.h"

using namespace std;

void testOriented(string in, string out, int size = 50, int N = 7000, float noise = 0.3f)
{
  Image texture("./Input/longBrush2.png");
  Image oriented = orientedPaint(Image(in), texture, size, N, noise);
  oriented.write(out);
}

int main()
{
  // name of input file, name of output file, brush stroke size (default is 50), number of strokes (default is 7000), amount of random color variation (default is 0.3f)
  testOriented("./Input/saena_color.png", "./Output/saena_color_painted.png", 25, 50000, 0.3f);
  return EXIT_SUCCESS;
}
