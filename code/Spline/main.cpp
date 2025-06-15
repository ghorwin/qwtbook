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
#include <QwtGraphic>

class OwnPlotCurve : public QwtPlotCurve {
public:
	QwtGraphic legendIcon(int index, const QSizeF & s) const override {
		// if not showing line, always use size 8x8
		if (!testLegendAttribute(LegendShowLine))
			return QwtPlotCurve::legendIcon(index, QSizeF(8,8));
		else
			return QwtPlotCurve::legendIcon(index, s);
	}
};


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
	plot.setAxisScale(QwtPlot::yLeft, 3, 11);
	plot.setAxisScale(QwtPlot::xBottom, 0, 18);

	QVector<double> x{1,2,5,6,10,12,15,16,8};
	QVector<double> y{5,4,8,8, 4, 5, 8, 9,10};
	// QVector<double> x{1,2,5,6,10,12,15,16,17};
	// QVector<double> y{5,4,8,8, 4, 7, 8, 9,7};

	// Eine Kurve mit Punkten an den originalen Positionen
	QwtPlotCurve *curveDots = new OwnPlotCurve();
	curveDots->setStyle(QwtPlotCurve::Dots);
	curveDots->setPen(QColor(0,0,0), 8);
	curveDots->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curveDots->setSamples(x, y);
	curveDots->attach(&plot); // Plot takes ownership
	curveDots->setTitle("Original points");
	curveDots->setLegendIconSize(QSize(30,8));
	curveDots->setZ(-2);

	// Eine Kurve mit Linien durch die originalen Positionen
	QwtPlotCurve * curveLines = new OwnPlotCurve();
	curveLines->setStyle(QwtPlotCurve::Lines);
	curveLines->setPen(QColor(140,140,255), 1);
	curveLines->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curveLines->setSamples(x, y);
	curveLines->setTitle("Bézier-Spline-Interpolation");
	curveLines->attach(&plot); // Plot takes ownership
	curveLines->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
	curveLines->setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);
	curveLines->setLegendIconSize(QSize(30,8));
	curveLines->setZ(1);

	// nun die Kurve mit dem Spline Fitter
	QwtPlotCurve * curveSpline = new OwnPlotCurve();
	curveSpline->setStyle(QwtPlotCurve::Lines);
	curveSpline->setPen(QColor(0,220,20), 2);
	curveSpline->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curveSpline->setSamples(x, y);
	curveSpline->setZ(-1);
	curveSpline->setTitle("Pleasing-Spline-Fitter");
	curveSpline->attach(&plot); // Plot takes ownership
	curveSpline->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
	curveSpline->setLegendIconSize(QSize(30,8));


	QwtSplineCurveFitter * splineFitter = new QwtSplineCurveFitter;
	// QwtSplinePleasing * spline = new QwtSplinePleasing();
	QwtSplineLocal * spline = new QwtSplineLocal(QwtSplineLocal::PChip);
	splineFitter->setSpline(spline);
	curveSpline->setCurveFitter(splineFitter);
	// fitting einschalten
	curveSpline->setCurveAttribute(QwtPlotCurve::Fitted, true);

#if 1
	QPolygonF poly;
	for (int i=0; i<x.count(); ++i)
		poly << QPointF(x[i],y[i]);
	QPolygonF splinePoly = spline->polygon(poly, 1e-2);

	QwtSymbol * symbol = new QwtSymbol(QwtSymbol::Ellipse);
	symbol->setSize(4);
	symbol->setPen(QColor(40,40,200), 0);
	symbol->setBrush(QColor(140,140,255));
	curveLines->setSymbol(symbol); // Curve takes ownership of symbol
	curveLines->setLegendIconSize(QSize(30,8));
	curveLines->setSamples(splinePoly);
#endif

	// Legende anzeigen
	QwtLegend * legend = new QwtLegend();
	QFont legendFont;
	legendFont.setPointSize(8);
	legend->setFont(legendFont);
	plot.insertLegend( legend , QwtPlot::BottomLegend); // plot takes ownership

	QwtPlotZoomer * zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen


	plot.show();
	return a.exec();
}
