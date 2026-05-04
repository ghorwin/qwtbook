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
#include <QDateTime>
#include <QTimeZone>
#include <QElapsedTimer>

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

class BenchmarkedPlot : public QwtPlot {
public:
	// QWidget interface
protected:
	void paintEvent(QPaintEvent * event) {
		QElapsedTimer timer;
		timer.start();
		QwtPlot::paintEvent(event);
		// qInfo() << "QwtPlot::paintEvent(): " << timer.elapsed() << "ms";
	}

	// QwtPlot interface
	void drawCanvas(QPainter *p) {
		QElapsedTimer timer;
		timer.start();
		QwtPlot::drawCanvas(p);
		qInfo() << "QwtPlot::drawCanvas(): " << timer.elapsed() << "ms";

	}
};


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

// Hilfsfunktion zum Hinzufügen einer Intervallkurve
void addIntervalCurve(QwtPlot * plot,
					  const QVector<double>  & x,
					  const QVector<QVector<double> > & y,
					  const QVector<QColor> & cols)
{
	int numSeries = y.count()-1;
	for (int j = 0; j < numSeries; ++j) {
		QwtPlotIntervalCurve *curve = new QwtPlotIntervalCurve();
		QVector<QwtIntervalSample> intervalSamples;
		for (int i = 0; i < x.count(); ++i)
			intervalSamples.append(QwtIntervalSample(x[i], y[j][i], y[j+1][i]));
		curve->setStyle(QwtPlotIntervalCurve::Tube);
		curve->setPen(cols[j+1].darker(150), 0);
		curve->setBrush(cols[j+1].lighter(120));
		curve->setZ(numSeries - j);
		curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
		curve->setSamples(intervalSamples);
		curve->attach(plot);
	}
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
		return; // leere Kurven überspringen

	QVector<double> newX, newY;
	newX.reserve(x.size());
	newY.reserve(x.size());

	// Alle Originalpunkte und dazwischenliegende Intervalle durchlaufen
	for (int i = 0; i < n; ++i) {
		// Originale Punkte behalten
		newX.append(x[i]);
		newY.append(y[i]);
		// Darauffolgendes Interval auf Nulldurchgang prüfen
		if (i + 1 < n) {
			double y0 = y[i], y1 = y[i + 1];
			if ((y0 > 0.0 && y1 < 0.0) || (y0 < 0.0 && y1 > 0.0)) {
				double denom = y0 - y1; // may never become 0
				// X Wert interpolieren 0 = x_0 + (y1-y0)/(x1-x0)*x_0
				newX.append(x[i] + (y0 / denom) * (x[i + 1] - x[i]));
				newY.append(0);
			}
		}
	}
	x.swap(newX);
	y.swap(newY);
}


void mergeCoordinates(
	const QVector<QVector<double> > & x,
	QVector<QVector<double> > & y,
	QVector<double> & unifiedX)
{
	// Alle Koordinaten von allen Vektoren in ein einheitliches Set einfügen
	std::set<double> xSet;
	int seriesCount = x.count();
	for (int k=0; k<x.count(); ++k)
		for (double val : x[k])
			xSet.insert(val);
			// insert() ist eine potentiell langsame Operation, weil der Baum
			// des Sets durchlaufen werden muss
	unifiedX = QVector<double>(xSet.begin(), xSet.end());

	// Nun alle x-Werte aller Serien durchlaufen und y-Werte für fehlende Stützstellen interpolieren.
	// Da hinterher alle Datenreihen die gleichen Stützstellen haben,
	// brauchen wir nur die y-Werte zu aktualisieren.
	for (int k=0; k<seriesCount; ++k) {
		if (x[k].count() == 0) {
			// Kurve ist leer, mit 0-Werten auffüllen
			y[k] = QVector<double>(unifiedX.count(), 0.0);
			continue;
		}

		const QVector<double> & xk = x[k];  // Lesbarkeitsvereinfachung
		QVector<double>       & yk = y[k];  // Lesbarkeitsvereinfachung
		QVector<double>       newY;         // Vektor für neue y-Werte
		newY.reserve(unifiedX.size());
		double xLast = xk.back();
		int i_unified = 0; // Indexzähler für unifiedX Werte
		int i = 0;         // Indexzähler für x-Werte der aktuellen Serie
		int n_unified = unifiedX.count();
		int n = xk.count();
		// Die x-Vectoren (unifiedX und xk) sind beide streng monoton steigend
		while (i_unified < n_unified && i < n) {
			// Fallunterscheidungen
			// unifiedX[i_unified] < x[0]  -> x[i_unified] und y=0 einfügen (auffüllen)
			// unifiedX[i_unified] < xLast -> x[i_unified] und y=0 einfügen (auffüllen)
			double xUni = unifiedX[i_unified];
			if (xUni < xk[0] || xUni > xLast) {
				newY.append(0);
				++i_unified;
				continue;
			}

			// Fall 1: x-Werte stimmen überein (gleiche Stützstelle)
			if (xk[i] == xUni) {
				newY.append(yk[i]);
				++i;
				++i_unified;
				continue;
			}

			// Fall 2: x-Wert fehlt im Interval: Interpolation notwendig
			double y1 = yk[i];
			double y0 = yk[i-1];
			double dx = xk[i] - xk[i-1];
			// y = (x - x0)/(x1-x0)(y1-y0) + y0
			double yinterpol = (xUni - xk[i-1])/dx*(y1-y0) + y0;
			newY.append(yinterpol);
			++i_unified; // nur den Zähler für den gemeinsamen X-Vector erhöhen
		}
		yk.swap(newY);
	}
}


