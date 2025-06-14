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

#define ORIGINAL_HEADERS
#ifndef ORIGINAL_HEADERS

#include <QwtPlot>
#include <QwtPlotCurve>
#include <QwtLegend>
#include <QwtText>
#include <QwtPlotGrid>
#include <QwtLogScaleEngine>
#include <QwtPlotMarker>
#include <QwtPlotZoomer>
#include <QwtPlotPanner>
#include <QwtPlotLayout>
#include <QwtScaleDraw>
#include <QwtCurveFitter>
#else

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_engine.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_layout.h>
#include <qwt_curve_fitter.h>
#include <qwt_polar_fitter.h>
#include <qwt_weeding_curve_fitter.h>
#include <qwt_spline_curve_fitter.h>
#include <qwt_spline_pleasing.h>
#endif

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	plot.resize(600,400);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground( Qt::white );

	// Daten zum Darstellen einlesen
	QVector<double> x, y;
	QFile f("spektrum.tsv");  // Datei enthält 2 Spalten
	f.open(QFile::ReadOnly);
	QTextStream strm(&f);
	strm.readLine(); // Kopfzeile überspringen
	while (!strm.atEnd()) {
		double xval, yval;
		if ( (strm >> xval >> yval).status() == QTextStream::Ok) {
			x.append(xval);
			y.append(std::max(1e-4, yval)); // prevent 0 or negative values as we want to plot log values
		}
	}

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setPen(QColor(180,40,20), 1);
	// curve->setBrush(QColor(180,40,20));
	curve->setTitle("Gamma-Spektrum");
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->attach(&plot); // Plot takes ownership

	// Legende anzeigen
	QwtLegend * legend = new QwtLegend();
	QFont legendFont;
	legendFont.setPointSize(8);
	legend->setFont(legendFont);
	plot.insertLegend( legend , QwtPlot::BottomLegend); // plot takes ownership

	// Titel hinzufügen
	QwtText text("Gamma-Spektrum");
	QFont titleFont;
	titleFont.setBold(true);
	titleFont.setPointSize(10);
	text.setFont(titleFont);
	plot.setTitle(text);

	// Haupt- und Nebengitter anzeigen
	QwtPlotGrid *grid = new QwtPlotGrid();
	QPen gridPen(Qt::gray);
	gridPen.setStyle(Qt::DashLine);
	grid->setMajorPen(gridPen);
	// Minor grid
	grid->enableYMin( true );
	gridPen.setColor(Qt::lightGray);
	gridPen.setStyle(Qt::DotLine);
	grid->setMinorPen(gridPen);
	grid->attach( &plot ); // plot takes ownership


	// Achsen formatieren
	QFont axisFont;
	axisFont.setPointSize(8);
	axisFont.setBold(true);
	QFont axisLabelFont;
	axisLabelFont.setPointSize(8);
	// X-Achse
	QwtText axisTitle("Kanal");
	axisTitle.setFont(axisFont);
	// Titel Text und Font setzen
	plot.setAxisTitle(QwtPlot::xBottom, axisTitle);
	// Font für Achsenzahlen setzen
	plot.setAxisFont(QwtPlot::xBottom, axisLabelFont);
	// Y-Achse
	axisTitle.setText("Ereignisse");
	plot.setAxisTitle(QwtPlot::yLeft, axisTitle);
	plot.setAxisFont(QwtPlot::yLeft, axisLabelFont);

	// Logarithmische Y-Achse
	QwtLogScaleEngine * logScale = new QwtLogScaleEngine();
	plot.setAxisScaleEngine(QwtPlot::yLeft, logScale); // plot takes ownership
	// manuelle Achsenlimits festlegen, da autoscale bei log-Achsen nicht sinnvoll funktioniert
	plot.setAxisScale(QwtPlot::yLeft, 1e-3,1000);

	// Vertikale, gestrichelte Plot-Markierung einfügen
	QwtText label("207,50 keV");
	label.setFont(axisLabelFont);

	QwtPlotMarker * marker = new QwtPlotMarker(label);
	// marker->setLabel(label);
	marker->setLabelOrientation(Qt::Vertical); // Vertikale Linie
	marker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom); // Label unten und rechts von der Linie
	marker->setValue(36, 0); // bei vertikalen Linien muss die x-Koordinate festgelegt werden
	QPen markerPen(QColor(40,60,255));
	markerPen.setStyle(Qt::SolidLine);
	marker->setLinePen(markerPen);
	marker->setLineStyle(QwtPlotMarker::VLine);
	marker->setLabel(label);
	marker->attach(&plot); // plot takes ownership

	QwtSplineCurveFitter * splineFitter = new QwtSplineCurveFitter;
	QwtSplinePleasing * spline = new QwtSplinePleasing();
	splineFitter->setSpline(spline);
	curve->setCurveFitter(splineFitter);
	// Zoomer hinzufügen
	// Achtung: NICHT QwtPlot selbst als 3 Argument übergeben, sonder das canvas()
	QwtPlotZoomer * zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen

	// Panner hinzufügen, wie auch beim PlotZoomer muss das Canvas-Objekt als Argument übergeben werden
	QwtPlotPanner * panner = new QwtPlotPanner(plot.canvas());  // plot takes ownership
	panner->setMouseButton(Qt::MiddleButton); // Mittlere Maustaste verschiebt
	plot.show();




	return a.exec();
}
