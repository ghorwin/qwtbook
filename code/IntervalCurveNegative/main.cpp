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
#include <QwtSymbol>

void addIntervalCurves(QwtPlot * plot, const QVector<double>  & x, const QVector<QVector<double> >   & y, const QVector<QColor> & cols) {
	int numSeries = y.count()-1;
	for (int j = 0; j < numSeries; ++j) {
		QwtPlotIntervalCurve *curve = new QwtPlotIntervalCurve();
		QVector<QwtIntervalSample> intervalSamples;
		for (int i = 0; i < x.count(); ++i)
			intervalSamples.append(QwtIntervalSample(x[i], y[j][i], y[j+1][i]));
		curve->setStyle(QwtPlotIntervalCurve::Tube);
		curve->setPen(cols[j+1], 2);
		curve->setBrush(cols[j+1].lighter(120));
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


void computeStackedLinesWithNegative(const QVector<QVector<double> > &y, QVector<QVector<double> > &yPos, QVector<QVector<double> > &yNeg) {
	// y[0] ist die Grundlinie (alles Nullen), y[1..n] sind die Rohdaten der Datenreihen.
	// Ausgabe: y wird ersetzt durch 2*n Einträge — Paare von (unten,oben) pro Datenreihe.
	// Positive Werte stapeln sich nach oben
	// Negative Werte stapeln sich nach unten
	int numSeries = y.count();
	int numPoints = y[0].count();

	QVector<double> posAccum(numPoints, 0.0);
	QVector<double> negAccum(numPoints, 0.0);

	// add base line
	yPos.append(posAccum);
	yNeg.append(posAccum);
	QVector<QVector<double> > result;
	for (int j = 0; j < numSeries; ++j) {
		QVector<double> lower(numPoints), upper(numPoints);
		for (int i = 0; i < numPoints; ++i) {
			double v = y[j][i];
			// positive values stack to positive accumulater, negative to negative accumulator
			if (v >= 0.0)
				posAccum[i] += v;
			else
				negAccum[i] += v;
			upper[i] = posAccum[i];
			lower[i] = negAccum[i];
		}
		yNeg.append(lower);
		yPos.append(upper);
	}
}


void computeStackedLinesWithNegative2(const QVector<QVector<double> > &x,
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
		QColor(156,39,6),
		QColor(212,91,18),
		QColor(242,188,43)
	};
	addIntervalCurves(plot, x, y, cols);
}


void stackedPlotNegativeSimple(QwtPlot * plot) {
	QVector<double> x{1,2,5,6,10};
	QVector<QVector<double> >  y;
	y.append( QVector<double>{0,    0,   0,  0,   0} ); // die Grundlinie
	y.append( QVector<double>{2,   -2,  -3,  4,   2} );
	y.append( QVector<double>{6,    5,   3,  4,  -5} );
	y.append( QVector<double>{7,   -6,  -3, -2,   3} );
	const QVector<QColor> cols = {
		QColor(96,60,20),
		QColor(156,39,6),
		QColor(212,91,18),
		QColor(242,188,43)
	};
	computeStackedLines(y);
	addIntervalCurves(plot, x, y, cols);
}


void stackedPlotNegative(QwtPlot * plot) {
	QVector<double> x{1,2,5,6,10};
	QVector<QVector<double> >  y;
	y.append( QVector<double>{0,    0,   0,  0,   0} ); // die Grundlinie
	y.append( QVector<double>{2,   -2,  -3,  4,   2} );
	y.append( QVector<double>{6,    5,   3,  4,  -5} );
	y.append( QVector<double>{7,   -6,  -3, -2,   3} );
	const QVector<QColor> cols = {
		QColor(96,60,20),
		QColor(156,39,6),
		QColor(212,91,18),
		QColor(242,188,43)
	};
	QVector<QVector<double> > yPos;
	QVector<QVector<double> > yNeg;
	computeStackedLinesWithNegative(y, yPos, yNeg);
	addIntervalCurves(plot, x, yPos, cols);
	addIntervalCurves(plot, x, yNeg, cols);
}


void addCurve(QwtPlot * plot, QVector<double> x, QVector<double> y, QColor col) {

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setStyle(QwtPlotCurve::Lines);
	curve->setPen(col, 0);
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->attach(plot); // Plot takes ownership

	// Symbol hinzufügen
	QwtSymbol * symbol = new QwtSymbol(QwtSymbol::Ellipse);
	symbol->setSize(10);
	symbol->setPen(Qt::black,0);
	symbol->setBrush(col.lighter(100));
	curve->setSymbol(symbol); // Curve takes ownership of symbol
}

void addZeroCrossings(QVector<double> & x, QVector<double> & y) {
	const int n = y.size();
	if (n == 0)
		return; // skip empty curves

	QVector<double> newX, newY;
	newX.reserve(x.size());
	newY.reserve(x.size());

	// now insert all x coordinates of original x vector into set
	for (int i = 0; i < n; ++i) {
		// add original point
		newX.append(x[i]);
		newY.append(y[i]);
		// also check each interval if a zero-crossing occurs
		if (i + 1 < n) {
			double y0 = y[i], y1 = y[i + 1];
			if ((y0 > 0.0 && y1 < 0.0) || (y0 < 0.0 && y1 > 0.0)) {
				double denom = y0 - y1; // may never become 0
				// interpolate x value   0 = x_0 + (y1-y0)/(x1-x0)*x_0
				newX.append(x[i] + (y0 / denom) * (x[i + 1] - x[i]));
				newY.append(0);
			}
		}
	}
	x.swap(newX);
	y.swap(newY);
}

void mergeCoordinates(const QVector<QVector<double> > & x, QVector<QVector<double> > & y, QVector<double> & unifiedX) {
	// insert all x-coordinates from all curves into a unified x-vector
	std::set<double> xSet;
	int seriesCount = x.count();
	for (int k=0; k<x.count(); ++k)
		for (double val : x[k])
			xSet.insert(val); // this is an n-log-n operation
	unifiedX = QVector<double>(xSet.begin(), xSet.end());

	// now process all y values and interpolate y values for x values not in the current series
	for (int k=0; k<seriesCount; ++k) {
		if (x[k].count() == 0) {
			// curve is empty, fill with y=0 values for all unifiedX
			y[k] = QVector<double>(unifiedX.count(), 0.0);
			continue;
		}
		const QVector<double> & xk = x[k];
		QVector<double> & yk = y[k];
		QVector<double> newY;
		newY.reserve(unifiedX.size());
		double xLast = xk.back();
		int i_unified = 0; // index counter for unified x values
		int i = 0;         // index counter for current series
		int n_unified = unifiedX.count();
		int n = xk.count();
		// process all x values in original vector and in current series' x vector
		// both are strictly monotonically increasing
		while (i_unified < n_unified && i < n) {
			// handle several cases:
			// unifiedX[i_unified] < x[0]  -> insert x[i_unified] and y=0
			// unifiedX[i_unified] < xLast -> insert x[i_unified] and y=0
			double xUni = unifiedX[i_unified];
			if (xUni < xk[0] || xUni > xLast) {
				newY.append(0);
				++i_unified;
				continue;
			}

			// standard case 1: x values match merged x values
			if (xk[i] == xUni) {
				newY.append(yk[i]);
				++i;
				++i_unified;
				continue;
			}

			// standard case 2: x value > unified x value: we need to interpolate
			double y1 = yk[i];
			double y0 = yk[i-1];
			double dx = xk[i] - xk[i-1];
			// y = (x - x0)/(x1-x0)(y1-y0) + y0
			double yinterpol = (xUni - xk[i-1])/dx*(y1-y0) + y0;
			newY.append(yinterpol);
			++i_unified; // advance unified x only
		}
		yk.swap(newY);
	}
}

void fullAlgorithm(QwtPlot * plot) {
	QVector<double> x_initial{1,2,5,6,10};
	QVector<QVector<double> >  y;
	y.append( QVector<double>{2,   -2,  -3,  4,   2} );
	y.append( QVector<double>{6,    5,   3,  4,  -5} );
	y.append( QVector<double>{7,   -6,  -3, -2,   3} );
	QVector<QColor> cols = {
		QColor(0xa00000), // #a00000
		QColor(0x208000), // #208000
		QColor(0x200080)  // #200080
	};
	// for (int i=0; i<y.count(); ++i)
	// addCurve(plot, x_initial, y[i], cols[i]);

	// step 1 : compute individual x vectors by interpolating y values
	QVector<QVector<double> >  x(y.count());
	for (int i=0; i<y.count(); ++i) {
		x[i] = x_initial;
		addZeroCrossings(x[i], y[i]); // in-place operation
	}
	// for (int i=0; i<y.count(); ++i)
	// 	addCurve(plot, x[i], y[i], cols[i]);

	// step 2 : merge x-coordinates and interpolate y values
	QVector<double> unifiedX;
	mergeCoordinates(x, y, unifiedX); // inplace operation

	// for (int i=0; i<y.count(); ++i) {
	// 	x[i] = unifiedX;
	// 	addCurve(plot, x[i], y[i], cols[i]);
	// }

	// // Achenskalierung
	plot->resize(600,400);
	// plot->setAxisScale(QwtPlot::yLeft, -8, 8);

	// now add lines
	QVector<QVector<double> > yPos;
	QVector<QVector<double> > yNeg;
	computeStackedLinesWithNegative(y, yPos, yNeg);

	// and plot stacked curves positive and negative
	cols.prepend(Qt::black);
	addIntervalCurves(plot, unifiedX, yPos, cols);
	addIntervalCurves(plot, unifiedX, yNeg, cols);

	plot->setAxisScale(QwtPlot::yLeft, -10, 18);
}

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	// plot.setWindowFlags(Qt::FramelessWindowHint);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground( Qt::white );

	// steps of demonstrating
	// 1. show original lines in regular plot, only positive values, x-values overlapping
	// 2. show stacked lines, just by adding up lines
	// 3. show original lines, this time with non-equidistant x values
	// 4. show algorithm to create unified x-value set and interpolate y-values for all curves
	// 5. show stacked lines obtained by adding up lines (same as 2)
	// 6. show original lines, this time with equidistant x-values, but negative values
	// 7. show stacked lines by adding up (same as 2) -> wrong result
	// 8. show algorithm to add up negative and positive contributions and plot 2 times the stacked curves
	// 9. show original lines, non-equidistant spaced, both positive and negative lines
	// 10. show calculation steps (4 and 8) and stacked lines obtained with that

	// stackedPlotSimple(&plot);
	//stackedPlotNegativeSimple(&plot);
	// stackedPlotNegative(&plot);

	// stacked plot
	fullAlgorithm(&plot);


	QFont titleFont(qApp->font());
	titleFont.setPointSize(10);
	titleFont.setBold(true);
	// QwtText t("QwtIntervalSample");
	// t.setFont(titleFont);
	// plot.setTitle(t);

	QwtPlotZoomer * zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen

	QwtPlotGrid *grid = new QwtPlotGrid();
	QPen gridPen(Qt::gray);
	gridPen.setStyle(Qt::DashLine);
	grid->setMajorPen(gridPen);
	grid->attach( &plot );

	plot.show();
	return a.exec();
}


