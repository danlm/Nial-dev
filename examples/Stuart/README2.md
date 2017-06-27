#WINDOWS NIAL DEMOS

##Points

1.  Except for the "keyless" demo, each demo requires either IrfanView or gnuplot or both.
    The standard Windows 10 installation of each of these puts the app into
    *C:\\ Program Files (x86)\\*, with a Desktop icon.

2.  If the image files used by a demo are not in the same directory as Nial, the full
    path will be required for that demo's file argument(s).

3.  Each demo is self-contained. Just **loaddefs** it and go. Instructions for using the
    demos are provided below.

5.  The .ndf files "langlet", "image", "display", and "transforms" provide the
    fundamental Nial routines used by the demos.

6.  The standard image formats for the demos are "plain" .pbm, .pgm, and .ppm.
    The "convert" op in irfanview allows conversion between these and almost any
    other known image format.

The following demos are currently available:

|                |                                                 |
| -------------- | ------------------------------------------------|
| gnuplot        | 2D and 3D plot examples                         |
| gnuplot2       | experiment with different palettes for heatmaps |
| image          | processing binary images in Nial                |
| irfanview      | conversion among image file formats, plus some  |
|                | additional image processing ops                 |
| keyless        | keyless cryptography                            |
| langlet        | some applications of Langlet transforms         |
|                | (recovering images from noisy backgrounds)      |
| Lsystem        | generate and display L-system fractals created  |
|                | by block replacement                            |
| Wolfram        | demonstration of Stephen Wolfram's simple       |
|                | cellular automaton                              |


##HOW TO USE THE DEMOS


###gnuplot.ndf

The gnuplot demos illustrate how to create simple 2D and 3D plots with gnuplot from within Nial.

1. demo2d calculates its own data, plots it, and saves the resulting plot as a .png file.

Example, 100 data points:

          demo2d 100

2. demo3d illustrates how to use the gnuplot command to create a surface plot from a datafile (this demo creates its own datafile). Different viewpoints are achieved by mousing around on the plot.

Example, 100 x 100 grid:

         demo3d 100 100


##gnuplot2.ndf

The gnuplot2 demos illustrate a few different ways to display bitmaps, grayscale images, and heatmaps. This demo is only a tiny sample of what is possible with gnuplot. For any of the palette demos below, obtain an image matrix with, for example,

    img := readpgm "jetplane.pgm ;

then generate an image with the desired palette with, for example,

    rainbow img

which will generate a picture with an improved "rainbow" palette.

The .pgm files available for demonstrations are house.pgm, jetplane.pgm, lake.pgm, and lena_gray_256.pgm. The available palettes are: grayscale, rainbow, stoplight, BWR, MATLAB, complex, and geo. The "bitmap" op is an alternative to the irfanview op "disp_bits" and "writepbm" for displaying binary images.


###image_demos.ndf

The image demos illustrate the use of Nial for processing binary images. These demos create binary, grayscale, and color image files, which can be viewed with any image-viewing application capable of displaying .pbm, .pgm, and .ppm files.

1. demo1 and demo2 show the usage of a variety of operations.

Examples:

    demo1 "apple.pbm
    demo2 "bat.pbm

The supplied test images are: apple.pbm, bat.pbm, rat.pbm, frog.pbm, and camel.pbm.

2. demo3 and demo4 show how con:nected components can be found in a binary image.

Examples:

    demo3 "crosses.pbm
    demo3 "objects.pbm

The supplied test images are objects.pbm and crosses.pbm, which are standard images for testing connected components algorithms.

3. demo4 creates its own test image.

    demo4 usage: demo4 n k

where n is the length of the side of the test image and k is the (approximate) number of individual objects in the image. Best results are obtained if k<0.1*n.

The "bw" ops require binary images that are square and have sides whose length is a power of 2. The op "bwprep" can be used to modify a binary image so that it meets these requirements. The supplied test images have already been prepped.


###irfanview.ndf

The irfanview demos illustrate some of the capabilities of IrfanView that can be accessed directly from Nial. Perhaps the most important is the ability to convert from virtually any known image file format to any other. demo1 and demo2 show just a few of the things that can be done with IrfanView. 

