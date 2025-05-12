TARGET   = Tutorial1
QT       += core gui widgets
CONFIG   += c++11

# Pfad zu den Qwt Headerdateien hinzufügen
INCLUDEPATH += C:/qwt-6.3.0/include

CONFIG(debug, debug|release) {
	QWTLIB = qwtd
}
else {
	QWTLIB = qwt
}

# Linkerpfad
LIBS += -LC://qwt-6.3.0/lib -l$$QWTLIB

SOURCES += \
	main.cpp
