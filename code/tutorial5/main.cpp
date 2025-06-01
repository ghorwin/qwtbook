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
#include <QwtSymbol>

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

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setStyle(QwtPlotCurve::Lines);
	curve->setPen(QColor(0,40,180,128), 2);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->attach(&plot); // Plot takes ownership

// #define PAINTERPATH
#ifdef PAINTERPATH
	// Symbol hinzufügen
	QwtSymbol * symbol = new QwtSymbol(QwtSymbol::Path);
	QwtText t("QwtSymbol::Path");
	QPainterPath p;
	p.addEllipse(QRectF(-10,-10,20,20));
	p.moveTo(-7,-7);
	p.lineTo(7,7);
	p.moveTo(7,-7);
	p.lineTo(-7,7);
	symbol->setPath(p);
	symbol->setPen(QColor(0,0,120), 2);
	symbol->setBrush(QColor(160,200,255));
	curve->setSymbol(symbol); // Curve takes ownership of symbol
#endif

// #define SVG
#ifdef SVG
	QwtSymbol * symbol = new QwtSymbol(QwtSymbol::SvgDocument);
	QwtText t("QwtSymbol::SvgDocument");
	QFile f("symbol.svg");
	f.open(QFile::ReadOnly);
	QTextStream strm(&f);
	QByteArray svgDoc = strm.readAll().toLatin1();
	symbol->setSvgDocument(svgDoc);
	QRect br = symbol->boundingRect(); // size of symbol
	symbol->setPinPoint(QPointF(br.width()/2-1,br.height()-3));
	curve->setSymbol(symbol); // Curve takes ownership of symbol
#endif

// #define PNG
#ifdef PNG
	QwtSymbol * symbol = new QwtSymbol(QwtSymbol::Pixmap);
	QwtText t("QwtSymbol::Pixmap");
	QPixmap pixmap;
	pixmap.load("symbol.png");
	symbol->setPixmap(pixmap);
	QRect br = symbol->boundingRect(); // size of symbol
	symbol->setPinPoint(QPointF(br.width()/2,br.height()-1));
	curve->setSymbol(symbol); // Curve takes ownership of symbol
#endif


	QFont titleFont(qApp->font());
	titleFont.setPointSize(10);
	titleFont.setBold(true);
	t.setFont(titleFont);
	plot.setTitle(t);

	plot.show();
	return a.exec();
}
