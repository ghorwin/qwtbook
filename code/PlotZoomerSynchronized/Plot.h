#ifndef PLOT_H
#define PLOT_H

#include <qwt_plot.h>

class QwtPlotZoomer;

class Plot : public QwtPlot {
	Q_OBJECT
public:
	Plot();


public slots:
	// Connected to other plot(s)
	void onZoomed( const QRectF& rect );

signals:
	void zoomed( const QRectF& rect );

private:
	QwtPlotZoomer * zoomer;
};

#endif // PLOT_H
