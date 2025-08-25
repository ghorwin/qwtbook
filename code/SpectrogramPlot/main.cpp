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
#include <QwtPlotSpectroCurve>
#include <QwtLinearColorMap>
#include <QwtPlotSpectrogram>
#include <QwtScaleWidget>
#include <QwtPlotLayout>

class SpectrogramData : public QwtRasterData {
public:
	SpectrogramData() {
		// kleine Optimierung, spart extra Aufwand beim Prüfen auf Lücken
		setAttribute(QwtRasterData::WithoutGaps, false);

		// Wertebereiche definieren
		m_intervals[Qt::XAxis] = QwtInterval(-1.5, 3);
		m_intervals[Qt::YAxis] = QwtInterval(-1.5, 1.5);
		m_intervals[Qt::ZAxis] = QwtInterval(0.0, 8.0);
	}

	// Diese Funktion liefert die Wertebereiche zurück und muss implementiert werden
	virtual QwtInterval interval(Qt::Axis axis) const override {
		if (axis >= 0 && axis <= 2)
			return m_intervals[axis];

		return QwtInterval();
	}

	// Dies ist die eigentliche Berechnungsfunktion, welche für eine x,y-Plotkoordinate den
	// entsprechenden z-Wert zurückliefert.
	virtual double value(double x, double y) const override {
		if ((x > 0.) &&	(x < 0.5) && (y > 0.) && (y < 0.5))
			return qQNaN();
		double z = (x - 1) * (x - 1) + (y - 2) * (y - 1);
		return z;
	}

private:
	QwtInterval m_intervals[3];
};


class LinearColorMap : public QwtLinearColorMap {
public:
	LinearColorMap( int formatType )
		: QwtLinearColorMap( Qt::darkCyan, Qt::red )
	{
		setFormat( ( QwtColorMap::Format ) formatType );

		addColorStop( 0.1, Qt::cyan );
		addColorStop( 0.6, Qt::green );
		addColorStop( 0.95, Qt::yellow );
	}
};


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	plot.setWindowFlags(Qt::FramelessWindowHint);
	plot.resize(400,300);

	QFont f;
	f.setPointSize(4);
	plot.setFont(f);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8, 8, 8, 8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground(Qt::white);

	QwtPlotSpectrogram * spectro = new QwtPlotSpectrogram("Some spectrogram");
	spectro->setRenderThreadCount( 0 ); // Parallisierung je nach Systemstand verwenden
	spectro->setCachePolicy( QwtPlotRasterItem::PaintCache ); // Bild nur neu rendern, wenn notwendig
	spectro->attach(&plot);

	// Datenhalteobjekt setzen, dieses liefert für jeden Bildpunkt einen z-Wert zurück.
	spectro->setData( new SpectrogramData() );

#if 0
	QList< double > contourLevels;
	for ( double level = 0.5; level < 10.0; level += 1.0 )
		contourLevels += level;
	spectro->setContourLevels( contourLevels );


	const QwtInterval zInterval = spectro->data()->interval( Qt::ZAxis );

	plot.setAxisScale( QwtAxis::YRight, zInterval.minValue(), zInterval.maxValue() );
	plot.setAxisVisible( QwtAxis::YRight );

	plot.plotLayout()->setAlignCanvasToScales( true );

	spectro->setColorMap( new LinearColorMap( QwtColorMap::RGB ) );

	// plot.axisWidget(QwtPlot::xBottom)->setFont(f);
	plot.setAxisFont(QwtPlot::xBottom, f);

	plot.axisWidget(QwtPlot::yLeft)->setFont(f);
	plot.axisWidget(QwtPlot::yRight)->setFont(f);

	// A color bar on the right axis
	QwtScaleWidget* rightAxis = plot.axisWidget( QwtAxis::YRight );
	QwtText title("Intensity");
	// title.setPaintAttribute(QwtText::PaintUsingTextFont, false);
	// f.setBold(true);
	// title.setFont(f);
	rightAxis->setTitle( "Intensity" );
	rightAxis->setTitle( title );
	rightAxis->setTitle( QwtText("Intensity"));
	rightAxis->setColorBarEnabled( true );
	rightAxis->setColorMap( zInterval, new LinearColorMap( QwtColorMap::RGB ) );

	// plot.setFont(f);
#endif
	plot.show();
	return a.exec();
}
