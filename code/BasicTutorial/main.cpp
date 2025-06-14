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
#include <QPainter>

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
#include <QwtPlotRenderer>
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
#include <qwt_plot_renderer.h>
#include <qwt_plot_layout.h>

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
		strm >> xval >> yval;
		x.append(xval);
		y.append(yval);
	}

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setPen(QColor(180,40,20), 0);
	curve->setTitle("Gamma-Spektrum");
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
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
	QwtPlotMarker * marker = new QwtPlotMarker;
	marker->setLabelOrientation(Qt::Vertical); // Vertikale Linie
	marker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom); // Label unten und rechts von der Linie
	marker->setValue(36, 0); // bei vertikalen Linien muss die x-Koordinate festgelegt werden
	QPen markerPen(QColor(40,60,255));
	markerPen.setStyle(Qt::SolidLine);
	marker->setLinePen(markerPen);
	marker->setLineStyle(QwtPlotMarker::VLine);
	QwtText markerLabel("207,50 keV");
	QFont markerFont;
	markerFont.setPointSize(8);
	markerLabel.setFont(markerFont);
	marker->setLabel(markerLabel);
	marker->attach(&plot); // plot takes ownership

	// Zoomer hinzufügen
	// Achtung: NICHT QwtPlot selbst als 3 Argument übergeben, sonder das canvas()
	QwtPlotZoomer * zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen

	// Panner hinzufügen, wie auch beim PlotZoomer muss das Canvas-Objekt als Argument übergeben werden
	QwtPlotPanner * panner = new QwtPlotPanner(plot.canvas());  // plot takes ownership
	panner->setMouseButton(Qt::MidButton); // Mittlere Maustaste verschiebt

	plot.show();

#if 0
	// Render-Objekt erstellen
	QwtPlotRenderer renderer;
	// Zielgröße festlegen
	QRect imageRect( 0.0, 0.0, 600, 400 );
	// Bildobjekt in der entsprechenden Größe erstellen...
	QImage image( imageRect.size(), QImage::Format_ARGB32 );
	// und mit weißem Hintergrund füllen
	image.fill(Qt::white);
	renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
	renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
	renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
	renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );

	plot.plotLayout()->setAlignCanvasToScale( QwtPlot::yLeft, true );
	plot.plotLayout()->setAlignCanvasToScale( QwtPlot::xBottom, true );
	plot.plotLayout()->setAlignCanvasToScale( QwtPlot::yRight, true );
	plot.plotLayout()->setAlignCanvasToScale( QwtPlot::xTop, true );

	// Das Diagramm in das QImage zeichnen
	QPainter painter( &image );
	renderer.render( &plot, &painter, imageRect );
	painter.end();

	// QImage zurück in Pixmap konvertieren
	QPixmap plotPixmap( QPixmap::fromImage(image) );
	plotPixmap.save("diagram.png");
#endif
	return a.exec();
}
