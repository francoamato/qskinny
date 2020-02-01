/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMaterialSkin.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskFunctions.h>
#include <QskMargins.h>
#include <QskPushButton.h>
#include <QskRgbValue.h>
#include <QskSkinlet.h>

namespace
{
    class ColorPalette
    {
      public:
        ColorPalette()
        {
            resetColors();
        }

        void resetColors()
        {
            baseColor = "#6200EE";
            textColor = Qt::white;
        }

        QColor baseColor;
        QColor textColor;
    };
}

class QskMaterialSkin::PrivateData
{
  public:
    ColorPalette palette;
};

QskMaterialSkin::QskMaterialSkin( QObject* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setupFonts( "Roboto" );

    auto buttonFont = font( QskSkin::DefaultFont );
    buttonFont.setCapitalization( QFont::AllUppercase );
    buttonFont.setPixelSize( 14 );

    // ### We could define some ButtonFont or so
    setFont( QskSkin::MediumFont, buttonFont );

    initHints();
}

QskMaterialSkin::~QskMaterialSkin()
{
}

void QskMaterialSkin::initHints()
{
    initCommonHints();

    initPushButtonHints();
}

void QskMaterialSkin::resetColors( const QColor& /*accent*/ )
{
    m_data->palette.resetColors();

    initHints();
}

void QskMaterialSkin::initCommonHints()
{
    using namespace QskAspect;
    using Q = QskControl;

    const ColorPalette& pal = m_data->palette;

    // ### do we need this?
    setGradient( Control, pal.baseColor );
    setColor( Control | StyleColor, pal.textColor );
}

void QskMaterialSkin::initPushButtonHints()
{
    using namespace QskAspect;
    using namespace QskRgbValue;
    using Q = QskPushButton;

    setMetric( Q::Panel | MinimumWidth, qskDpiScaled( 64.0 ) );
    setMetric( Q::Panel | MinimumHeight, qskDpiScaled( 36.0 ) );
    setMetric( Q::Panel | MaximumHeight, qskDpiScaled( 36.0 ) );

    const QskMargins padding( 16, 0 ); // ### vertical padding? ### Depends on icon

    setMargins( Q::Panel | Padding, padding );

    setGradient( Q::Panel, { "#6200EE" } );
    setGradient( Q::Panel | Q::Flat, Qt::white );

    setColor( Q::Text, m_data->palette.textColor );
    setColor( Q::Text | Q::Flat, m_data->palette.baseColor );
    setFontRole( Q::Text, MediumFont );
    setSkinHint( Q::Text | Alignment, Qt::AlignCenter );
    setBoxShape( Q::Panel, 4 );

    // for outlined buttons:
    setBoxBorderColors(Q::Panel | Q::Flat, { m_data->palette.baseColor } );

    setMetric( Q::Graphic | MinimumHeight, 18 );
    setMetric( Q::Graphic | MaximumHeight, 18 );
    setMetric( Q::Graphic | Position, 18 );
}

#include "moc_QskMaterialSkin.cpp"
