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
#include <fstream>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <QwtPlotRenderer>
#include <QwtPlotLayout>
#include <QwtPlotRenderer>
#include <QwtScaleDraw>

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



int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;

	plot.setContentsMargins(8,8,8,8);
	plot.setCanvasBackground( Qt::white );
	plot.resize(1000,800);


	// Daten zum Darstellen generieren
	QVector<double> x, y;
#if 1
	for (unsigned int i=0; i<10000000; ++i) {
		x.append(i);
		y.append(std::sin(i*0.00001));
	}
#else
	// std::ofstream dump("data.tsv");
	// for (unsigned int i=0; i<1000000; ++i) {
	// 	double y = QRandomGenerator64::global()->generateDouble();
	// 	dump << y << '\t';
	// }
	// dump.close();

	std::ifstream in("data.tsv");
	for (unsigned int i=0; i<1000000; ++i) {
		double yval;
		in >> yval;
		x.append(i);
		y.append(yval);
	}

	// y[0] = 0.7;
	// y[1] = 0.5;
	// y[2] = 0.2;
	// y[3] = 0.6;
	// y[4] = 0.2;
	// y[5] = 0.9;
	// y[6] = 0.8;
	// plot.setAxisScale(QwtPlot::yLeft, -1, 2);
	// plot.setAxisScale(QwtPlot::xBottom, 0, 4000);
	// plot.resize(600,400);
	// plot.setAxisScale(QwtPlot::yLeft, -1, 2);
	// plot.setAxisScale(QwtPlot::xBottom, 0, 4000);
#endif

	QwtPlotCurve *curve = new BenchmarkedPlotCurve();
	curve->setPen(QColor(180,40,20), 0);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true); // Antialiasing verwenden
	curve->setPaintAttribute(QwtPlotCurve::FilterPoints, true); // Punktefilter ausschalten
	curve->setPaintAttribute(QwtPlotCurve::FilterPointsAggressive, false); // Punktefilter ausschalten
	curve->setPaintAttribute(QwtPlotCurve::ClipPolygons, true); // TODO : Profile mit und ohne clipping
	curve->setSamples(x, y);
	curve->attach(&plot); // Plot takes ownership

	QwtPlotZoomer * zoomer = new QwtPlotZoomer(plot.canvas());

	// plot.setAxisScale(QwtPlot::xBottom, 150000, 160000);
	// plot.setAxisScale(QwtPlot::yLeft, 0.99, 1);

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

	plot.show();

	return a.exec();
}
