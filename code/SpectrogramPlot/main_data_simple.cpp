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

class SpectrogramDataTable : public QwtRasterData {
public:

	// Stützstellen x-Raster (Anzahl Zellen/Elemente entlang x-Achse + 1)
	std::vector<double> m_xvalues;
	// Stützstellen y-Raster (Anzahl Zellen/Elemente entlang y-Achse + 1)
	std::vector<double> m_yvalues;
	// Werte in den Zellen, m_zvalues[yIndex][xIndex]
	std::vector<std::vector<double> > m_zvalues;

	void updateIntervals() {
		double minVal = std::numeric_limits<double>::max();
		double maxVal = -std::numeric_limits<double>::max();
		for (unsigned int j=0; j<m_zvalues.size(); ++j)
			for (unsigned int i=0; i<m_zvalues[j].size(); ++i) {
				double v = m_zvalues[j][i];
				minVal = std::min(minVal, v);
				maxVal = std::max(maxVal, v);
			}
		m_intervals[0].setInterval(m_xvalues.front(), m_xvalues.back());
		m_intervals[1].setInterval(m_yvalues.front(), m_yvalues.back());
		m_intervals[2].setInterval(minVal, maxVal);
	}

	// Diese Funktion liefert die Wertebereiche zurück und muss implementiert werden
	virtual QwtInterval interval(Qt::Axis axis) const override {
		if (axis >= 0 && axis <= 2)
			return m_intervals[axis];

		return QwtInterval();
	}


	// Diese Funktion sucht die zu den x,y-Koordinaten passende Zelle heraus und liefert den Wert zurück
	virtual double value(double x, double y) const override {
		// Außerhalb des Wertebereichs?
		if ((x < m_xvalues.front())  || (y < m_yvalues.front()) ||
			(x > m_xvalues.back()) || (y > m_yvalues.back()))
		{
			return qQNaN();
		}

		// Index mittels std::lower_bound() suchen
		std::vector<double>::const_iterator it = std::lower_bound(m_xvalues.begin(), m_xvalues.end(), x);
		unsigned int xIdx = it - m_xvalues.begin();
		it = std::lower_bound(m_yvalues.begin(), m_yvalues.end(), y);
		unsigned int yIdx = it - m_yvalues.begin();

		// Spezialbehandlung:
		//   x = xMin -> xIdx == 0
		//   x > xMin -> xIdx == xIdx - 1 (da im ersten Interval)
		//   x == xMax -> xIdx == xIdx - 1 (da im letzten Interval)

		if (xIdx > 0)
			--xIdx;

		if (yIdx > 0)
			--yIdx;

		Q_ASSERT(xIdx < m_xvalues.size()-1);
		Q_ASSERT(yIdx < m_yvalues.size()-1);

		return m_zvalues[yIdx][xIdx];
	}

private:
	QwtInterval m_intervals[3];
};


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	plot.setWindowFlags(Qt::FramelessWindowHint);
	plot.resize(400,300);

	QFont f;
	f.setPointSize(6);
	plot.setFont(f);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8, 8, 8, 8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground(Qt::white);

	QwtPlotSpectrogram * spectro = new QwtPlotSpectrogram("Some spectrogram");
	spectro->setRenderThreadCount( 1 ); // Parallisierung je nach Systemstand verwenden
	spectro->setCachePolicy( QwtPlotRasterItem::PaintCache ); // Bild nur neu rendern, wenn notwendig
	spectro->attach(&plot);

	// Datenhalteobjekt setzen, dieses liefert für jeden Bildpunkt einen z-Wert zurück.
	SpectrogramDataTable * data = new SpectrogramDataTable();
	// 4 x 5 Zellen/Elemente
	data->m_xvalues = {0,1,2,5,6};
	data->m_yvalues = {0,0.1,0.2,0.3,0.4,0.5};
	data->m_zvalues.push_back( { 1, 2, 3, 4} );
	data->m_zvalues.push_back( { 1, 2.2, 3.5, 4} );
	data->m_zvalues.push_back( { 1.4, 2.4, 3.7, 4.6} );
	data->m_zvalues.push_back( { 1.5, 2.5, 3.9, 5.1} );
	data->m_zvalues.push_back( { 1.5, 2.6, 4.2, 5.2} );
	data->updateIntervals();
	spectro->setData( data );

	QwtLinearColorMap * cm = new QwtLinearColorMap( QwtColorMap::RGB );
	cm->setColorInterval(QColor(0x000080), QColor(0x800000));
	cm->addColorStop(0.2, QColor(0x0080ff));
	cm->addColorStop(0.5, QColor(0x00ff00));
	cm->addColorStop(0.7, QColor(0xffff00));
	spectro->setColorMap( cm );


	plot.setAxisFont(QwtPlot::xBottom, f);

	plot.axisWidget(QwtPlot::yLeft)->setFont(f);
	plot.axisWidget(QwtPlot::yRight)->setFont(f);
	plot.plotLayout()->setAlignCanvasToScales( true );

	const QwtInterval xInterval = spectro->data()->interval( Qt::XAxis );
	const QwtInterval yInterval = spectro->data()->interval( Qt::YAxis );
	const QwtInterval zInterval = spectro->data()->interval( Qt::ZAxis );

	plot.setAxisScale( QwtAxis::XBottom, xInterval.minValue(), xInterval.maxValue() );
	plot.setAxisScale( QwtAxis::YLeft, yInterval.minValue(), yInterval.maxValue() );
	plot.setAxisScale( QwtAxis::YRight, zInterval.minValue(), zInterval.maxValue() );
	plot.setAxisVisible( QwtAxis::YRight );

		   // A color bar on the right axis
	// QwtScaleWidget* rightAxis = plot.axisWidget( QwtAxis::YRight );
	// QwtText title;
	// title.setText("Temperature");
	// f.setPointSize(8);
	// f.setBold(true);
	// title.setFont(f);
	// rightAxis->setTitle( title);
	// rightAxis->setColorBarEnabled( true );
	// rightAxis->setColorMap( zInterval, cm );
	// rightAxis->setLayoutFlag(QwtScaleWidget::TitleInverted, false);

	// title.setText("Coordinate");
	// plot.axisWidget(QwtAxis::YLeft)->setTitle(title);
	// plot.axisWidget(QwtAxis::XBottom)->setTitle(title);


	plot.show();
	return a.exec();
}
