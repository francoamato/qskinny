#include "QskFlowView.h"

#include "QskEvent.h"
#include "QskGesture.h"

#include <QtMath>

QskFlowView::QskFlowView( QQuickItem* parent ) : QskControl( parent )
{
    m_panRecognizer.setOrientations( Qt::Horizontal );
    m_panRecognizer.setWatchedItem( this );

    setAcceptedMouseButtons( Qt::LeftButton ); // to get gesture events from the mouse as well
}

QskFlowView::~QskFlowView()
{
}

qreal QskFlowView::angle() const
{
    return m_angle;
}

void QskFlowView::setAngle( qreal angle )
{
    m_angle = angle;
}

int QskFlowView::visibleCount() const
{
    // should be an odd number
    return m_visibleCount;
}

void QskFlowView::setVisibleCount( int count )
{
    m_visibleCount = count;
}

int QskFlowView::currentIndex() const
{
    return m_currentIndex;
}

void QskFlowView::setCurrentIndex( int index )
{
    m_currentIndex = index;
}

int QskFlowView::count() const
{
    return m_count;
}

void QskFlowView::setCount(int count)
{
    m_count = count;
}

Qsk::Direction QskFlowView::swipeDirection() const
{
    return m_swipeDirection;
}

qreal QskFlowView::currentItemWidth() const
{
    auto radians = qDegreesToRadians( angle() );
    qreal scaleFactor = qCos( radians ); // ### does not seem to be quite right yet for ~ 45 degrees
    auto width = contentsRect().width() / ( 1 + ( visibleCount() - 1 ) * scaleFactor );

    return width;
}

void QskFlowView::gestureEvent( QskGestureEvent* event )
{
    // ### Shouldn't we be getting a swipe gesture?

    if ( event->gesture()->type() == QskGesture::Pan )
    {
        const auto gesture = static_cast< const QskPanGesture* >( event->gesture() );
        switch ( gesture->state() )
        {
            case QskGesture::Started:
            {
                auto deltaX = gesture->delta().x();
                m_swipeOffset = deltaX;
                m_swipeDirection = ( deltaX < 0 ) ? Qsk::RightToLeft : Qsk::LeftToRight;
                break;
            }
            case QskGesture::Updated:
            {
                // ### Try this: If velocity is above a certain threshold, swipe
                // fast through all covers, otherwise swipe to the next one

//                auto v = gesture->velocity();

//                if( v > 3500 || v < -3500 )
//                {
//                    qDebug() << "fast swipe";
//                    // ### here implement fast swipe
//                }
//                else
//                {
                    m_swipeOffset += gesture->delta().x();
//                    qDebug() << "swipe offset" << m_swipeOffset;
                    update();
//                }
                break;
            }
            case QskGesture::Finished:
            {
                if( m_swipeDirection == Qsk::LeftToRight )
                {
                    if( m_currentIndex > 0 )
                    {
                        m_currentIndex--;
                        update();
                    }
                }
                else
                {
                    if( m_currentIndex < count() - 1 )
                    {
                        m_currentIndex++;
                        update();
                    }

                }

                // ### here align covers

//                m_data->flicker.setWindow( window() );
//                m_data->flicker.accelerate( gesture->angle(), gesture->velocity() );
                break;
            }
            case QskGesture::Canceled:
            {
                // what to do here: maybe going back to the origin of the gesture ??
                break;
            }
            default:
                break;
        }

        return;
    }

    QskControl::gestureEvent( event );
}

bool QskFlowView::gestureFilter( QQuickItem* item, QEvent* event )
{
    // ### some stuff here is just commented out, check what to do with it:

    if ( event->type() == QEvent::MouseButtonPress )
    {
        // Checking first if panning is possible at all

        bool maybeGesture = false;

        const auto orientations = m_panRecognizer.orientations();
        if ( orientations )
        {
//            const QSizeF viewSize = viewContentsRect().size();
//            const QSizeF& scrollableSize = m_data->scrollableSize;

            if ( orientations & Qt::Vertical )
            {
//                if ( viewSize.height() < scrollableSize.height() )
                    maybeGesture = true;
            }

            if ( orientations & Qt::Horizontal )
            {
//                if ( viewSize.width() < scrollableSize.width() )
                    maybeGesture = true;
            }
        }

        if ( !maybeGesture )
            return false;
    }

    /*
        This code is a bit tricky as the filter is called in different situations:

        a) The press was on a child of the view
        b) The press was on the view

        In case of b) things are simple and we can let the recognizer
        decide without timeout if it is was a gesture or not.

        In case of a) we give the recognizer some time to decide - usually
        based on the distances of the following mouse events. If no decision
        could be made the recognizer aborts and replays the mouse events, so
        that the children can process them.

        But if a child does not accept a mouse event it will be sent to
        its parent. So we might finally receive the reposted events, but then
        we can proceed as in b).
     */

    auto& recognizer = m_panRecognizer;

    if ( event->type() == QEvent::MouseButtonPress )
    {
        if ( ( item != this ) && ( recognizer.timeout() < 0 ) )
        {
            const auto mouseEvent = static_cast< QMouseEvent* >( event );

            if ( recognizer.hasProcessedBefore( mouseEvent ) )
                return false;
        }

//        recognizer.setTimeout( ( item == this ) ? -1 : m_data->panRecognizerTimeout );
    }

    return m_panRecognizer.processEvent( item, event );
}
