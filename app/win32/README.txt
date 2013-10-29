How to create the Windows .ico file:
------------------------------------

On the Linux command line, run:

pngtopnm ../data/16x16/pdfviewer.png > pdfviewer16.png
pngtopnm ../data/32x32/pdfviewer.png > pdfviewer32.png
pngtopnm ../data/48x48/pdfviewer.png > pdfviewer48.png
pngtopnm ../data/64x64/pdfviewer.png > pdfviewer64.png
pngtopnm ../data/128x128/pdfviewer.png > pdfviewer128.png
pnmquant 256 pdfviewer32.png > pdfviewer32b.png
pnmquant 256 pdfviewer48.png > pdfviewer48b.png
pnmquant 256 pdfviewer64.png > pdfviewer64b.png
pnmquant 256 pdfviewer128.png > pdfviewer128b.png
mv pdfviewer32b.png pdfviewer32.png
mv pdfviewer48b.png pdfviewer48.png
mv pdfviewer64b.png pdfviewer64.png
mv pdfviewer128b.png pdfviewer128.png
ppmtowinicon -output pdfviewer.ico *.pnm

This does however not preserve transparency, so use GIMP instead:

1. Open ../data/128x128/pdfviewer.png in GIMP:
2. Select "Layer" -> "Layer to Image Size".
3. Select "Layer" -> "Duplicate Layer".
4. Select "Layer" -> "Scale Layer" and scale to 64x64 pixels.
5. Open ../data/64x64/pdfviewer.png, select all, copy, and paste into
   the layer created in step 4.
6. Repeat steps 3-5 for the sizes 48x48, 32x32 and 16x16.
7. Select "File" -> "Save As..." and enter the new name ending in ".ico"
   (without the quotes).
