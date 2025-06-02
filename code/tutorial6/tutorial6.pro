TARGET   = Tutorial6
QT       += core gui widgets
CONFIG   += c++11

win32 {
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
}
else {
	# Pfad zu den Qwt Headerdateien hinzufügen
	INCLUDEPATH += /usr/local/qwt-6.3.0/include/
	# Linkerpfad, unter Linux wird standardmäßig nur die release-Version der Lib gebaut und installiert
	LIBS += -L/usr/local/qwt-6.3.0/lib -lqwt
}


SOURCES += \
	main.cpp
