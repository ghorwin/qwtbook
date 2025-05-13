#!/bin/bash

echo '*** Generating html ***'
asciidoctor QwtBook.adoc

echo '*** Generating pdf ***'
asciidoctor-pdf -a pdf-theme=pdf-theme.yml -r ./rouge_theme.rb  -a pdf-fontsdir="./fonts;GEM_FONTS_DIR" QwtBook.adoc
#asciidoctor-pdf -a pdf-theme=pdf-theme.yml -r ./rouge_theme.rb -r asciidoctor-mathematical -a pdf-fontsdir="./fonts;GEM_FONTS_DIR" QwtBook.adoc

echo '*** Copying files to ../docs directory'

mv QwtBook.html ../docs/index.html
mv QwtBook.pdf ../docs/QwtBook.pdf

cp -r images ../docs/
cp -r fonts ../docs/
cp -r css ../docs/

