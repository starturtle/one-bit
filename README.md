# stixelator

## Motivation
This tool is aimed at people crafting with thread, in the more literal sense of the word. That is knitting, crocheting, stitching, you name it. 
I've often stumbled upon the problem that I wanted to knit a pattern that had not yet been converted into an actual two-color stitch pattern, or that the gauge for my material wouldn't fit the pattern's aspect ratio.
To solve the problem, I started painting over digital images, hoping the gauge would hold. I succeeded eventually, but one pattern could take up to five hours.

## Installation

### Requirements
Disclaimer: I have not made any experiments wrt backwards compatibility yet. 
#### CMake
Configuration of this project requires [CMake](https://cmake.org/download/) to be installed. I'm using 3.15.2. You need CTest if you wish to build the unit tests.
#### Qt5
Building the program such that it has a GUI mode requires a [Qt SDK](https://www.qt.io/download) to be installed. I'm using 5.14.2.
The libqt5 root folder must be part of your CMAKE_MODULE_PATH. You need at least the packages Core, Gui, Network, QmlModes, Qml, QuickCompiler and Quick.
#### Doctest
Testing requires [doctest](https://github.com/onqtam/doctest) on your machine. Only for running the program, it is not necessary.
Doctest is header only, so it doesn't technically get installed. To use it with this program, provide the config entry DOCTEST_INCLUDE_DIR as the path to your doctest copy (root directory) when configuring the project in CMake.

## Automatic generation

### Input Data
The input image must be of JPG or PNG format. Note that for proper pixelation, you need to know the gauge and colors of your yarn and the desired size of the workpiece. 

### GUI Mode
Select the input file from the File menu using "Load...". If successful, the image will show in the input window, and a first preview will be calculated.
Use the text input fields to determine gauge size and desired output size of your workpiece. The preview will adapt.
You can select a ROI from the input image. The aspect ratio is fixed to the one you specified as desired result size. You are not allowed to exceed input image range.
By default, you have two result stitch colors in the list. Using the Change button, you can select different output colors that match your yarn. The Add button allows you to add up to four colors. The Remove button allows you to reduce it back to at least two. 
The stitches are separated by helper lines (complete with a highlight color to help you count). If the colors don't work well with your yarn colors, you can adapt them in the same way. You can also disable helper grids using the check box at the top.
The preview will adjust each time you change properties. When you're done, Select "Save as..." from the File menu.
You can exit the program through the X knob on the program window, or through the "Quit" command from the File menu.