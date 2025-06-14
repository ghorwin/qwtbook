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
#include <QwtSplineCurveFitter>
#include <QwtSplinePleasing>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	plot.setWindowFlags(Qt::FramelessWindowHint);
	plot.resize(600,400);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground( Qt::white );

	// Achenskalierung
	plot.setAxisScale(QwtPlot::yLeft, 2, 10);

	QVector<double> x{1,2,5,6,10,12,15,16,17};
	QVector<double> y{5,4,8,8, 4, 5, 8, 9,7};

	// Eine Kurve mit Punkten an den originalen Positionen
	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setStyle(QwtPlotCurve::Dots);
	curve->setPen(QColor(0,40,180), 4);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->attach(&plot); // Plot takes ownership

	// Eine Kurve mit Linien durch die originalen Positionen
	curve = new QwtPlotCurve();
	curve->setStyle(QwtPlotCurve::Lines);
	curve->setPen(QColor(100,100,255), 1);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->attach(&plot); // Plot takes ownership

	// nun die Kurve mit dem Spline Fitter
	curve = new QwtPlotCurve();
	curve->setStyle(QwtPlotCurve::Lines);
	curve->setPen(QColor(0,220,20), 2);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->attach(&plot); // Plot takes ownership
	curve->setZ(-1);

#if 0
	// Kurve ausfüllen
	curve->setBrush(QColor(0xa0d0ff));
	// Bezugslinie setzen
	curve->setBaseline(8);
#endif

	QwtSplineCurveFitter * splineFitter = new QwtSplineCurveFitter;
	QwtSplinePleasing * spline = new QwtSplinePleasing();
	splineFitter->setSpline(spline);
	curve->setCurveFitter(splineFitter);
	// fitting einschalten
	curve->setCurveAttribute(QwtPlotCurve::Fitted, true);

	QwtText t("QwtSplineCurveFitter: QwtSplinePleasing");
	QFont f;
	f.setBold(true);
	f.setPointSize(10);
	t.setFont(f);
	plot.setTitle(t);

	plot.show();
	return a.exec();
}
