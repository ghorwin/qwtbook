# Verwendung der Bibliothek in eigenen Programmen

Wie bei anderen externen Bibliotheken, gibt es grundsätzlich zwei Möglichkeiten, die Qwt-Bibliothek in eigenen Projekten zu verwenden:

1. Verwendung einer installierten, kompilierten Bibliothek unter Verwendung der installierten Headerdateien und Bibliotheken
2. Verwendung der Qwt-Bibliothek im Quelltext innerhalb der eigenen Projektstruktur

Unter Linux-Systemen (bzw. unter MacOSX, wenn Qwt über Homebrew oder MacPorts installiert ist), wird zumeist die Variante 1 sinnvoll sein.

Variante 2 ist stets dann sinnvoll, wenn man Zugriff auf die Implementierung der Qwt-Bibliothek haben möchte und gegebenenfalls eigene Änderungen einbringen möchte, oder wenn man mit einer noch nicht veröffentlichten Version aus einem Quelltextarchiv arbeiten will.

Auch für die Verwendung von Visual Studio unter Windows ist die Variante 2 am sinnvollsten, da so flexibel zwischen Debug und Release-Builds umgeschaltet werden kann (siehe [Visual Studio Projekte](#erstellen-mit-visual-studio)).

## Installation von vorkompilierten Bibliotheken aus Linux-Paketrepositories

Bei populären Linux-Distributionen gibt es Pakete für die Qwt-Bibliothek. Bei Ubuntu sind das beispielsweise:

**Ubuntu 16.04 LTS**:

```bash
# Qwt 5.x unter Verwendung von qt4 (für Qwt 5.2.3-1)
sudo apt install libqwt5-qt4-dev

# Qwt 6.x unter Verwendung von qt5 (für Qwt 6.1.2-5)
sudo apt install libqwt-qt5-dev
```

**Ubuntu 18.04 LTS**:

```bash
# Qwt 5.x unter Verwendung von qt4 (für Qwt 5.2.3-1)
sudo apt install libqwt5-qt4-dev

# Qwt 6.x unter Verwendung von qt4 (für Qwt 6.1.3-1)
sudo apt install libqwt-dev

# Qwt 6.x unter Verwendung von qt5 (für Qwt 6.1.3-1)
sudo apt install libqwt-qt5-dev
```

Bei anderen Linux-Distributionen ist das ähnlich.

## Installation mittels HomeBrew auf dem Mac

Für neuere Mac-Versionen (sierra bis mojave) kann man [HomeBrew](https://brew.sh/index_de) verwenden:

```
brew install qwt
```

Für ältere Versionen kann man Qwt aus dem Quelltextarchiv installieren (siehe unten).

## Installation vom Quelltextarchiv (alle Plattformen)

Die jeweils aktuellste Qwt-Bibliothek kann vom SourceForge-Repository heruntergeladen werden: [Qwt SourceForge Projektseite](https://sourceforge.net/projects/qwt).

Die zip-Datei wird entpackt.

Im Verzeichnis gibt es die Datei `qwtconfig.pri`, welche gut dokumentiert ist und einige Anpassungen an den zu verwendenden/zu erstellenden Qwt-Komponenten erlaubt. Beispielsweise können durch Einschalten der Zeile die Beispiele erstellt werden.

```bash
QWT_CONFIG     += QwtExamples
```

Nach der Anpassung der Konfigurationsdatei wird die Bibliothek erstellt.

### Erstellen und Installation der Bibliothek unter Linux/Mac

**MacOSX Anpassung**: In der Datei `designer/designer.pro` den Text:

		target.path = $${QWT_INSTALL_PLUGINS}
		INSTALLS += target
	}
	else {
		TEMPLATE        = subdirs # do nothing
	}

mit

		target.path = $${QWT_INSTALL_PLUGINS}
		INSTALLS += target

		macx {
			contains(QWT_CONFIG, QwtFramework) {
				QWT_LIB = qwt.framework/Versions/$${QWT_VER_MAJ}/qwt
			}
			else {
				QWT_LIB = libqwt.$${QWT_VER_MAJ}.dylib
			}
			QMAKE_POST_LINK = install_name_tool -change $${QWT_LIB} $${QWT_INSTALL_LIBS}/$${QWT_LIB} $(DESTDIR)$(TARGET)
		}
	}
	else {
		TEMPLATE        = subdirs # do nothing

ersetzen. **MacOSX Anpassung Ende**


Im Wurzelverzeichnis werden auf der Kommandozeile folgende Befehler ausgeführt:

```bash
> qmake
> make    # oder make -j4
> sudo make install
```

Standardmäßig wird Qwt in folgende Verzeichnisse installiert (weswegen sudo benötigt wird):

    /usr/local/qwt-<version>

mit z.B.:

    Include-Dateien in : /usr/local/qwt-6.1.4/include
    Bibliothek in      : /usr/local/qwt-6.1.4/lib
    Dokumentation in   : /usr/local/qwt-6.1.4/doc

### Erstellen und Installation der Bibliothek unter Windows

Je nach verfügbarem Compiler ist das Prozedere etwas anders, aber ähnlich. Ist der Qt Creator samt Compiler bereits eingerichtet, kann man die `qwt.pro`-Datei einfach als Projekt in Qt Creator öffnen und im Debug/Release-Modus erstellen. Man kann dann in den Projekteinstellungen einen _Deployment_-Schritt konfigurieren, und darüber dann die Bibliothek installieren (man benötigt allerdings Schreibrechte im Zielverzeichnis, sodass dieses Verfahren nur bei Installation in einem selbstgewählten Zielverzeichnis außerhalb der Windows System/Programmverzeichnisse funktioniert).

Alternativ braucht man eine Kommandozeile mit entsprechend gesetzten Umgebungsvariablen zum Übersetzen von C++-Programmen (Beispielsweise bei Visual Studio die `vcvarsall.bat` aufrufen). Auch die Pfade zur Qt-Installation müssen gesetzt sein, sodass `qmake` im Pfad ist.

Dann wird wie unter Linux/Mac erst `qmake` und dann (bei VC) `nmake` bzw. `jom` aufgerufen.



# Verwenden der vorkompilierten Bibliothek

## Typische Installationspfade

### Ubuntu 16.04 - Qwt mit Qt5

Installieren via Paketmanager:
```bash
sudo apt install libqwt-qt5-dev
```

Projektkonfiguration:
```bash
# includes
/usr/include/qwt
# library
qwt-qt5
```


## QMake und Qt Creator
...

## CMake
...

## Visual Studio

...

# Erstellung aus dem Quelltext

Die Qwt-Bibliothek kann auch als Quelltext-Archiv heruntergeladen werden. In diesem Fall muss die Bibliothek zuerst erstellt und gegebenenfalls installiert werden. Letzteres kopiert Bibliothek und benötigte Headerdateien ins Installationsverzeichnis, sodass andere Bibliotheken/Programme darauf zugreifen können.

Je nach Entwicklungsumgebung/Build-System kann man die Bibliothek auch ohne Installation verwenden (siehe [Verwendung der vorkompilierten Bibliothek](#verwenden-der-vorkompilierten-bibliothek)).

## Quelltext-Verzeichnisstruktur

Das Quelltextarchiv enthält folgende Verzeichnisstruktur:

```bash
admin         # Deployment-Scripte, in trunk
classincludes # Include-Dateien im Still von Qt5 Includes
designer/     # Quelltext der Designer-Plugins
doc/          # Doxygen Konfigurationsdateien
examples/     # Beispiele
playground/   # Zusätzliche Tests/Beispiele, ab Qwt 6.2
src/          # Der eigentliche Quelltext
tests/        # spez. Komponententests, ab Qwt 6.2
textengines/  # Zusatzkomponenten für Texte (MathML), ab Qwt 6.3

Qwtbuild.pri      # Grundlegende Kompilierungseinstellungen
Qwtconfig.pri     # Auswahl der zu kompilierenden Teile
Qwt.pro           # Master QMake-Datei
```

## Erstellen mit qmake

### Konfiguration

- TODO: QMake Build System und einflussreiche Dateien
- TODO: wo passe ich was an
- TODO: wie erstelle ich Beispiele (-> Examples + Playground)

### Erstellen und (optionale) Installation

Im Basisverzeichnes des Qwt Quelltextarchivs (bezeichnet mit `<qwt-root>`) ist `qmake` aufzurufen und dann `make`, bzw. unter Windows entsprechend `jom` oder `nmake`.

```bash
qmake
make
```

Die Beispiele (sofern konfiguriert) werden in `<qwt-root>/examples/bin` erstellt.

... TODO

## Erstellen mit cmake

## Visual Studio Projektdateien

- mit Erstellen der Bibliothek (Qwt-vcproj)


# Qwt Designer Plugins

...
