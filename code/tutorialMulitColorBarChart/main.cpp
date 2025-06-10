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
	MultiColorBarChart() {
		setLayoutPolicy(QwtPlotBarChart::ScaleSamplesToAxes);
		setLayoutHint(0.8);
		// Legende zeigt individuelle Balkentitel
		setLegendMode( QwtPlotBarChart::LegendBarTitles );
		setLegendIconSize( QSize( 10, 14 ) );
	}

	// we want to have individual colors for each bar
	virtual QwtColumnSymbol* specialSymbol(
		int sampleIndex, const QPointF&) const QWT_OVERRIDE
	{
		// generate symbol with color for each bar
		QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
		symbol->setLineWidth( 2 );
		symbol->setFrameStyle( QwtColumnSymbol::Raised );

		QColor c( Qt::white );
		if ( sampleIndex >= 0 && sampleIndex < m_colors.size() )
			c = m_colors[ sampleIndex ];
		symbol->setPalette( c );

		return symbol;
	}

	virtual QwtText barTitle( int sampleIndex ) const QWT_OVERRIDE {
		if ( sampleIndex >= 0 && sampleIndex < m_titles.size() )
			return m_titles[ sampleIndex ];
		return QwtText();
	}

	QStringList	  m_titles;
	QList<QColor> m_colors;
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

	// Daten zum Darstellen
	const struct
	{
		const double popularity;
		const char* name;
		QColor color;
	}
	cityPop[] =
	{
		{ 20, "Dresden", QColor(0xf19624) },
		{ 10, "Berlin", QColor(0x0081a6) },
		{ 15, "Leipzig", QColor(0xdceb54) },
		{ 14, "Hamburg", QColor(0x00a668) },
		{ 18, "Wolgast", QColor(0xf72415) },
		{ 12, "Saalfeld", QColor(0x912415) }
	};
	MultiColorBarChart * curve = new MultiColorBarChart;
	QVector<double>		samples;
	for (auto barData : cityPop) {
		samples += barData.popularity;
		curve->m_titles += barData.name;
		curve->m_colors += barData.color;
	}
	curve->setSamples(samples);
	curve->attach(&plot); // plot takes ownership

	// Legende anzeigen
	QwtLegend * legend = new QwtLegend();
	QFont legendFont;
	legendFont.setPointSize(7);
	legend->setFont(legendFont);
	plot.insertLegend(legend, QwtPlot::RightLegend); // plot takes ownership

	// Titel hinzufügen
	QwtText text("Multi-colored QwtPlotBarChart");
	QFont titleFont;
	titleFont.setBold(true);
	titleFont.setPointSize(10);
	text.setFont(titleFont);
	plot.setTitle(text);

	curve->setMargin(10); // margin left/right of bars
	plot.setAxisVisible(QwtPlot::xBottom, false);
	plot.plotLayout()->setCanvasMargin( 0 ); // canvas margin all around
	plot.plotLayout()->setAlignCanvasToScale( QwtPlot::yLeft, false); // do not fix y-axis at 0 and left edge of canvas
	plot.updateCanvasMargins();

	// Zoomer hinzufügen
	// Achtung: NICHT QwtPlot selbst als 3 Argument übergeben, sonder das canvas()
	QwtPlotZoomer * zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen

	// Panner hinzufügen, wie auch beim PlotZoomer muss das Canvas-Objekt als Argument übergeben werden
	QwtPlotPanner * panner = new QwtPlotPanner(plot.canvas());  // plot takes ownership
	panner->setMouseButton(Qt::MidButton); // Mittlere Maustaste verschiebt

	plot.show();
	return a.exec();
}
