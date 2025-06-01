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
#include <QGridLayout>
#include <QRandomGenerator>

#include <QwtPlot>
#include <QwtPlotCurve>
#include <QwtSymbol>
#include <QwtLegend>
#include <QwtText>
#include <QwtPlotGrid>
#include <QwtLogScaleEngine>
#include <QwtPlotMarker>
#include <QwtPlotZoomer>
#include <QwtPlotPanner>

QwtPlot * createPlot(QwtSymbol::Style s, QString title);

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	QWidget w;
	w.setWindowFlags(Qt::FramelessWindowHint);
	QGridLayout * gridLay = new QGridLayout;

	gridLay->addWidget(createPlot(QwtSymbol::Ellipse, "<b>Ellipse</b> (circle)"), 0, 0);
	gridLay->addWidget(createPlot(QwtSymbol::Rect, "<b>Rect</b> (square)"), 0, 1);
	gridLay->addWidget(createPlot(QwtSymbol::Diamond, "<b>Diamond</b>"), 0, 2);
	gridLay->addWidget(createPlot(QwtSymbol::Hexagon, "<b>Hexagon</b>"), 0, 3);

	gridLay->addWidget(createPlot(QwtSymbol::Triangle, "<b>UTriangle</b> or <b>Triangle</b> (upwards)"), 1, 0);
	gridLay->addWidget(createPlot(QwtSymbol::DTriangle, "<b>DTriangle</b> (downwards)"), 1, 1);
	gridLay->addWidget(createPlot(QwtSymbol::LTriangle, "<b>LTriangle</b> (left)"), 1, 2);
	gridLay->addWidget(createPlot(QwtSymbol::RTriangle, "<b>RTriangle</b> (right)"), 1, 3);

	gridLay->addWidget(createPlot(QwtSymbol::HLine, "<b>HLine</b>"), 2, 0);
	gridLay->addWidget(createPlot(QwtSymbol::VLine, "<b>VLine</b>"), 2, 1);
	gridLay->addWidget(createPlot(QwtSymbol::Cross, "<b>Cross</b>"), 2, 2);
	gridLay->addWidget(createPlot(QwtSymbol::XCross, "<b>XCross</b>"), 2, 3);

	gridLay->addWidget(createPlot(QwtSymbol::Star1, "<b>Star1</b> (x and + combined)"), 3, 0);
	gridLay->addWidget(createPlot(QwtSymbol::Star2, "<b>Star2</b>"), 3, 1);

	w.setLayout(gridLay);
	w.resize(1200,1200);
	w.show();
	return a.exec();
}


QwtPlot * createPlot(QwtSymbol::Style s, QString title) {
	QwtPlot * plot = new QwtPlot;

	// etwas Abstand zwischen Rand und Achsentiteln
	plot->setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot->setCanvasBackground( Qt::white );
	plot->resize(300,300);
	plot->setMaximumSize(300,300);

	// Achenskalierung
	plot->setAxisScale(QwtPlot::yLeft, 0, 20);
	plot->enableAxis(QwtPlot::xBottom, false);
	plot->enableAxis(QwtPlot::yLeft, false);

	QVector<double> x{1,2,5,6,10,12,15,16,17};
	QVector<double> y{5,4,8,8, 4, 5, 8, 9,11};

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setStyle(QwtPlotCurve::Lines);
	curve->setPen(QColor(0,40,180), 0);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->attach(plot); // Plot takes ownership

	// Symbol hinzufügen
	QwtSymbol * symbol = new QwtSymbol(s);
	symbol->setSize(10);
	symbol->setPen(QColor(0,0,160), 2);
	symbol->setBrush(QColor(160,200,255));
	curve->setSymbol(symbol); // Curve takes ownership of symbol

	QFont titleFont(qApp->font());
	titleFont.setPointSize(10);
	titleFont.setBold(false);
	QwtText t(title);
	t.setFont(titleFont);
	plot->setTitle(t);

	return plot;
}
