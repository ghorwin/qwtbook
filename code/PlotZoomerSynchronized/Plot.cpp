#include "Plot.h"

#include <QFile>
#include <QTextStream>
#include <QPen>

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

Plot::Plot() {
	resize(600,400);

	// etwas Abstand zwischen Rand und Achsentiteln
	setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	setCanvasBackground( Qt::white );

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
	curve->attach(this); // Plot takes ownership

	// Legende anzeigen
	QwtLegend * legend = new QwtLegend();
	QFont legendFont;
	legendFont.setPointSize(8);
	legend->setFont(legendFont);
	insertLegend( legend , QwtPlot::BottomLegend); // plot takes ownership

	// Titel hinzufügen
	QwtText text("Gamma-Spektrum");
	QFont titleFont;
	titleFont.setBold(true);
	titleFont.setPointSize(10);
	text.setFont(titleFont);
	setTitle(text);

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
	grid->attach(this); // plot takes ownership


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
	setAxisTitle(QwtPlot::xBottom, axisTitle);
	// Font für Achsenzahlen setzen
	setAxisFont(QwtPlot::xBottom, axisLabelFont);
	// Y-Achse
	axisTitle.setText("Ereignisse");
	setAxisTitle(QwtPlot::yLeft, axisTitle);
	setAxisFont(QwtPlot::yLeft, axisLabelFont);

	// Logarithmische Y-Achse
	QwtLogScaleEngine * logScale = new QwtLogScaleEngine();
	setAxisScaleEngine(QwtPlot::yLeft, logScale); // plot takes ownership
	// manuelle Achsenlimits festlegen, da autoscale bei log-Achsen nicht sinnvoll funktioniert
	setAxisScale(QwtPlot::yLeft, 1e-3,1000);

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
	marker->attach(this); // plot takes ownership

	// Zoomer hinzufügen
	// Achtung: NICHT QwtPlot selbst als 3 Argument übergeben, sonder das canvas()
	zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen

	connect(zoomer, &QwtPlotZoomer::zoomed, this, &Plot::zoomed);

	// Panner hinzufügen, wie auch beim PlotZoomer muss das Canvas-Objekt als Argument übergeben werden
	QwtPlotPanner * panner = new QwtPlotPanner(canvas());  // plot takes ownership
	panner->setMouseButton(Qt::MiddleButton); // Mittlere Maustaste verschiebt
}


void Plot::onZoomed(const QRectF &rect) {
	// we only adjust the x-axis
	QRectF currentZoomRect = zoomer->zoomRect();
	currentZoomRect.setLeft(rect.left());
	currentZoomRect.setRight(rect.right());
	// disable signals
	blockSignals(true);
	zoomer->zoom(currentZoomRect);
	blockSignals(false);
}
