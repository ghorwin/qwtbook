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

#include <fstream>
#include <sstream>
#include <limits>

size_t explode(const std::string& str, std::vector<std::string>& tokens, char delim) {
	std::string tmp;
	tokens.clear();
	for (std::string::const_iterator it=str.begin(); it!=str.end(); ++it) {
		if (*it!=delim)
			tmp+=*it;
		else {
			tokens.push_back(tmp);
			tmp.clear();
		}
	}
	tokens.push_back(tmp);
	// if 'str' was empty, return also a vector with exactly one empty string
	if (tokens.empty())
		tokens.push_back("");
	return tokens.size();
}
// ---------------------------------------------------------------------------

class SpectrogramData : public QwtRasterData {
public:
	SpectrogramData() {
		// kleine Optimierung, spart extra Aufwand beim Prüfen auf Lücken
		setAttribute(QwtRasterData::WithoutGaps, false);

	}

	void read(const std::string & fname) {
		setlocale(LC_NUMERIC,"C");
		std::ifstream in(fname);
		std::string line;
		std::getline(in, line);
		std::stringstream strm(line);
		double d;
		strm >> d;
		m_yvalues.push_back(d);
		while (strm >> d)
			m_xvalues.push_back(d);
		double minVal = std::numeric_limits<double>::max();
		double maxVal = -std::numeric_limits<double>::max();
		while (std::getline(in, line)) {
			// split at \t char
			std::vector<std::string> tokens;
			explode(line, tokens, '\t');
			m_yvalues.push_back(std::stod(tokens[0]));
			std::vector<double> vals;
			for (unsigned int i=1; i<tokens.size(); ++i) {
				if (tokens[i].empty())
					vals.push_back(qQNaN());
				else {
					double v = std::stod(tokens[i]);
					minVal = std::min(minVal, v);
					maxVal = std::max(maxVal, v);
					vals.push_back(v);
				}
			}
			m_zvalues.push_back(std::vector<double>(vals.begin()+1, vals.end()));
		}

		std::reverse(m_yvalues.begin(), m_yvalues.end());
		std::reverse(m_zvalues.begin(), m_zvalues.end());

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

	// Dies ist die eigentliche Berechnungsfunktion, welche für eine x,y-Plotkoordinate den
	// entsprechenden z-Wert zurückliefert.
	virtual double value(double x, double y) const override {
		// find the interval that x and y falls into
		unsigned int xIdx = 0;
		while (xIdx < m_xvalues.size() && x > m_xvalues[xIdx]) ++xIdx;
		unsigned int yIdx = 0;
		while (yIdx < m_yvalues.size() && y > m_yvalues[yIdx]) ++yIdx;

		// outside bounds?
		if (x < m_xvalues.front() || y < m_yvalues.front() ||
			xIdx == m_xvalues.size()-1 || yIdx == m_yvalues.size()-1)
		{
			return qQNaN();
		}
		return m_zvalues[yIdx][xIdx];
	}

private:
	QwtInterval m_intervals[3];

	std::vector<double> m_xvalues;
	std::vector<double> m_yvalues;
	std::vector<std::vector<double> > m_zvalues;
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
	plot.resize(800,600);

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
	SpectrogramData * data = new SpectrogramData();
	data->read("data.tsv");
	spectro->setData( data );

	LinearColorMap * cm = new LinearColorMap( QwtColorMap::RGB );
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
	QwtScaleWidget* rightAxis = plot.axisWidget( QwtAxis::YRight );
	QwtText title;
	title.setText("Temperature");
	f.setPointSize(8);
	f.setBold(true);
	title.setFont(f);
	rightAxis->setTitle( title);
	rightAxis->setColorBarEnabled( true );
	rightAxis->setColorMap( zInterval, new LinearColorMap( QwtColorMap::RGB ) );
	rightAxis->setLayoutFlag(QwtScaleWidget::TitleInverted, false);

	title.setText("Coordinate");
	plot.axisWidget(QwtAxis::YLeft)->setTitle(title);
	plot.axisWidget(QwtAxis::XBottom)->setTitle(title);

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
