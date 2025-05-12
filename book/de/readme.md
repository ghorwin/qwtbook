# Writer's Notes

Written in 'AsciiDoctor'

See: _https://asciidoctor.org_

## Install (Linux/Ubuntu)

```bash
> sudo apt install asciidoctor 

# Install ruby, and then:

> sudo gem install asciidoctor-pdf --pre
> sudo gem install rouge

# Install rouge syntax highlighter extension
> sudo gem install asciidoctor-rouge
> sudo gem install asciidoctor-mathematical
```

## Generating Documentation

```bash

# html export
> asciidoctor OpenGLQtTutorial.adoc

# pdf export
> asciidoctor-pdf OpenGLQtTutorial.adoc
```

PDF-Export using custom theme:

```bash
echo '*** Generating pdf ***'
asciidoctor-pdf -a pdf-theme=OpenGLQtTutorial-pdf-theme.yml OpenGLQtTutorial.adoc
```

PDF-Export using rouge syntax highlighter

```bash
echo '*** Generating pdf ***'
asciidoctor-pdf -a pdf-theme=OpenGLQtTutorial-pdf-theme.yml -r ./rouge_theme.rb OpenGLQtTutorial.adoc
```


## Writing notes

Use Editor: AsciidocFX _https://asciidocfx.com_

Mind:

- Pictures file names must only have one .

- `QwtPlot`, `QwtDial`, ... - Qwt-Klassen im Fließtext immer als Code-Schnipsel
- Quelltextblöcke immer mit c++ Hervorhebung einfügen (Dreifach schräges Hochkomma)
- Bilder ohne Skalierung einfügen (Skalierung erfolgt durch CSS)


## TODO - Tool chain

PDFs need high-res images, HTML needs images
properly resized to 1000 pixel widths (depending on style sheet)


## Screenshots

### MacOS

Cmd + Shift + 4 - dann Space, Screenshot wird auf dem Schreibtisch gespeichert

### Linux

Shutter

### Windows

SnagIt


