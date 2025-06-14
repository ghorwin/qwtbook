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
#include <QwtSymbol>
#include <QwtGraphic>
#include <QwtPainter>

class OwnPlotCurve : public QwtPlotCurve {
public:
	QwtGraphic legendIcon(int, const QSizeF & ) const override {
		QwtGraphic graphic;
		QSizeF s(30,16);
		graphic.setDefaultSize( s );
		graphic.setRenderHint( QwtGraphic::RenderPensUnscaled, true );

		QPainter painter( &graphic );
		painter.setRenderHint( QPainter::Antialiasing, false);
#if 0
		QRect r(0, 0, s.width(), s.height() );
		painter.setPen(Qt::black);
		painter.setBrush(pen().color());
		painter.drawRect(r);
#else

		// Mittellinie im Iconrechteck
		const double y = 0.5 * s.height();

		// Hintergrund zeichnen (für den schwarzen Rahmen)
		QPen backgroundPen(Qt::black);
		backgroundPen.setWidth(5);
		backgroundPen.setCapStyle( Qt::FlatCap );
		painter.setPen( backgroundPen );
		QwtPainter::drawLine( &painter, 0.0, y, s.width(), y );

		// Linienfarbe zeichnen)
		QPen pn = pen();
		pn.setCapStyle( Qt::FlatCap );
		pn.setWidth(3);
		painter.setPen( pn );
		QwtPainter::drawLine( &painter, 1, y, s.width()-1, y );
#endif
		return graphic;
	}
};


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
	QVector<double> y2{6,4.2,9,10, 7, 6, 9, 10,12};

	QwtPlotCurve *curve = new OwnPlotCurve();
	curve->setStyle(QwtPlotCurve::Lines);
	curve->setPen(QColor(160,200,255), 2);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->setTitle("Linie 1");
	curve->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
#define WITH_SYMBOL
#ifdef WITH_SYMBOL
	curve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);

	QwtSymbol * symbol = new QwtSymbol(QwtSymbol::Rect);
	symbol->setSize(6);
	symbol->setPen(QColor(0,0,160), 1);
	symbol->setBrush(QColor(160,200,255));
	curve->setSymbol(symbol); // Curve takes ownership of symbol

	// curve->setLegendIconSize(QSize(8,8));
#endif

	curve->attach(&plot); // Plot takes ownership



	curve = new OwnPlotCurve();
	curve->setStyle(QwtPlotCurve::Lines);
	curve->setPen(QColor(0xddaa00), 2);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y2);
	curve->setTitle("Linie 2");
	curve->setLegendAttribute(QwtPlotCurve::LegendShowLine, true);
#ifdef WITH_SYMBOL
	// curve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol, true);

	symbol = new QwtSymbol(QwtSymbol::XCross);
	symbol->setSize(6);
	symbol->setPen(QColor(0xddaa00).darker(120), 2);
	curve->setSymbol(symbol); // Curve takes ownership of symbol
	// curve->setLegendIconSize(QSize(8,8));
#endif

	curve->attach(&plot); // Plot takes ownership

	// Legende anzeigen
	QwtLegend * legend = new QwtLegend();
	QFont legendFont;
	legendFont.setPointSize(8);
	legend->setFont(legendFont);
	plot.insertLegend( legend , QwtPlot::RightLegend); // plot takes ownership


	plot.show();
	return a.exec();
}
