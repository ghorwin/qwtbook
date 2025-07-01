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
#include <QElapsedTimer>
#include <QDebug>
#include <QTimer>

#include <cmath>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_weeding_curve_fitter.h>
#include <qwt_plot_canvas.h>

class BenchmarkedPlotCanvas : public QwtPlotCanvas {
public slots:
	// slot, when called, resizes parent QwtPlot window
	void resizePlot() {
		// ((QWidget*)parent())->resize(1600,1000);
		((QWidget*)parent())->resize(2400,1200);
	}

protected:
	void paintEvent(QPaintEvent * event) override {
		QElapsedTimer timer;
		timer.start();
		QwtPlotCanvas::paintEvent(event);
		qDebug() << "QwtPlotCanvas::paintEvent(): " << timer.elapsed() << "ms";
	}
};


class BenchmarkedWeedingCurveFitter : public QwtWeedingCurveFitter {
public:
	QPolygonF fitCurve(const QPolygonF & polygon) const override {
		QElapsedTimer timer;
		timer.start();
		const QPolygonF & stripped = QwtWeedingCurveFitter::fitCurve(polygon);
		qDebug() << "QwtWeedingCurveFitter::fitCurve():"
				 << polygon.count() << "->" << stripped.count()
				 << "points: " << timer.elapsed() << "ms";
		return stripped;
	}
};


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;

	plot.setContentsMargins(8,8,8,8);
	// eigene Canvas-Klasse mit Benchmark-Wrapper einsetzen
	BenchmarkedPlotCanvas * canvas = new BenchmarkedPlotCanvas;
	plot.setCanvas(canvas);
	plot.setCanvasBackground( Qt::white );

	// Daten zum Darstellen generieren
	QVector<double> x, y;
	for (unsigned int i=0; i<10000000; ++i) {
		x.append(i);
		y.append(std::sin(i*0.00001));
	}

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setPen(QColor(180,40,20), 1);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true); // Antialiasing verwenden
	curve->setPaintAttribute(QwtPlotCurve::FilterPoints, true); // Punktefilter
	curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, true); // Punktefilter
	curve->setSamples(x,y);
	curve->attach(&plot); // Plot takes ownership

	// WeedingCurveFitter mit Benchmark-Wrapper einsetzen
	QwtWeedingCurveFitter * weedingFitter = new BenchmarkedWeedingCurveFitter;
	curve->setCurveFitter(weedingFitter);
	curve->setCurveAttribute(QwtPlotCurve::Fitted, false);

#if 1
	// use curve fitter to reduce data to plot
	QPolygonF poly;
	for (int i=0; i<x.count(); ++i)
		poly << QPointF(x[i],y[i]);
	QwtWeedingCurveFitter weedingAlgorithm(0.001);
	poly = weedingAlgorithm.fitCurve(poly);
	curve->setSamples(poly);
#endif

	plot.show();
	plot.resize(1000,800);

	QTimer::singleShot(1000, canvas, &BenchmarkedPlotCanvas::resizePlot);
	QTimer::singleShot(4000, &plot, &BenchmarkedPlotCanvas::close);

	return a.exec();
}
