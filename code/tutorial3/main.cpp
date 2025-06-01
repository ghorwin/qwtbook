/*

The MIT License (MIT)

Copyright (c) 2025 Andreas Nicolai

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include <QApplication>
#include <QPen>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>

#include <QwtPlot>
#include <QwtPlotCurve>
#include <QwtLegend>
#include <QwtText>
#include <QwtPlotGrid>
#include <QwtLogScaleEngine>
#include <QwtPlotMarker>
#include <QwtPlotZoomer>
#include <QwtPlotPanner>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	plot.setWindowFlags(Qt::FramelessWindowHint);
	plot.resize(400,300);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground( Qt::white );

	// Achenskalierung
	plot.setAxisScale(QwtPlot::yLeft, 0, 20);

	QVector<double> x{1,2,5,6,10,12,15,16,17};
	QVector<double> y{5,4,8,8, 4, 5, 8, 9,11};

#if 1
	unsigned int NUM = 20000;
	x.resize(NUM);
	y.resize(NUM);
	QRandomGenerator rand;

	for (unsigned int i=0; i<NUM; ++i) {
		x[i] = rand.generateDouble()*20;
		double yNominal = 0.4*x[i] + 2;
		// generiere Zufallszahl nach Polar-Methode
		double q, u1;
		do {
			u1 = (rand.generateDouble()-0.5)*2; // -1..1
			double u2 = (rand.generateDouble()-0.5)*2; // -1..1
			q = u1*u1 + u2*u2;
		} while (q == 0 || q >= 1);
		double p = std::sqrt(-2*std::log(q)/q);
		y[i] = yNominal + p*u1*1.5;
	}
#endif
	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setStyle(QwtPlotCurve::Dots);
	QwtText t("QwtPlotCurve::Dots");
	curve->setPen(QColor(0,40,180,128), 2);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->attach(&plot); // Plot takes ownership

	QFont titleFont(qApp->font());
	titleFont.setPointSize(10);
	titleFont.setBold(true);
	t.setFont(titleFont);
	plot.setTitle(t);

	plot.show();
	return a.exec();
}
