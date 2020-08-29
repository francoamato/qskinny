#include "Diagram.h"

#include <QPainter>

#include <cmath>

namespace
{
    float distance( const QPointF& pt1, const QPointF& pt2 )
    {
        float hd = ( pt1.x() - pt2.x() ) * ( pt1.x() - pt2.x() );
        float vd = ( pt1.y() - pt2.y() ) * ( pt1.y() - pt2.y() );
        return std::sqrt( hd + vd );
    }

    QPointF getLineStart( const QPointF& pt1, const QPointF& pt2 )
    {
        QPointF pt;
        float rat = 10.0 / distance( pt1, pt2 );

        if( rat > 0.5 )
        {
            rat = 0.5;
        }

        pt.setX( ( 1.0 - rat ) * pt1.x() + rat * pt2.x() );
        pt.setY( ( 1.0 - rat ) * pt1.y() + rat * pt2.y() );
        return pt;
    }

    QPointF getLineEnd( const QPointF& pt1, const QPointF& pt2 )
    {
        QPointF pt;
        float rat = 10.0 / distance( pt1, pt2 );

        if( rat > 0.5 )
        {
            rat = 0.5;
        }

        pt.setX( rat * pt1.x() + ( 1.0 - rat )*pt2.x() );
        pt.setY( rat * pt1.y() + ( 1.0 - rat )*pt2.y() );
        return pt;
    }

    QPainterPath smoothOut( const QPainterPath& path )
    {
        QList<QPointF> points;
        QPointF p;

        for( int i = 0; i < path.elementCount() - 1; i++ )
        {
            p = QPointF( path.elementAt( i ).x, path.elementAt( i ).y );
            points.append( p );
        }

        QPointF pt1;
        QPointF pt2;
        QPainterPath newPath;

        for( int i = 0; i < points.count() - 1; i++ )
        {
            pt1 = getLineStart( points[i], points[i + 1] );

            if( i == 0 )
            {
                newPath.moveTo( points[0] );
            }
            else
            {
                newPath.quadTo( points[i], pt1 );
            }

            pt2 = getLineEnd( points[i], points[i + 1] );
            newPath.lineTo( pt2 );
        }

        return newPath;
    }
}

static constexpr int segments = 7;

Diagram::Diagram( QQuickItem* parent )
    : QskControl( parent )
    , m_content( new DiagramContent( this ) )
{
    setAutoLayoutChildren( true );
}

void Diagram::updateLayout()
{
    m_content->setContentsSize( size().toSize() );
    m_content->update();
}

DiagramContent::DiagramContent( QQuickItem* parent ) : QQuickPaintedItem( parent )
{
}

void DiagramContent::paint( QPainter* painter )
{
    painter->setRenderHint( QPainter::Antialiasing, true );

    QPair<QColor, QColor> colors[] =
    {
        {"#996776FF", "#116776FF"},
        {"#aaFF3122", "#11FF3122"},
        {"#FF7D34", "#11FF7D34"}
    };

    qreal yValues[][8] =
    {
        {0.8, 0.85, 0.92, 0.5, 0.88, 0.7, 0.8, 0.3},
        {0.2, 0.6, 0.5, 0.9, 0.3, 0.4, 0.8, 0.4},
        {0.5, 0.4, 0.7, 0.1, 0.6, 0.9, 0.3, 0.1}
    };

    for( int i = 0; i < 3; i++ )
    {
        QLinearGradient myGradient( {width() / 2, 0}, {width() / 2, height()} );
        myGradient.setColorAt( 0, colors[i].first );
        myGradient.setColorAt( 1, colors[i].second );
        QPen myPen( Qt::transparent );

        QPainterPath myPath;
        myPath.moveTo( 0, height() );
        myPath.lineTo( 0, 50 );

        qreal stepSize = width() / segments;

        for( int j = 1; j < segments; j++ )
        {
            qreal x1 = j * stepSize + stepSize / 2;
            qreal y = ( 1 - yValues[i][j] ) * height();
            myPath.lineTo( x1, y );
        }

        myPath.lineTo( width(), ( 1 - yValues[i][7] ) * height() );
        myPath.lineTo( width(), height() );

        QPainterPath smoothPath = smoothOut( myPath );
        smoothPath.lineTo( width(), height() );
        painter->setBrush( myGradient );
        painter->setPen( myPen );
        painter->drawPath( smoothPath );
    }
}
