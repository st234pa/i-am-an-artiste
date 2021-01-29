#include <iostream>

#include "bynumber.h"

using namespace std;

void testPaintByNumber(string in, string out)
{
  Image im(in);
  Image byNumber = paintByNumber(im);
  byNumber.write(out);
}

int main()
{
  testPaintByNumber("./Input/saena_color.png", "./Output/saena_pbn.png");
  return EXIT_SUCCESS;
}