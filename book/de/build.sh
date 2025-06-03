#!/bin/bash

echo '*** Generating html ***' &&
asciidoctor QwtBook.adoc &&

echo '*** Generating pdf ***' &&
#asciidoctor-pdf -a pdf-theme=pdf-theme.yml -r ../rouge_theme.rb  -a pdf-fontsdir="../fonts;GEM_FONTS_DIR" QwtBook.adoc &&
#asciidoctor-pdf -a pdf-theme=pdf-theme.yml -r ./rouge_theme.rb -r asciidoctor-mathematical -a pdf-fontsdir="./fonts;GEM_FONTS_DIR" QwtBook.adoc

echo '*** Copying files to ../docs directory' &&

cp QwtBook.html ../../docs/de/index.html &&
mv QwtBook.pdf ../../docs/QwtBook_de.pdf &&

cp -r ../images ../../docs/ &&
cp -r ../fonts ../../docs/ &&
cp -r ../css ../../docs/ &&
cp -r ../code ../../docs/

