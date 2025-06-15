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
#include <QwtSplineLocal>
#include <QwtSplineCubic>
#include <QwtSymbol>

QVector<double> x{1,2,5,6,10,12,15,16,8};
QVector<double> y{5,4,8,8, 4, 5, 8, 9,10};

void addSplineFitter(QwtPlot * plot, QwtSpline * spline, QString title, QColor col);

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	plot.setWindowFlags(Qt::FramelessWindowHint);
	plot.resize(1000,600);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground( Qt::white );

	// Achenskalierung
	plot.setAxisScale(QwtPlot::yLeft, 3, 11);
	plot.setAxisScale(QwtPlot::xBottom, 0, 18);

	// Eine Kurve mit Punkten an den originalen Positionen
	QwtPlotCurve *curveDots = new QwtPlotCurve();
	curveDots->setStyle(QwtPlotCurve::Dots);
	curveDots->setPen(QColor(0,40,180), 8);
	curveDots->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curveDots->setSamples(x, y);
	curveDots->setTitle("Original points");
	curveDots->attach(&plot); // Plot takes ownership

#if 1
	addSplineFitter(&plot, new QwtSplinePleasing, "QwtSplinePleasing", QColor(0xddaa00));
	addSplineFitter(&plot, new QwtSplineCubic, "QwtSplineCubic", QColor(0x00b21e));
	addSplineFitter(&plot, new QwtSplineLocal(QwtSplineLocal::PChip), "QwtSplineLocal(PChip)", QColor(0xb20000));
#else
	addSplineFitter(&plot, new QwtSplineLocal(QwtSplineLocal::ParabolicBlending), "QwtSplineLocal(ParabolicBlending)", QColor(0x1c88f6));
	addSplineFitter(&plot, new QwtSplineLocal(QwtSplineLocal::Akima), "QwtSplineLocal(Akima)", QColor(0xb26d00));
	addSplineFitter(&plot, new QwtSplineLocal(QwtSplineLocal::Cardinal), "QwtSplineLocal(Cardinal)", QColor(0xdd00d5));
#endif
	// Legende anzeigen
	QwtLegend * legend = new QwtLegend();
	QFont legendFont;
	legendFont.setPointSize(8);
	legend->setFont(legendFont);
	plot.insertLegend( legend , QwtPlot::RightLegend); // plot takes ownership

	QwtPlotZoomer * zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen


	plot.show();
	return a.exec();
}


void addSplineFitter(QwtPlot * plot, QwtSpline * spline, QString title, QColor col) {
	// nun die Kurve mit dem Spline Fitter
	QwtPlotCurve * curveSpline = new QwtPlotCurve();
	curveSpline->setStyle(QwtPlotCurve::Lines);
	curveSpline->setPen(col, 0);
	curveSpline->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curveSpline->setSamples(x, y);
	curveSpline->attach(plot); // Plot takes ownership
	curveSpline->setTitle(title);
	curveSpline->setZ(-1);

	QwtSplineCurveFitter * splineFitter = new QwtSplineCurveFitter;
	splineFitter->setSpline(spline);
	curveSpline->setCurveFitter(splineFitter);
	// fitting einschalten
	curveSpline->setCurveAttribute(QwtPlotCurve::Fitted, true);

}
