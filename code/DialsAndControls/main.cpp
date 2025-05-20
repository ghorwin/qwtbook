

#include <QApplication>
#include <QGridLayout>

#include <QwtKnob>
#include <QwtDial>
#include <QwtDialNeedle>
#include <QwtWheel>
#include <QwtThermo>


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	QWidget main;
	// we create a grid of controls
	QGridLayout * lay = new QGridLayout(&main);

	// QwtWheel
	// QwtKnob

	// QwtDial

	QwtDial * dial = new QwtDial(&main);
	QwtDialNeedle * dialNeedle = new QwtDialSimpleNeedle(QwtDialSimpleNeedle::Arrow);
	dial->setNeedle(dialNeedle);
	dial->setValue(40);
	lay->addWidget(dial, 0, 0);

	// QwtThermo
	QwtThermo * thermo = new QwtThermo(&main);
	thermo->setValue(50);
	lay->addWidget(thermo, 0, 1);

	// QwtWheel
	QwtWheel * wheel = new QwtWheel(&main);
	thermo->setValue(50);
	lay->addWidget(wheel, 1, 0);

	main.setLayout(lay);
	main.show();

	return a.exec();
}
