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
#include <QPainterPath>

#include <QwtPlot>
#include <QwtPlotCurve>
#include <QwtLegend>
#include <QwtText>
#include <QwtPlotGrid>
#include <QwtLogScaleEngine>
#include <QwtPlotMarker>
#include <QwtPlotZoomer>
#include <QwtPlotPanner>
#include <QwtPlotIntervalCurve>
#include <QwtScaleMap>

class FixedGradientIntervalCurve : public QwtPlotIntervalCurve {
public:
	void draw(QPainter * painter,
			  const QwtScaleMap & xMap, const QwtScaleMap & yMap,
			  const QRectF & canvasRect) const override
	{
		// min/max y-Pixel berechnen
		QRectF br = boundingRect();
		double topPixel = yMap.transform(br.top());
		double bottomPixel = yMap.transform(br.bottom());
		QLinearGradient grad(0,bottomPixel,0,topPixel);
		grad.setColorAt(0, QColor(60,200,255));
		grad.setColorAt(1, QColor(0,60,120));
		const_cast<FixedGradientIntervalCurve*>(this)->setBrush( QBrush(grad));
		// originale Zeichenfunktion aufrufen
		QwtPlotIntervalCurve::draw(painter, xMap, yMap, canvasRect);
	}
};

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	// plot.setWindowFlags(Qt::FramelessWindowHint);
	plot.resize(400,400);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground( Qt::white );

	// Achenskalierung
	plot.setAxisScale(QwtPlot::yLeft, 0, 20);

#if 0
	QVector<double> x{1,2,5,6,10,12,15,16,17};
	QVector<double> y1{2,2,3,4, 2, 4, 4, 5,11};
	QVector<double> y2{6,4.4,9,10, 5.5, 5.7, 9, 11,12};

	QVector<QwtIntervalSample> intervalSamples;
	for (int i=0; i<x.count(); ++i)
		intervalSamples.append(QwtIntervalSample(x[i],y1[i],y2[i]));

	QwtPlotIntervalCurve *curve = new QwtPlotIntervalCurve();
	curve->setStyle(QwtPlotIntervalCurve::Tube);
	curve->setPen(QColor(0,40,180), 2);
	curve->setBrush( QColor(60,200,255) );
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(intervalSamples);
	curve->attach(&plot); // Plot takes ownership

#endif

#if 0
	QVector<double> x{1,2,5,6,10,12,15,16,17};
	QVector<double> y1{2,2,3,4, 2, 4, 4, 5,11};
	QVector<double> y2{6,4.4,9,10, 5.5, 5.7, 9, 11,12};

	QVector<QwtIntervalSample> intervalSamples;
	for (int i=0; i<x.count(); ++i) {
		intervalSamples.append(QwtIntervalSample(x[i],y1[i],y2[i]));
	}

	QwtPlotIntervalCurve *curve = new QwtPlotIntervalCurve();
	curve->setStyle(QwtPlotIntervalCurve::Tube);
	curve->setPen(QColor(0,40,180), 2);
	QLinearGradient grad(0,90,0,220);
	// grad.setSpread(QGradient::RepeatSpread);
	grad.setColorAt(0, QColor(60,200,255));
	grad.setColorAt(1, QColor(0,60,120));
	curve->setBrush( QBrush(grad));
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(intervalSamples);
	curve->attach(&plot); // Plot takes ownership
#endif

#if 1
	// stacked plot
	QVector<double> x{1,2,5,6,10,12,15,16,17};
	QVector<QVector<double> >  y;
	y.append( QVector<double>{0,  0, 0, 0,  0,  0, 0,  0,  0} );
	y.append( QVector<double>{2,  2, 3, 4,  2,  4, 4,  5, 11} );
	y.append( QVector<double>{6,4.4, 9, 8,5.5,5.7, 9, 11, 12} );
	y.append( QVector<double>{7,6.6,12,10,  9, 11,12, 12, 13} );

	const QColor cols[] = { QColor(96,60,20),
							QColor(156,39,6),
							QColor(212,91,18),
							QColor(242,188,43)
						  };

	for (int j=0;j<y.count()-1; ++j) {
		QwtPlotIntervalCurve *curve = new QwtPlotIntervalCurve();
		QVector<QwtIntervalSample> intervalSamples;
		for (int i=0; i<x.count(); ++i)
			intervalSamples.append(QwtIntervalSample(x[i],y[j][i],y[j+1][i]));
		curve->setStyle(QwtPlotIntervalCurve::Tube);
		curve->setPen(cols[j+1], 2);
		curve->setBrush(cols[j+1].darker(200));
		curve->setZ(y.count()-j);
		curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
		curve->setSamples(intervalSamples);
		curve->attach(&plot); // Plot takes ownership
	}

#endif

	QFont titleFont(qApp->font());
	titleFont.setPointSize(10);
	titleFont.setBold(true);
	QwtText t("QwtIntervalSample");
	t.setFont(titleFont);
	plot.setTitle(t);

	QwtPlotZoomer * zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen

	plot.show();
	return a.exec();
}
