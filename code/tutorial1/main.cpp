#include <QApplication>

#include <QwtPlot>
#include <QwtPlotCurve>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QwtPlot plot;
	plot.resize(500,300);

	// etwas Abstand zwischen Rand und Achsentiteln
	plot.setContentsMargins(8,8,8,8);
	// Hintergrund der Zeichenfläche soll weiß sein
	plot.setCanvasBackground( Qt::white );

	// Daten zum Darstellen
	QVector<double> x = {0,4,5,10,12};
	QVector<double> y = {5.1,4,6.8,6.5,5.2};

	QwtPlotCurve *curve = new QwtPlotCurve();
	curve->setPen(QColor(180,40,20), 0);
	curve->setTitle("Line 1");
	curve->setRenderHint( QwtPlotItem::RenderAntialiased, true ); // Antialiasing verwenden
	curve->setSamples(x, y);
	curve->attach(&plot); // Plot takes ownership

	plot.show();
	return a.exec();
}
