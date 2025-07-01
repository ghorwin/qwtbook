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
#include <QElapsedTimer>
#include <QDebug>
#include <QRandomGenerator64>
#include <QPdfWriter>

#include <cmath>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_weeding_curve_fitter.h>
#include <qwt_plot_zoomer.h>
#include <QwtPlotRenderer>
#include <QwtPlotLayout>
#include <QwtPlotRenderer>
#include <QwtScaleDraw>
#include <QwtSymbol>

// Spezialisierte QwtPlotCurve mit Zeitmessung um drawCurve()
class BenchmarkedPlotCurve : public QwtPlotCurve {
protected:
	void drawCurve(QPainter *p, int style,
		const QwtScaleMap & xMap, const QwtScaleMap & yMap,
		const QRectF & canvasRect, int from, int to) const override
	{
		QElapsedTimer timer;
		timer.start();
		QwtPlotCurve::drawCurve(p, style, xMap, yMap, canvasRect, from, to);
		qDebug() << "QwtPlotCurve::drawCurve(): " << timer.elapsed() << "ms";
	}
};



class BenchmarkedWeedingCurveFitter : public QwtWeedingCurveFitter {
public:
	QPolygonF fitCurve(const QPolygonF & polygon) const override {
		QElapsedTimer timer;
		timer.start();
		const QPolygonF & stripped = QwtWeedingCurveFitter::fitCurve(polygon);
		qDebug() << "QwtWeedingCurveFitter::fitCurve():" << polygon.count() << "->" << stripped.count()
				 << "points: " << timer.elapsed() << "ms";
		return stripped;
	}
};


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;

	plot.setContentsMargins(8,8,8,8);
	plot.setCanvasBackground( Qt::white );

	// Daten zum Darstellen generieren
	QVector<double> x, y;
	for (unsigned int i=0; i<10000000; ++i) {
		x.append(i);
#if 1
		y.append(std::sin(i*0.000001)*330);
#else
		y.append(QRandomGenerator64::global()->generateDouble());
#endif
	}

	QwtPlotCurve *curve = new BenchmarkedPlotCurve();
	curve->setPen(QColor(180,40,20), 1);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true); // Antialiasing verwenden
	curve->setPaintAttribute(QwtPlotCurve::FilterPoints, true); // Punktefilter ausschalten
	curve->setPaintAttribute(QwtPlotCurve::FilterPointsAggressive, false); // Punktefilter ausschalten
	// curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, true);
	curve->setSamples(x, y);
	curve->attach(&plot); // Plot takes ownership

	// WeedingCurveFitter mit Benchmark-Wrapper einsetzen
	QwtWeedingCurveFitter * weedingFitter = new BenchmarkedWeedingCurveFitter;
	curve->setCurveFitter(weedingFitter);
	curve->setCurveAttribute(QwtPlotCurve::Fitted, true);

#if 1
	curve = new BenchmarkedPlotCurve();
	curve->setStyle(QwtPlotCurve::NoCurve);
	curve->setPen(QColor(0,80,220), 1);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true); // Antialiasing verwenden
	curve->setPaintAttribute(QwtPlotCurve::FilterPoints, true); // Punktefilter ausschalten
	QwtSymbol * symbol = new QwtSymbol(QwtSymbol::Rect);
	symbol->setSize(8);
	symbol->setPen(QColor(0,0,160), 2);
	symbol->setBrush(QColor(160,200,255));
	curve->setSymbol(symbol); // Curve takes ownership of symbol
	curve->attach(&plot); // Plot takes ownership

	// use curve fitter to fit the curve
	QPolygonF poly;
	for (int i=0; i<x.count(); ++i)
		poly << QPointF(x[i],y[i]);
	weedingFitter->setTolerance(1);
	weedingFitter->setChunkSize(100000);
	poly = weedingFitter->fitCurve(poly);
	curve->setSamples(poly);
	weedingFitter->setTolerance(1);
#endif

	QwtPlotZoomer * zoomer = new QwtPlotZoomer(plot.canvas());

#if 0
	// export the plot
	QwtPlotRenderer renderer;
	renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );

	for  (int i=0; i<4; ++i)
		plot.plotLayout()->setAlignCanvasToScale( i, true );

	plot.axisScaleDraw(QwtPlot::xBottom)->setPenWidthF(1);
	plot.axisScaleDraw(QwtPlot::yLeft)->setPenWidthF(1);

	renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
	renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
	renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );

	QPdfWriter writer("plot.pdf");
	writer.setTitle("Mein plot");
	writer.setCreator("Ich");
	writer.setPageSize(QPageSize(QPageSize::A4));
	writer.setResolution(300);
	writer.setPageOrientation(QPageLayout::Landscape);
	renderer.renderTo( &plot, writer);
#endif

	// plot.setAxisScale(QwtPlot::yLeft, -2, 2);
	// plot.setAxisScale(QwtPlot::xBottom, 522000, 532000);
	// plot.setAxisScale(QwtPlot::xBottom, 0, 1000000);
	plot.resize(1000,800);

	plot.show();

	return a.exec();
}
