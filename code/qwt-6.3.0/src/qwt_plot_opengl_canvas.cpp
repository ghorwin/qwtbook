/******************************************************************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot_opengl_canvas.h"
#include "qwt_plot.h"
#include "qwt_painter.h"

#include <qpainter.h>
#include <qpainterpath.h>
#include <qcoreevent.h>
#include <qopenglframebufferobject.h>
#include <qopenglpaintdevice.h>

class QwtPlotOpenGLCanvas::PrivateData
{
  public:
    PrivateData()
        : numFBOSamples( -1 )
        , isPolished( false )
        , fboDirty( true )
        , fbo( NULL )
    {
    }

    ~PrivateData()
    {
        delete fbo;
    }

    int numFBOSamples;

    bool isPolished;
    bool fboDirty;
    QOpenGLFramebufferObject* fbo;
};

/*!
   \brief Constructor

   \param plot Parent plot widget
   \sa QwtPlot::setCanvas()
 */
QwtPlotOpenGLCanvas::QwtPlotOpenGLCanvas( QwtPlot* plot )
    : QOpenGLWidget( plot )
    , QwtPlotAbstractGLCanvas( this )
{
    init();
}

/*!
   \brief Constructor

   \param numSamples Number of samples, see QSurfaceFormat::samples()
   \param plot Parent plot widget
   \sa QwtPlot::setCanvas()
 */
QwtPlotOpenGLCanvas::QwtPlotOpenGLCanvas( int numSamples, QwtPlot* plot )
    : QOpenGLWidget( plot )
    , QwtPlotAbstractGLCanvas( this )
{
    if ( numSamples < -1 )
        numSamples = -1;

    QSurfaceFormat fmt = format();
    if ( numSamples != fmt.samples() )
    {
        fmt.setSamples( numSamples );
        setFormat( fmt );
    }

    init();
}

void QwtPlotOpenGLCanvas::init()
{
    m_data = new PrivateData;

#if 1
    setAttribute( Qt::WA_OpaquePaintEvent, true );
#endif

    setLineWidth( 2 );
    setFrameShadow( QFrame::Sunken );
    setFrameShape( QFrame::Panel );
}

//! Destructor
QwtPlotOpenGLCanvas::~QwtPlotOpenGLCanvas()
{
    delete m_data;
}

/*!
   Paint event

   \param event Paint event
   \sa QwtPlot::drawCanvas()
 */
void QwtPlotOpenGLCanvas::paintEvent( QPaintEvent* event )
{
    if ( m_data->isPolished )
        QOpenGLWidget::paintEvent( event );
}

/*!
   Qt event handler for QEvent::PolishRequest and QEvent::StyleChange
   \param event Qt Event
   \return See QGLWidget::event()
 */
bool QwtPlotOpenGLCanvas::event( QEvent* event )
{
    if ( event->type() == QEvent::Resize )
    {
        if ( m_data->numFBOSamples < 0  )
        {
            /*
                QOpenGLWidget always uses a FBO and sets the number of samples for
                the FBO not for the widget itself. So format().samples() does not
                return the correct value after the first QEvent::Resize.
             */
            m_data->numFBOSamples = qMax( format().samples(), 0 );
        }
    }

    const bool ok = QOpenGLWidget::event( event );

    if ( event->type() == QEvent::PolishRequest )
    {
        // In opposite to non OpenGL widgets receive pointless
        // early repaints. As we always have a QEvent::PolishRequest
        // followed by QEvent::Paint, we can ignore all these repaints.

        m_data->isPolished = true;
    }

    if ( event->type() == QEvent::PolishRequest ||
        event->type() == QEvent::StyleChange )
    {
        // assuming, that we always have a styled background
        // when we have a style sheet

        setAttribute( Qt::WA_StyledBackground,
            testAttribute( Qt::WA_StyleSheet ) );
    }

    return ok;
}

/*!
   Invalidate the paint cache and repaint the canvas
   \sa invalidatePaintCache()
 */
void QwtPlotOpenGLCanvas::replot()
{
    QwtPlotAbstractGLCanvas::replot();
}

//! Invalidate the internal backing store
void QwtPlotOpenGLCanvas::invalidateBackingStore()
{
    m_data->fboDirty = true;
}

void QwtPlotOpenGLCanvas::clearBackingStore()
{
    delete m_data->fbo;
    m_data->fbo = NULL;
}

/*!
   Calculate the painter path for a styled or rounded border

   When the canvas has no styled background or rounded borders
   the painter path is empty.

   \param rect Bounding rectangle of the canvas
   \return Painter path, that can be used for clipping
 */
QPainterPath QwtPlotOpenGLCanvas::borderPath( const QRect& rect ) const
{
    return canvasBorderPath( rect );
}

//! No operation - reserved for some potential use in the future
void QwtPlotOpenGLCanvas::initializeGL()
{
}

//! Paint the plot
void QwtPlotOpenGLCanvas::paintGL()
{
    const bool hasFocusIndicator =
        hasFocus() && focusIndicator() == CanvasFocusIndicator;

    QPainter painter;

    if ( testPaintAttribute( QwtPlotOpenGLCanvas::BackingStore ) &&
        QOpenGLFramebufferObject::hasOpenGLFramebufferBlit() )
    {
        const qreal pixelRatio = QwtPainter::devicePixelRatio( this );
        const QSize fboSize = size() * pixelRatio;

        if ( hasFocusIndicator )
            painter.begin( this );

        /*
           QOpenGLWidget has its own internal FBO, that is used to restore
           its content without having to repaint. This works fine when f.e
           a rubberband is moving on top, but there are still situations,
           where we can repaint without an potentially expensive replot:

               - when having the focus the top level window gets activated/deactivated
               - ???
         */

        if ( m_data->fbo )
        {
            if ( m_data->fbo->size() != fboSize )
            {
                delete m_data->fbo;
                m_data->fbo = NULL;
            }
        }

        if ( m_data->fbo == NULL )
        {
            QOpenGLFramebufferObjectFormat fboFormat;
            fboFormat.setAttachment( QOpenGLFramebufferObject::CombinedDepthStencil );

            if ( m_data->numFBOSamples > 0 )
                fboFormat.setSamples( m_data->numFBOSamples );

            m_data->fbo = new QOpenGLFramebufferObject( fboSize, fboFormat );
            m_data->fboDirty = true;
        }

        if ( m_data->fboDirty )
        {
            m_data->fbo->bind();

            QOpenGLPaintDevice pd( fboSize );

            QPainter fboPainter( &pd );
            fboPainter.scale( pixelRatio, pixelRatio );
            draw( &fboPainter);
            fboPainter.end();

            m_data->fboDirty = false;
        }

        QOpenGLFramebufferObject::blitFramebuffer( NULL, m_data->fbo );
    }
    else
    {
        painter.begin( this );
        draw( &painter );
    }

    if ( hasFocusIndicator )
        drawFocusIndicator( &painter );
}

//! No operation - reserved for some potential use in the future
void QwtPlotOpenGLCanvas::resizeGL( int, int )
{
    // nothing to do
}

#include "moc_qwt_plot_opengl_canvas.cpp"
