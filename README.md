# one-bit

This tool is aimed at people crafting with thread, in the more literal sense of the word. That is knitting, crocheting, stitching, you name it. 
I've often stumbled upon the problem that I wanted to knit a pattern that had not yet been converted into an actual two-color stitch pattern, or that the gauge for my material wouldn't fit the pattern's aspect ratio.
To solve the problem, I started painting over digital images, hoping the gauge would hold. I succeeded eventually, but one pattern could take up to five hours.

## Automatic generation

### Input Data
The input image must be of JPG or PNG format. Note that for proper pixelation, you need to know the gauge and colors of your yarn and the desired size of the workpiece. 

### Recomputation
From the gauge and result size, the amount and dimensions of one "stixel" (stitch-sized pixel) can directly be computed. 
The algorithm will then determine the overall color of that stixel in original colors, then figure out which of the yarn colors comes closest. 
The output image will be a cropped version of the original one, set as stixels of the aspect ratio computed earlier and of the color determined in the previous step. 

Note that for successful computation, you must specify an output path. The input file is not overwritten unless you explicitly define its path as the output file. 