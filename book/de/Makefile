ADOC_FILES  := $(wildcard *.adoc)
IMAGE_FILES := $(shell find ../images -type f)

DOCS_DIR    := ../../docs
HTML_OUT    := QwtBook.html
PDF_OUT     := QwtBook.pdf

.PHONY: all html pdf deploy clean

all: html pdf deploy

html: $(HTML_OUT)

pdf: $(PDF_OUT)

$(HTML_OUT): $(ADOC_FILES) $(IMAGE_FILES)
	asciidoctor QwtBook.adoc

$(PDF_OUT): $(ADOC_FILES) $(IMAGE_FILES)
	asciidoctor-pdf -a pdf-theme=pdf-theme.yml -r ../rouge_theme.rb \
			-a pdf-fontsdir="../fonts;GEM_FONTS_DIR" QwtBook.adoc

deploy: $(HTML_OUT) $(PDF_OUT)
	mkdir -p $(DOCS_DIR)/de
	cp $(HTML_OUT) $(DOCS_DIR)/de/index.html
	cp $(PDF_OUT)  $(DOCS_DIR)/QwtBook_de.pdf
	cp -r ../images $(DOCS_DIR)/
	cp -r ../fonts  $(DOCS_DIR)/
	cp -r ../css    $(DOCS_DIR)/
	cp -r ../code   $(DOCS_DIR)/

clean:
	rm -f $(HTML_OUT) $(PDF_OUT)

