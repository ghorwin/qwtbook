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

Entweder man entpackt die zip-Datei eines Releases, oder man checkt the Quelltext des subversion-Repositories aus. Das Quelltextarchiv (aktueller Stand im Repository) enthält folgende Verzeichnisstruktur:

```bash
admin         # Deployment-Scripte, nur im Repository
classincludes # Include-Dateien im Still von Qt5 Includes (ab Qwt 6.2)
designer/     # Quelltext der Designer-Plugins
doc/          # Doxygen Konfigurationsdateien
examples/     # Beispiele
playground/   # Zusätzliche Tests/Beispiele, ab Qwt 6.2
src/          # Der eigentliche Quelltext
tests/        # spez. Komponententests, ab Qwt 6.2
textengines/  # Zusatzkomponenten für Texte (MathML), ab Qwt 6.3

qwtbuild.pri      # Grundlegende Kompilierungseinstellungen
qwtconfig.pri     # Auswahl der zu kompilierenden Teile
qwt.pro           # Haupt QMake-pro-Datei
```

Im Verzeichnis gibt es die Datei `qwtconfig.pri`, welche gut dokumentiert ist und einige Anpassungen an den zu verwendenden/zu erstellenden Qwt-Komponenten erlaubt. Beispielsweise können durch Einschalten der Zeile die Beispiele erstellt werden.

```bash
QWT_CONFIG     += QwtExamples
```

Nach der Anpassung der Konfigurationsdatei wird die Bibliothek erstellt.

### Erstellen und Installation der Bibliothek unter Linux/Mac

----
**MacOSX Anpassung**:

In der Datei `designer/designer.pro` den Text:

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

ersetzen.

**MacOSX Anpassung Ende**

----

Im Wurzelverzeichnis werden auf der Kommandozeile folgende Befehler ausgeführt:

```bash
> qmake
> make    # oder make -j4
> sudo make install
```

Standardmäßig wird Qwt in folgende Verzeichnisse installiert (weswegen sudo benötigt wird):

    /usr/local/qwt-<version>

also z.B.:

    Include-Dateien in : /usr/local/qwt-6.1.4/include
    Bibliothek in      : /usr/local/qwt-6.1.4/lib
    Dokumentation in   : /usr/local/qwt-6.1.4/doc

### Erstellen und Installation der Bibliothek unter Windows

Je nach verfügbarem Compiler ist das Prozedere etwas anders, aber meist sehr ähnlich. Ist der Qt Creator samt Compiler bereits eingerichtet, kann man die `qwt.pro`-Datei einfach als Projekt in Qt Creator öffnen und im Debug/Release-Modus erstellen. Man kann dann in den Projekteinstellungen einen _Deployment_-Schritt konfigurieren, und darüber dann die Bibliothek installieren.

Alternativ braucht man eine Kommandozeile mit entsprechend gesetzten Umgebungsvariablen zum Übersetzen von C++-Programmen (Beispielsweise bei Visual Studio die `vcvarsall.bat` aufrufen). Auch die Pfade zur Qt-Installation müssen gesetzt sein, sodass `qmake` im Pfad ist.

Beispiel:

```batch
:: Entwicklungsumgebung für VC (x64) einrichten
C:\Downloads\qwt-6.1.4>"c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64

:: Pfad zu Qt setzen
C:\Downloads\qwt-6.1.4>set PATH=%PATH%;C:\Qt\5.11.3\msvc2015_64\bin

:: qmake ausführen
C:\Downloads\qwt-6.1.4>qmake
Info: creating stash file C:\Downloads\qwt-6.1.4\.qmake.stash

:: erstellen
C:\Downloads\qwt-6.1.4>nmake

:: installieren
C:\Downloads\qwt-6.1.4>nmake install
```

Standardmäßig wird Qwt in folgende Verzeichnisse installiert:

    C:\Qwt-<version>

also z.B.:

    Include-Dateien in : C:\Qwt-6.1.4\include
    Bibliothek in      : C:\Qwt-6.1.4\lib
    Dokumentation in   : C:\Qwt-6.1.4\doc

> Mächte man "am Ball" bleiben und Qwt gelegentlich aktualisieren, so kann man das Verzeichnis nach `C:\Qwt-6.1` umbenennen, und so die Konfiguration abhängiger Projekte längere Zeit unverändert lassen, auch wenn man zwischenzeitlich eine neue Qwt-Version installiert.

# Verwenden der installierten Bibliothek

## Typische Installationspfade bei vorkompilierten Bibliotheken aus dem Paketmanager

Die Installationspfade sind von der jeweiligen Linux-Distribution abhängig, üblicherweise liegen die Bibliotheken und Include-Pfade jedoch in den systemweiten Standardsuchverzeichnissen. Das macht die Verwendung einfach.

Bei manuell installierten Bibliotheken aus dem Quelltextarchiv, können die Pfade beliebig über den Install-Prefix angepasst werden - oder man verwendet die oben aufgeführten Standardpfade.

## Konfiguration von Projektdateien

Unabhängig vom Buildsystem (qmake, cmake, Visual Studio, ...) muss der Pfad zur Datei `qwt.h` im Suchpfad stehen. Ebenso muss der Linker die `libqwt-xxx.so`-Datei finden (bzw. bei statisch erstellter Qwt-Bibliothek die Datei `libqwt.a`).

### Beispielkonfiguration - Windows - Qmake

```
INCLUDEPATH += C:\Qwt-6.1.4\include

LIBS += -LC:\Qwt-6.1.4\lib -lqwt
```

Im Debug-Modus kann auch `-lqwtd` verwendet werden.

### Beispielkonfiguration - Linux/Mac - Qmake - bei Installation aus Quelltextarchiv

```
INCLUDEPATH += /usr/local/qwt-6.1.4/include

LIBS += -L/usr/local/qwt-6.1.4/lib -lqwt
```

### Beispielkonfiguration - Linux/Mac - Qmake - bei Installation aus Paketquelle

```
INCLUDEPATH += /usr/include/qwt

LIBS += -L/usr/lib -lqwt-qt5
```

Der tatsächliche Bibliotheksname hängt von dem jeweiligen Paket der Linux-Distribution ab. Notfalls mal im Verzeichnis `/usr/lib` nachschauen, was da an `libqwt....`-Dateie existiert.

## Suchpfade für Laufzeitbibliotheken

Unter Windows muss man entweder den Pfad zur Qwt-DLL in die PATH-Variable eintragen, oder man kopiert einfach die DLL in das Verzeichnis der eigenen exe-Datei (das ist zumeist der einfachste Weg - hierbei je nach Debug/Release-Modus die passende `qwtd.dll` und `qwt.dll` linken und kopieren).

Unter Linux kann man den LD_LIBRARY_PATH anpassen oder mittels RPATH den Laufzeitpfad in die Anwendung hineinkompilieren. Mehr dazu in der Qwt-Dokumentation unter `doc/html/qwtinstall.html`.

Auch praktikabel ist unter beiden Plattformen die Erstellung und Verwendung von Qwt als statischer Bibliothek. Dies kann in der `qwtconfig.pri` eingestellt werden (Zeile `QWT_CONFIG           += QwtDll` auskommentieren!).


# Erstellen mit Visual Studio

... TODO 
