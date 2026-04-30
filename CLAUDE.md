# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this repo is

A programmer's manual ("Qwt Entwicklerhandbuch") for the [Qwt](https://qwt.sourceforge.io/) C++/Qt plotting library. The book is written in German in AsciiDoc format. The `code/` directory contains companion Qt/C++ example projects.

## Repository layout

- `book/de/` — active AsciiDoc source (German). This is where all writing happens.
- `book/de-alt/` — old MkDocs-based version, no longer maintained.
- `book/adoc_utils/` — shared Python helper scripts (link checker, image prep, etc.).
- `book/images/`, `book/fonts/`, `book/css/` — shared assets referenced by the AsciiDoc source.
- `code/` — Qt `.pro` projects, one sub-directory per example/tutorial.
- `docs/` — build output deployed to GitHub Pages (do not edit manually).

## Building the book

All commands run from `book/de/`:

```bash
# HTML only
asciidoctor QwtBook.adoc

# PDF only (requires asciidoctor-pdf and rouge gems)
asciidoctor-pdf -a pdf-theme=pdf-theme.yml -r ../rouge_theme.rb \
  -a pdf-fontsdir="../fonts;GEM_FONTS_DIR" QwtBook.adoc

# Both HTML + PDF, then copy output to docs/
./build.sh

# Check internal/external links
./linkcheck.sh
```

### Required gems (install once)

```bash
sudo apt install asciidoctor
sudo gem install asciidoctor-pdf --pre
sudo gem install rouge asciidoctor-rouge
```

## Building code examples

Each project under `code/` is a qmake project. Expected Qwt install paths:

- Linux: `/usr/local/qwt-6.3.0/`
- Windows: `C:\qwt-6.3.0\`

```bash
cd code/<ProjectName>
qmake && make
```

## AsciiDoc conventions

- The master document is `book/de/QwtBook.adoc`; chapter files are included from it.
- Images live in `book/images/` and are referenced via `:imagesdir: ../images`.
- Image filenames must not contain more than one `.`.
- Qwt class names (`QwtPlot`, `QwtDial`, …) are always formatted as inline code.
- Source blocks use `[source,c++]` with rouge syntax highlighting.
- Images are inserted without explicit scaling; CSS handles sizing.
