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
#include <QDebug>
#include <QTextStream>

//#define ORIGINAL_HEADERS
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
#include <QwtPlotBarChart>
#include <QwtColumnSymbol>
#include <QwtScaleDraw>
#include <QwtPlotLayout>

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

#endif


class MultiColorBarChart : public QwtPlotBarChart {
public:
	MultiColorBarChart() : QwtPlotBarChart( "Popular Cities" ) {
		setLegendMode( QwtPlotBarChart::LegendBarTitles );
		setLegendIconSize( QSize( 10, 14 ) );
		setLayoutPolicy( AutoAdjustSamples );
		setLayoutHint( 4.0 ); // minimum width for a single bar

		setSpacing( 10 ); // spacing between bars
	}

	void addBar( double value, const QString& title, const QColor& color ) {
		m_samples += value;
		m_titles += title;
		m_colors += color;
		setSamples(m_samples); // this will trigger a redraw
	}

	virtual QwtColumnSymbol* specialSymbol(int index, const QPointF& ) const QWT_OVERRIDE {
		// we want to have individual colors for each bar

		QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
		symbol->setLineWidth( 2 );
		symbol->setFrameStyle( QwtColumnSymbol::Raised );

		QColor c( Qt::white );
		if ( index >= 0 && index < m_colors.size() )
			c = m_colors[ index ];

		symbol->setPalette( c );

		return symbol;
	}

	virtual QwtText barTitle( int sampleIndex ) const QWT_OVERRIDE {
		if ( sampleIndex >= 0 && sampleIndex < m_titles.size() )
			return m_titles[ sampleIndex ];
		return QwtText();
	}

	QVector<double>		m_samples;
	QVector<QString>	m_titles;
	QVector<QColor>		m_colors;
};


class ScaleDraw : public QwtScaleDraw {
public:
	ScaleDraw(const QStringList& labels ) : m_labels( labels ) {
		enableComponent( QwtScaleDraw::Ticks, false );
		enableComponent( QwtScaleDraw::Backbone, false );
		setLabelAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
	}

	virtual QwtText label( double value ) const QWT_OVERRIDE {
		const int index = qRound( value );
		if ( index >= 0 && index < m_labels.size() && qAbs(index-value) < 1e-6)
			return m_labels[index];
		return QwtText();
	}

	QStringList m_labels;
};


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	plot.resize(400,300);
	plot.setWindowFlag(Qt::FramelessWindowHint, true);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground( Qt::white );

	// Daten zum Darstellen einlesen
	const struct
	{
		const double popularity;
		const char* name;
		QColor color;
	}
	cityPop[] =
	{
	{ 10, "Dresden", QColor(0xf19624) },
		{ 20, "Berlin", QColor(0x0081a6) },
		{ 15, "Leipzig", QColor(0xdceb54) },
		{ 14, "Hamburg", QColor(0x00a668) },
		{ 18, "Wolgast", QColor(0xf72415) },
		{ 12, "Saalfeld", QColor(0x912415) }
	};
	MultiColorBarChart * curve = new MultiColorBarChart;
	for (auto barData : cityPop)
		curve->addBar(barData.popularity, barData.name, barData.color);
//	curve->setLayoutPolicy(QwtPlotBarChart::AutoAdjustSamples);
//	curve->setLayoutHint(10);
	curve->setLayoutPolicy(QwtPlotBarChart::ScaleSamplesToAxes);
	curve->setLayoutHint(0.8);
//	curve->setLayoutPolicy(QwtPlotBarChart::ScaleSampleToCanvas);
//	curve->setLayoutHint(0.1); // bar width 10% of canvas width

//	curve->setSpacing(40);
//	curve->setBaseline(15);

	// QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
	// symbol->setFrameStyle(QwtColumnSymbol::Plain);
	// symbol->setLineWidth(1);
	// QPalette palette(QColor(0xfff194));
	// palette.setBrush(QPalette::Dark, Qt::black); // black frame
	// symbol->setPalette(palette);

	// curve->setSymbol( symbol );

	curve->attach(&plot); // Plot takes ownership

	// Legende anzeigen
	QwtLegend * legend = new QwtLegend();
	QFont legendFont;
	legendFont.setPointSize(7);
	legend->setFont(legendFont);
	plot.insertLegend( legend , QwtPlot::RightLegend); // plot takes ownership

	// Titel hinzufügen
	QwtText text("Multi-colored QwtPlotBarChart");
	QFont titleFont;
	titleFont.setBold(true);
	titleFont.setPointSize(10);
	text.setFont(titleFont);
	plot.setTitle(text);


