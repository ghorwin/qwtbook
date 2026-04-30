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

void addIntervalCurves(QwtPlot * plot, const QVector<double>  & x, const QVector<QVector<double> >   & y, const QVector<QColor> & cols) {
	int numSeries = y.count()-1;
	for (int j = 0; j < numSeries; ++j) {
		QwtPlotIntervalCurve *curve = new QwtPlotIntervalCurve();
		QVector<QwtIntervalSample> intervalSamples;
		for (int i = 0; i < x.count(); ++i)
			intervalSamples.append(QwtIntervalSample(x[i], y[j][i], y[j+1][i]));
		curve->setStyle(QwtPlotIntervalCurve::Tube);
		curve->setPen(cols[j+1].darker(120), 2);
		curve->setBrush(cols[j+1]);
		curve->setZ(numSeries - j);
		curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
		curve->setSamples(intervalSamples);
		curve->attach(plot);
	}
}


void computeStackedLines(QVector<QVector<double> > &y) {
	for (int i = 1; i < y.count(); ++i)
		for (int j = 0; j < y[i].count(); ++j)
			y[i][j] += y[i-1][j];
}


void computeStackedLinesWithNegative(QVector<QVector<double> > &y) {
	// y[0] ist die Grundlinie (alles Nullen), y[1..n] sind die Rohdaten der Datenreihen.
	// Ausgabe: y wird ersetzt durch 2*n Einträge — Paare von (unten,oben) pro Datenreihe.
	// Positive Werte stapeln sich nach oben
	// Negative Werte stapeln sich nach unten
	int numSeries = y.count() - 1;
	int numPoints = y[0].count();

	QVector<double> posAccum(numPoints, 0.0);
	QVector<double> negAccum(numPoints, 0.0);

	QVector<QVector<double> > result;
	for (int j = 1; j <= numSeries; ++j) {
		QVector<double> lower(numPoints), upper(numPoints);
		for (int i = 0; i < numPoints; ++i) {
			double v = y[j][i];
			if (v >= 0.0) {
				lower[i] = posAccum[i];
				upper[i] = posAccum[i] + v;
				posAccum[i] += v;
			} else {
				lower[i] = negAccum[i] + v;
				upper[i] = negAccum[i];
				negAccum[i] += v;
			}
		}
		result.append(lower);
		result.append(upper);
	}
	y = result;
}


void computeStackedLinesWithNegative(const QVector<QVector<double> > &x,
									 const QVector<QVector<double> > &y,
									 QVector<QVector<double> > &xMerged,
									 QVector<QVector<double> > &yPos,
									 QVector<QVector<double> > &yNeg)
{
	// === Pass 1: build unified sorted x-vector across all visible curves ===
	// Zero-crossing x-values are pre-inserted so that adjacent unified x-points
	// never straddle a sign change; this removes the need for look-ahead during stacking.
	std::set<double> xSet;

	// process all curves x, y[i]
	for (int i=0; i<y.count(); ++i) {
		const QVector<double> &rawX = x[i];
		const QVector<double> &rawY = y[i];
		const int ni = rawY.size();
		if (ni == 0) continue; // skip empty curves

		// now insert all x coordinates of original x vector into set
		for (int k = 0; k < ni; ++k) {
			xSet.insert(rawX[k]); // NOTE: after first series was processed, this is a n log n operation for all k series
			// also check each interval if a zero-crossing occurs
			if (k + 1 < ni) {
				double y0 = rawY[k], y1 = rawY[k + 1];
				if ((y0 > 0.0 && y1 < 0.0) || (y0 < 0.0 && y1 > 0.0)) {
					double denom = y0 - y1; // may never become 0
					xSet.insert(rawX[k] + (y0 / denom) * (rawX[k + 1] - rawX[k]));
				}
			}
		}
	}

	if (xSet.empty()) return; // no data at all? Skip
}


void stackedPlotSimple(QwtPlot * plot) {
	QVector<double> x{1,2,5,6,10,12,15,16,17};
	QVector<QVector<double> >  y;
	y.append( QVector<double>{0,    0,   0,  0,   0,   0,  0,   0,  0} ); // die Grundlinie
	y.append( QVector<double>{2,  2, 3, 4,  2,  4, 4,  5, 4} );
	y.append( QVector<double>{6,4.4, 9, 8,5.5,5.7, 9, 11, 7} );
	y.append( QVector<double>{7, 6.6, 12, 10, 9, 11, 12, 12, 13});
	const QVector<QColor> cols = {
		QColor(96,60,20),
		QColor(212,91,18),
		QColor(242,188,43)
	};
	addIntervalCurves(plot, x, y, cols);
}


void stackedPlotNegativeSimple(QwtPlot * plot) {
// #if 0
// 	y.append( QVector<double>{0,    0,   0,  0,   0,   0,  0,   0,  0} ); // die Grundlinie
// 	y.append( QVector<double>{2,   -2,  -3,  4,   2,   4, -4,   5,  4} );
// 	// y.append( QVector<double>{6,  4.4,   9,  8, 5.5, 5.7, -9, -11, -7} );
// 	// y.append( QVector<double>{7, -6.6, -12, 10,   9,  11, 12,  12, 13} );
// #endif

// 	QVector<QVector<double> >  x;
// 	for (int i=0; i<y.size(); ++i) {

// 	}

// 	QVector<QVector<double> >  xRes;
// 	QVector<QVector<double> >  yPos;
// 	QVector<QVector<double> >  yNeg;
// 	computeStackedLinesWithNegative(y,y2);
// 	// jetzt wird die Fläche einer jede Datenreihe durch zwei y-Vektoren beschrieben

// 	const QVector<QColor> cols = {
// 		QColor(96,60,20),
// 		QColor(212,91,18),
// 		QColor(212,91,18),
// 		QColor(212,91,18),
// 		QColor(242,188,43)
// 	};

}


void stackedPlotNegative(QwtPlot * plot) {

}


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
	plot.setAxisScale(QwtPlot::yLeft, -10, 20);

	stackedPlotSimple(&plot);
	stackedPlotNegativeSimple(&plot);
	stackedPlotNegative(&plot);

	// stacked plot


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


