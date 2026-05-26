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


class ScaleDraw : public QwtScaleDraw {
public:
	ScaleDraw(const QStringList& labels ) : m_labels( labels ) {
		enableComponent( QwtScaleDraw::Ticks, false );
		enableComponent( QwtScaleDraw::Backbone, false );
		setLabelAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
	}

	virtual QwtText label( double value ) const QWT_OVERRIDE {
		const int index = qRound( value );
		if ( index >= 0 && index < m_labels.size()
#if 1
			 && qAbs(index-value) < 1e-6
#endif
		   )
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
	QVector<double> y{10,20,15,14,18,12};
	QwtPlotBarChart * curve = new QwtPlotBarChart;
	curve->setSamples(y);
	curve->attach(&plot); // Plot takes ownership

	// Zoomer hinzufügen
	// Achtung: NICHT QwtPlot selbst als 3 Argument übergeben, sonder das canvas()
	QwtPlotZoomer * zoomer = new QwtPlotZoomer(QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());  // plot takes ownership
	zoomer->setTrackerMode( QwtPlotPicker::AlwaysOn ); // Kurvenvwerte unterm Cursor anzeigen

	int plotType = 0;
	if (argc > 1)
		plotType = std::atoi(argv[1]);
	switch (plotType) {
		case 0 :
		break; // nothing to do

		case 1 :
			curve->setBaseline(15);
		break;

		case 2 :
			curve->setMargin(20);
			curve->setSpacing(40);
		break;

		case 3:
			curve->setLayoutPolicy(QwtPlotAbstractBarChart::AutoAdjustSamples);
		break;

		case 4:
			curve->setLayoutPolicy(QwtPlotAbstractBarChart::AutoAdjustSamples);
			curve->setLayoutHint(10);
		break;

		case 5:
			curve->setLayoutPolicy(QwtPlotBarChart::ScaleSamplesToAxes);
			curve->setLayoutHint(0.5);
		break;

		case 6:
			curve->setLayoutPolicy(QwtPlotBarChart::ScaleSampleToCanvas);
			curve->setLayoutHint(0.1); // bar width 10% of canvas width
		break;

		case 7: {
			QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
			symbol->setLineWidth( 2 );
			symbol->setFrameStyle( QwtColumnSymbol::Raised );
			symbol->setPalette( QPalette( QColor(0xE35811) ) );
			curve->setSymbol( symbol );
		} break;

		case 8: {
			QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
			symbol->setFrameStyle(QwtColumnSymbol::Plain);
			symbol->setLineWidth(1);
			QPalette palette(QColor(0xc1e311));
			palette.setBrush(QPalette::Dark, Qt::black); // black frame
			symbol->setPalette(palette);
			curve->setSymbol( symbol );
		} break;

		case 9: {
			QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
			symbol->setFrameStyle(QwtColumnSymbol::Plain);
			symbol->setLineWidth(1);
			QPalette palette(QColor(0x4B8FD7));
			palette.setBrush(QPalette::Dark, Qt::black); // black frame
			symbol->setPalette(palette);
			curve->setSymbol( symbol );

			curve->setLayoutPolicy(QwtPlotBarChart::ScaleSamplesToAxes);
			curve->setLayoutHint(1);

			curve->setMargin(20); // margin left/right of bars
			plot.plotLayout()->setCanvasMargin( 10 ); // canvas margin all around

			// x-Achsenticks verstecken
			QwtScaleDraw* scaleDraw1 = plot.axisScaleDraw( QwtPlot::xBottom );
			scaleDraw1->enableComponent( QwtScaleDraw::Backbone, false );
			scaleDraw1->enableComponent( QwtScaleDraw::Ticks, false );

			// do not fix y-axis at 0 and left edge of canvas
			plot.plotLayout()->setAlignCanvasToScale( QwtPlot::yLeft, false );
			plot.updateCanvasMargins();
		} break;

		case 10: {
			QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
			symbol->setFrameStyle(QwtColumnSymbol::Plain);
			symbol->setLineWidth(1);
			QPalette palette(QColor(0x4B8FD7));
			palette.setBrush(QPalette::Dark, Qt::black); // black frame
			symbol->setPalette(palette);
			curve->setSymbol( symbol );

			curve->setLayoutPolicy(QwtPlotBarChart::ScaleSamplesToAxes);
			curve->setLayoutHint(1);

			curve->setMargin(0);
			plot.plotLayout()->setCanvasMargin(0);

			// hide x-axis ticks
			QwtScaleDraw* scaleDraw1 = plot.axisScaleDraw( QwtPlot::xBottom );
			scaleDraw1->enableComponent( QwtScaleDraw::Backbone, false );
			scaleDraw1->enableComponent( QwtScaleDraw::Ticks, false );

			// do not fix y-axis at 0 and left edge of canvas
			plot.plotLayout()->setAlignCanvasToScale( QwtPlot::yLeft, true );
			plot.updateCanvasMargins();
		} break;

		case 11: {
			QwtColumnSymbol* symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
			symbol->setFrameStyle(QwtColumnSymbol::Plain);
			symbol->setLineWidth(1);
			QPalette palette(QColor(0xFFF194));
			palette.setBrush(QPalette::Dark, Qt::black); // black frame
			symbol->setPalette(palette);
			curve->setSymbol( symbol );

			curve->setLayoutPolicy(QwtPlotBarChart::ScaleSamplesToAxes);
			curve->setLayoutHint(0.8);

			curve->setMargin(10); // margin left/right of bars
			plot.plotLayout()->setCanvasMargin( 10 ); // canvas margin all around

			plot.plotLayout()->setAlignCanvasToScale( QwtPlot::yLeft, false );
			plot.updateCanvasMargins();

			QwtScaleDraw * scaleDraw = new ScaleDraw(QStringList() << "Dresden" << "Berlin" << "Leipzig" << "Hamburg" << "Wolgast" << "Saalfeld");
			QFont f;
			f.setPointSize(7);
			f.setBold(true);
			plot.setAxisFont(QwtPlot::xBottom, f);
			plot.setAxisScaleDraw(QwtPlot::xBottom, scaleDraw);
		} break;
	}

	plot.show();
	return a.exec();
}
