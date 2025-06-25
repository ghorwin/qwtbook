TARGET   = WeedingCurveFitter
QT       += core gui widgets
CONFIG   += c++11

c++11 {
	CONFIG           += strict_c++
}

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
	lessThan(QT_MAJOR_VERSION, 4) {
		# Pfad zu den Qwt Headerdateien hinzufügen
		INCLUDEPATH += /usr/local/qwt-6.3.0/include/
		# Linkerpfad, unter Linux wird standardmäßig nur die release-Version der Lib gebaut und installiert
		LIBS += -L/usr/local/qwt-6.3.0/lib -lqwt
	}
	else {
		INCLUDEPATH += /home/ghorwin/git/_webpages/qwtbook/code/qwt-6.3.0/src \
			/home/ghorwin/git/_webpages/qwtbook/code/qwt-6.3.0/classincludes/
		CONFIG(debug, debug|release) {
			LIBS += -L/home/ghorwin/git/_webpages/qwtbook/code/qwt-6.3.0/build/Desktop-Debug/lib -lqwt
		}
		else {
			LIBS += -L/home/ghorwin/git/_webpages/qwtbook/code/qwt-6.3.0/build/Desktop-Release/lib -lqwt
		}
	}
}


SOURCES += \
	main.cpp
