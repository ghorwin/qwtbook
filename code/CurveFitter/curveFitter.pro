TARGET   = CurveFitter
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
	lessThan(QT_MAJOR_VERSION, 6) {
		# Pfad zu den Qwt Headerdateien hinzufügen
		INCLUDEPATH += /usr/local/qwt-6.3.0/include/
		# Linkerpfad, unter Linux wird standardmäßig nur die release-Version der Lib gebaut und installiert
		LIBS += -L/usr/local/qwt-6.3.0/lib -lqwt
	}
	else {
		INCLUDEPATH += /home/ghorwin/git/_webpages/qwtbook/code/qwt-6.3.0/src \
			/home/ghorwin/git/_webpages/qwtbook/code/qwt-6.3.0/classincludes/
		LIBS += -L/home/ghorwin/git/_webpages/qwtbook/code/qwt-6.3.0/build/Desktop_Qt_6_9_1-Debug/lib/ -lqwt
	}
}


SOURCES += \
	main.cpp