//	QwtScaleDraw* scaleDraw1 = plot.axisScaleDraw( QwtPlot::xBottom );
//	scaleDraw1->enableComponent( QwtScaleDraw::Backbone, false );
//	scaleDraw1->enableComponent( QwtScaleDraw::Ticks, false );

	QwtScaleDraw * scaleDraw = new ScaleDraw(QStringList(curve->m_titles.constBegin(), curve->m_titles.constEnd()));
	QFont f;
	f.setPointSize(7);
	f.setBold(true);
	plot.setAxisFont(QwtPlot::xBottom, f);
	plot.setAxisScaleDraw(QwtPlot::xBottom, scaleDraw);

	curve->setMargin(10); // margin left/right of bars
	plot.plotLayout()->setCanvasMargin( 0 ); // canvas margin all around
	plot.plotLayout()->setAlignCanvasToScale( QwtPlot::yLeft, false); // do not fix y-axis at 0 and left edge of canvas
	plot.updateCanvasMargins();



//	// Haupt- und Nebengitter anzeigen
//	QwtPlotGrid *grid = new QwtPlotGrid();
//	QPen gridPen(Qt::gray);
//	gridPen.setStyle(Qt::DashLine);
//	grid->setMajorPen(gridPen);
//	// Minor grid
//	grid->enableYMin( true );
//	gridPen.setColor(Qt::lightGray);
//	gridPen.setStyle(Qt::DotLine);
//	grid->setMinorPen(gridPen);
//	grid->attach( &plot ); // plot takes ownership


//	// Achsen formatieren
//	QFont axisFont;
//	axisFont.setPointSize(8);
//	axisFont.setBold(true);
//	QFont axisLabelFont;
//	axisLabelFont.setPointSize(8);
//	// X-Achse
//	QwtText axisTitle("Kanal");
//	axisTitle.setFont(axisFont);
//	// Titel Text und Font setzen
//	plot.setAxisTitle(QwtPlot::xBottom, axisTitle);
//	// Font für Achsenzahlen setzen
//	plot.setAxisFont(QwtPlot::xBottom, axisLabelFont);
//	// Y-Achse
//	axisTitle.setText("Ereignisse");
//	plot.setAxisTitle(QwtPlot::yLeft, axisTitle);
//	plot.setAxisFont(QwtPlot::yLeft, axisLabelFont);

//	// Logarithmische Y-Achse
//	QwtLogScaleEngine * logScale = new QwtLogScaleEngine();
//	plot.setAxisScaleEngine(QwtPlot::yLeft, logScale); // plot takes ownership
//	// manuelle Achsenlimits festlegen, da autoscale bei log-Achsen nicht sinnvoll funktioniert
//	plot.setAxisScale(QwtPlot::yLeft, 1e-3,1000);

//	// Vertikale, gestrichelte Plot-Markierung einfügen
//	QwtPlotMarker * marker = new QwtPlotMarker("207,50 keV");
//	marker->setLabelOrientation(Qt::Vertical); // Vertikale Linie
//	marker->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom); // Label unten und rechts von der Linie
//	marker->setValue(36, 0); // bei vertikalen Linien muss die x-Koordinate festgelegt werden
//	QPen markerPen(QColor(40,60,255));
//	markerPen.setStyle(Qt::SolidLine);
//	marker->setLinePen(markerPen);
//	marker->setLineStyle(QwtPlotMarker::VLine);
//	marker->setLabel(QwtText("207,50 keV"));
//	marker->attach(&plot); // plot takes ownership

	// Zoomer hinzufügen
	// Achtung: NICHT QwtPlot selbst als 3 Argument übergeben, sonder das canvas()
	QwtPlotZoomer * zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen

//	// Panner hinzufügen, wie auch beim PlotZoomer muss das Canvas-Objekt als Argument übergeben werden
//	QwtPlotPanner * panner = new QwtPlotPanner(plot.canvas());  // plot takes ownership
//	panner->setMouseButton(Qt::MidButton); // Mittlere Maustaste verschiebt

	plot.show();
	return a.exec();
}