Examples:

    demo1 0
    demo2 0

NB: IrfanView retains control until the image being displayed is closed.


###keyless.ndf

The keyless demos demonstrate a keyless cryptographic system described in the paper "Keyless encryption and decryption," which can be found at www.cs.uml.edu/~stu. The system employs the operations of Gerard Langlet's "parity logic" to encode and decode plaintexts.

1. encrypt_demo demonstrates keyless encryption. The first argument is the string to be encrypted, and the second argument is the algorithm to be used, represented as a string.

Example:

    m := encrypt_demo 'This is a string.' 'HEL RCOG BVD'

2. decrypt_demo demonstrates keyless decryption. The first argument is the boolean vector to be decoded, and the second is the algorithm to be used. Using the result m from the example above, the correct decryption would be:

Example:

    decrypt_demo m 'BVI RCOG RCOG HEL'

Additional demonstrations and tests are provided in keyless.ndf. These ops show how to build and test encryption and decryption algorithms and how to obtain performance data.


###anglet_demos.ndf

The langlet.ndf file contains all the ops needed to experiment with Gerard Langlet's "parity logic". The four
demos in http://www.sfml-dev.org/tutorials/2.4/audio-sounds.php langlet_demos.ndf show how Langlet's transforms
can be used to extract information from noisy backgrounds.

1. apple_test: fast detection of an embedded picture in an "easy" image

Examples:

    apple_test 'hel'
    apple_test 'cog'

2. peppers_test: not-so-fast detection of an embedded picture in a "difficult" image

Example:

    peppers_test 0

3. forensic_test: Lifting a fingerprint from a fabric background

Example:

    forensic_test 0

4. steganography_test: Extracting a handwritten message from a security envelope pattern 

Example:

    steganography_test 0


###Lsystem.ndf

The Lsystem demos show how one class of L-system fractals can be generated using just a few simple boolean operations.

1. demo1 generates familiar patterns with 2x2 block replacement. The argument is the number of iterations of replacement. Each time the argument is increased by 1, the side of the square image doubles in length. Use small values, e.g., 2-4, and resize the images manually from the IrfanView toolbar: Image > Resize/Resample.

Example:

    demo1 2

will generate 8x8 instances of several familiar patterns.

2. demo2 provides an opportunity to experiment with block replacement. For the first two arguments enter any pair of integers in the range 0-15. The third argument is the number of iterations of block replacement as in demo1 above. Use small values, e.g., 2-4, and resize the images manually from the IrfanView toolbar: Image > Resize/Resample. The result image has the fractal image on the left, its helical transform in the middle, and its cognitive transform on the right.

Example:

    demo2 9 9 2

will create a standard checkerboard.

3. demo3 creates a "pariton" and its helical and cognitive transforms. The result image has the pariton on the left, its helical transform in the middle, and its cognitive transform on the right. (NB: a pariton is a type of fractal, but not an L-system).

Example:

    demo3 6

will create a random 128x128 pariton and its helical and cognitive transforms.


###Wolfram.ndf

The Wolfram demonstration implements Stephen Wolfram's simple cellular automaton in Nial. The demo function can produce images of the evolution of the automaton using all 256 rules.

The demo op shows 256 steps of the evolution of the cellular automaton with a selected rule. The rule may be any integer 0-255. Some of the more interesting rules are: 18 22 26 30 45 60 73 75 82 86 89 90 101 102 105 110 124 126 129 135 137 146 149 150 153 154 161 165 167 169 181 182 193 195 210 218 225. The image will be immediately displayed by IrfanView, which will retain control until IrfanView is closed.

Examples:

    demo 110

will generate 256 steps of rule 110, which has been proven to be Turing universal.

    demo 30

will generate 256 steps of rule 30, which was the crucial experiment that led to the publication of "A New Kind of Science." Rule 30 is also the basis of Mathematica's random number generator.

For the theory and applications of the simple cellular automaton, see:

Stephen Wolfram. A New Kind of Science. Champaign, IL: Wolfram Media (2002)



