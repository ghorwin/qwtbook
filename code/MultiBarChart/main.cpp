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

#include <QwtPlot>
#include <QwtLegend>
#include <QwtText>
#include <QwtPlotGrid>
#include <QwtPlotZoomer>
#include <QwtPlotMultiBarChart>
#include <QwtColumnSymbol>
#include <QwtScaleDraw>
#include <QwtPlotLayout>


class ScaleDraw : public QwtScaleDraw {
public:
	ScaleDraw(const QStringList& labels) : m_labels(labels) {
		enableComponent(QwtScaleDraw::Ticks, false);
		enableComponent(QwtScaleDraw::Backbone, false);
		setLabelAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}

	virtual QwtText label(double value) const QWT_OVERRIDE {
		const int index = qRound(value);
		if (index >= 0 && index < m_labels.size() && qAbs(index - value) < 1e-6)
			return m_labels[index];
		return QwtText();
	}

	QStringList m_labels;
};


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	plot.resize(500, 350);

	plot.setContentsMargins(8, 8, 8, 8);
	plot.setCanvasBackground(Qt::white);

	// Gitter hinzufügen
	QwtPlotGrid *grid = new QwtPlotGrid;
	grid->enableX(false);
	grid->setMajorPen(Qt::gray, 0, Qt::DotLine);
	grid->attach(&plot);

	// Daten: 4 Städte (Gruppen), je 3 Balken (Jahre 2021, 2022, 2023)
	// Jeder Eintrag in samples entspricht einer Gruppe (x-Position),
	// jeder Wert darin einem Balken der Gruppe.
	QVector<QVector<double>> samples;
	samples << (QVector<double>() << 10 << 15 << 12); // Dresden
	samples << (QVector<double>() << 20 << 18 << 22); // Berlin
	samples << (QVector<double>() << 15 << 13 << 17); // Leipzig
	samples << (QVector<double>() << 14 << 16 << 19); // Hamburg

	QwtPlotMultiBarChart *barChart = new QwtPlotMultiBarChart;
	barChart->setStyle(QwtPlotMultiBarChart::Grouped);
	barChart->setSamples(samples);

	// Farben und Symbole für jeden Balken (Index = Jahres-Reihe)
	const QColor colors[] = {
		QColor(0x5b9bd5), // blau  — 2021
		QColor(0x70ad47), // grün  — 2022
		QColor(0xed7d31)  // orange — 2023
	};
	for (int i = 0; i < 3; ++i) {
		QwtColumnSymbol *sym = new QwtColumnSymbol(QwtColumnSymbol::Box);
		sym->setFrameStyle(QwtColumnSymbol::Plain);
		sym->setLineWidth(1);
		QPalette pal(colors[i].lighter(130));
		pal.setBrush(QPalette::Dark, colors[i].darker(140));
		sym->setPalette(pal);
		barChart->setSymbol(i, sym);
	}

	// Legendentitel für jede Balkenreihe
	QList<QwtText> titles;
	titles << QwtText("2021") << QwtText("2022") << QwtText("2023");
	barChart->setBarTitles(titles);
	barChart->setLegendIconSize(QSize(10, 14));

	barChart->attach(&plot);

	// Legende
	QwtLegend *legend = new QwtLegend();
	QFont legendFont;
	legendFont.setPointSize(8);
	legend->setFont(legendFont);
	plot.insertLegend(legend, QwtPlot::RightLegend);

	// Titel
	QwtText title("QwtPlotMultiBarChart (gruppiert)");
	QFont titleFont;
	titleFont.setBold(true);
	titleFont.setPointSize(10);
	title.setFont(titleFont);
	plot.setTitle(title);

	// x-Achse mit Städtenamen beschriften
	QwtScaleDraw *scaleDraw = new ScaleDraw(
		QStringList() << "Dresden" << "Berlin" << "Leipzig" << "Hamburg");
	QFont axisFont;
	axisFont.setPointSize(8);
	axisFont.setBold(true);
	plot.setAxisFont(QwtPlot::xBottom, axisFont);
	plot.setAxisScaleDraw(QwtPlot::xBottom, scaleDraw);
	plot.setAxisScale(QwtPlot::xBottom, -0.5, 3.5);

	barChart->setMargin(10);
	plot.plotLayout()->setCanvasMargin(0);
	plot.plotLayout()->setAlignCanvasToScale(QwtPlot::yLeft, false);
	plot.updateCanvasMargins();

	QwtPlotZoomer *zoomer = new QwtPlotZoomer(
		QwtPlot::xBottom, QwtPlot::yLeft, plot.canvas());
	zoomer->setTrackerMode(QwtPlotPicker::AlwaysOn);

	plot.show();
	return a.exec();
}