void computeStackedLinesWithNegative(
	const QVector<QVector<double> > & y,
	QVector<QVector<double> > & yPos,
	QVector<QVector<double> > & yNeg)
{
	// y[0..n-1] sind die Rohdaten der Datenreihen.
	int numSeries = y.count();
	int numPoints = y[0].count();

	QVector<double> posAccum(numPoints, 0.0);
	QVector<double> negAccum(numPoints, 0.0);

	QVector<QVector<double> > result;
	for (int j = 0; j < numSeries; ++j) {
		QVector<double> lower(numPoints), upper(numPoints);
		for (int i = 0; i < numPoints; ++i) {
			double v = y[j][i];
			// Positive Werte in posAccum akkumulieren, negative in negAccum
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


bool checkMonotonic(const QVector<double> & x) {
	bool failed = false;
	for (int i=1; i<x.count(); ++i) {
		if (x[i-1] >= x[i]) {
			qDebug() << "not monotonic at i=" << i;
			failed = true;
		}
	}
	return !failed;
}

void fullAlgorithm(QwtPlot * plot) {
#define VAR 4
	// Originale Daten
	QVector<double> x_initial{1,   2,   5,  6,  10};
	QVector<QVector<double> >  y;
	y.append( QVector<double>{2,   1,  -3,  4,   2} ); // rot
	y.append( QVector<double>{6,   5,   3,  4,  -5} ); // grün
	y.append( QVector<double>{5,  -6,  -3, -2,   1} ); // blau
	QVector<QColor> cols = {
		QColor(0xb00000), // #a00000
		QColor(0x20b000), // #208000
		QColor(0x2000b0)  // #200080
	};

#if 1
	// lade Testdaten aus der Datei
	QFile f("testdata.tsv");
	if (!f.open(QFile::ReadOnly)) {
		qCritical() << "Missing file 'testdata.tsv'";
		exit(1);
	}
	QTextStream strm(&f);
	QStringList header = strm.readLine().split('\t');
	y = QVector<QVector<double> >(header.count()-1);
	x_initial.clear();
	QString l;
	QString lLast;
	l = strm.readLine();
	QDateTime year;
	while (!strm.atEnd()) {
		if (l.isEmpty())
			continue;
		QTextStream st(&l);
		QString t,t2;
		st >> t >> t2;
		QDate d = QDate::fromString(t, "yyyy-MM-dd");
		QTime tim = QTime::fromString(t2, "hh:mm:ss");
		QDateTime dt(d,tim,QTimeZone::utc());
		// QDateTime dt = QDateTime::fromString(t + " " + t2, "yyyy-MM-dd hh:mm:ss", QTimeZone::utc());
		if (!year.isValid())
			year = dt;
		double xval = year.secsTo(dt)/(24*3600.0);
		if (!x_initial.empty() && x_initial.last() >= xval) {
			qDebug() << "Not monotinic, skipped:" << x_initial.last() << xval;
		}
		else {
			x_initial.append(xval);
			for (int i=0; i<y.count(); ++i) {
				double yval;
				st >> yval;
				y[i].append(yval);
			}
		}
		lLast = l;
		l = strm.readLine();
	}
	cols = {
		"#8C564B",
		"#BCBD22",
		"#E377C2",
		"#1F77B4",
		"#D62728",
		"#FF7F0E",
		"#2CA02C",
		"#7F7F7F",
		"#17BECF"
	};
#endif

	// Achenskalierung
	plot->resize(1000,600);
	plot->setAxisScale(QwtPlot::xBottom, 0, 365);

#if VAR==1
	QwtText t("Originale Datenreihen");
	// plot->setAxisScale(QwtPlot::yLeft, -8, 8);

	for (int i=0; i<y.count(); ++i)
		addCurve(plot, x_initial, y[i], cols[i]);
#elif VAR==2
	// step 1 : compute individual x vectors by interpolating y values
	QwtText t("Datenreihen mit Null-Durchgangspunkten (interpoliert)");
	// plot->setAxisScale(QwtPlot::yLeft, -8, 8);

	// jede Datenreihe braucht einen eigenen X-Vector
	QVector<QVector<double> >  x(y.count());
	for (int i=0; i<y.count(); ++i) {
		x[i] = x_initial;
		addZeroCrossings(x[i], y[i]); // in-place operation
	}
	for (int i=0; i<y.count(); ++i)
		addCurve(plot, x[i], y[i], cols[i]);
#elif VAR==3
	QwtText t("Datenreihen mit vereinheitlichtem X-Raster (interpoliert)");
	// plot->setAxisScale(QwtPlot::yLeft, -8, 8);

	if (!checkMonotonic(x_initial)) {
		qDebug() << "data not monotonic";
	}

	// jede Datenreihe braucht einen eigenen X-Vector
	QVector<QVector<double> >  x(y.count());
	for (int i=0; i<y.count(); ++i) {
		x[i] = x_initial;
		addZeroCrossings(x[i], y[i]); // in-place operation
	}
	for (int i=0; i<y.size(); ++i) {
		if (!checkMonotonic(x[i])) {
			qDebug() << "series " << i;
		}
	}
	// step 2 : merge x-coordinates and interpolate y values
	QVector<double> unifiedX;
	mergeCoordinates(x, y, unifiedX); // inplace operation
	for (int i=0; i<y.size(); ++i) {
		qDebug() << *std::max_element(y[i].begin(), y[i].end());
	}
	double lastX = unifiedX.last();

	for (int i=0; i<y.count(); ++i) {
		x[i] = unifiedX;
		addCurve(plot, x[i], y[i], cols[i]);
	}

#else
	QwtText t("Positiv und negative akkumulierte Kurven");
	// plot->setAxisScale(QwtPlot::yLeft, -10, 18);

	QVector<QVector<double> >  x(y.count());
	for (int i=0; i<y.count(); ++i) {
		x[i] = x_initial;
		addZeroCrossings(x[i], y[i]); // in-place operation
	}
	QVector<double> unifiedX;
	mergeCoordinates(x, y, unifiedX); // inplace operation

	// clear all but last
	for (unsigned int i=0;i<unifiedX.count(); ++i) {
		// y[0][i] = 0;
		// y[1][i] = 0;
		// y[2][i] = 0;
		// y[3][i] = 0;
		// y[4][i] = 0;
		// y[5][i] = 0;
		// y[6][i] = 0;
		// y[7][i] = 0;
		// y[8][i] = 0;
	}

	// Datenreihen addieren
	QVector<QVector<double> > yPos;
	QVector<QVector<double> > yNeg;
	computeStackedLinesWithNegative(y, yPos, yNeg);

#if 0
	// Farbe für die unterste Intervallgrenze einfügen
	cols.prepend(Qt::black);
	// Untere Intervallgrenze hinzufügen
	QVector<double> nullVector(unifiedX.count(), 0.0);
	yPos.prepend(nullVector);
	yNeg.prepend(nullVector);
	// die positiven Anteile als QwtPlotIntervalCurve hinzufügen
	addIntervalCurve(plot, unifiedX, yPos, cols);
	// und die negativen Anteile
	addIntervalCurve(plot, unifiedX, yNeg, cols);
#else
	t = QwtText("Performancevergleich, 10000 Datenpunkte nach Algorithmus");
	// Positive Kurven hinzufügen
	for (int j=yPos.count()-1;j>=0; --j) {
		QwtPlotCurve *curve = new QwtPlotCurve();
		curve->setPen(cols[j].darker(150), 0);
		curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
		curve->setSamples(unifiedX, yPos[j]);
		curve->setBrush(cols[j].lighter(120));
		curve->setZ(y.count()-j); // Zeichenreihenfolge über z-Wert setzen
		curve->attach(plot);
	}
	// Negative Kurven hinzufügen
	for (int j=yNeg.count()-1;j>=0; --j) {
		QwtPlotCurve *curve = new QwtPlotCurve();
		curve->setPen(cols[j].darker(150), 0);
		curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
		curve->setSamples(unifiedX, yNeg[j]);
		curve->setBrush(cols[j].lighter(120));
		curve->setZ(y.count()-j); // Zeichenreihenfolge über z-Wert setzen
		curve->attach(plot);
	}
#endif

#endif

	QFont titleFont(qApp->font());
	titleFont.setPointSize(10);
	titleFont.setBold(true);
	t.setFont(titleFont);
	plot->setTitle(t);
}


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	BenchmarkedPlot plot;
	plot.setWindowFlags(Qt::FramelessWindowHint);

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


