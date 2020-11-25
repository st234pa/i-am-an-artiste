# i-am-an-artiste
beep boop i'm an impressionist painter

## How to use
### Get the code
Clone or download (and unzip) this project.

### Add input images
Add your image (has to be in `.png` format) to the `Input` sub-folder.

### Modify `main()`
In the `a9_main.cpp` file, change the line
```
testOriented("./Input/saena_color.png", "./Output/saena_color_painted.png", 25, 50000, 0.3f);
```
to call the function on your input image.
In this line you must specify the name of your input file, the name of your output file. You can also adjust the brush stroke size, the number of brush strokes, and the amount of noise (random color variation) if you so choose.

### Run code
To run the code, first open your terminal and then navigate to the project's root directory.
```
cd <path where you stored this project>/i-am-an-artiste
```
Finally run the command below and wait.
```
make run
```
Et voilà!
