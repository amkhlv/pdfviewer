#!/bin/sh
OLDPWD=$PWD
cd src/
qdbusxml2cpp -a pdfviewer_adaptor.h: pdfviewer.xml
qdbusxml2cpp -i pdfviewer_adaptor.h -a :pdfviewer_adaptor.cpp pdfviewer.xml
qdbusxml2cpp -c PdfViewerInterface -p pdfviewer_interface.h: pdfviewer.xml
qdbusxml2cpp -c PdfViewerInterface -i pdfviewer_interface.h -p :pdfviewer_interface.cpp pdfviewer.xml
cd $OLDPWD
