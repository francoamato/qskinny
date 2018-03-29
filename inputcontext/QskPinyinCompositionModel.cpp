/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPinyinCompositionModel.h"

#include "pinyinime.h"

#include <QDebug>
#include <QVector>

class QskPinyinCompositionModel::PrivateData
{
public:
    PrivateData()
    {
    }

    QList< QString > candidates; // ### put that into input composition model and add protected accessors
    QVector< Qt::Key > groups;
};

QskPinyinCompositionModel::QskPinyinCompositionModel():
    QskInputCompositionModel(),
    m_data( new PrivateData )
{
    QString dictionary = QString::fromLocal8Bit( qgetenv( "DCU_DATA_ROOT" ) )
            + QStringLiteral( "/3rdparty/pinyin/data/dict_pinyin.dat" );
    // ### prevent having 2 calls to im_open_decoder by using a singleton or so
    bool opened = ime_pinyin::im_open_decoder( dictionary.toUtf8().constData(), "" );

    if( !opened )
    {
        qWarning() << "could not open pinyin decoder dictionary at" << dictionary;
    }
}

QskPinyinCompositionModel::~QskPinyinCompositionModel()
{
    ime_pinyin::im_close_decoder();
}

bool QskPinyinCompositionModel::supportsSuggestions() const
{
    return true;
}

int QskPinyinCompositionModel::candidateCount() const
{
    return qMax( 0, m_data->candidates.count() );
}

QString QskPinyinCompositionModel::candidate( int index ) const
{
    return m_data->candidates.at( index );
}

QVector< Qt::Key > QskPinyinCompositionModel::groups() const
{
    return m_data->groups;
}

bool QskPinyinCompositionModel::hasIntermediate() const
{
    return m_data->candidates.count() > 0;
}

QString QskPinyinCompositionModel::polishPreedit( const QString& preedit )
{
    if( preedit.isEmpty() )
    {
        ime_pinyin::im_reset_search();
    }

    QByteArray preeditBuffer = preedit.toLatin1();
    size_t numSearchResults = ime_pinyin::im_search( preeditBuffer.constData(), size_t( preeditBuffer.length() ) );

    if( numSearchResults > 0 )
    {
        QList< QString > newCandidates;
        newCandidates.reserve( 1 );

        QVector< QChar > candidateBuffer;
        candidateBuffer.resize( ime_pinyin::kMaxSearchSteps + 1 );

        // ### numSearchResults is way too big, we should only do this for the first ten results or so
        for( unsigned int a = 0; a < numSearchResults; a++ )
        {
            size_t length = static_cast< size_t >( candidateBuffer.length() - 1 );
            bool getCandidate = ime_pinyin::im_get_candidate( a, reinterpret_cast< ime_pinyin::char16* >( candidateBuffer.data() ), length );

            QString candidate;

            if( getCandidate )
            {
                candidateBuffer.last() = 0;
                candidate = QString( candidateBuffer.data() );
            }

            Qt::Key key = Qt::Key( candidate.at( 0 ).unicode() );
            QString string = QChar( key );

            newCandidates.append( string );
        }

        m_data->candidates = newCandidates;
        Q_EMIT candidatesChanged();
    }

    return preedit;
}

bool QskPinyinCompositionModel::isComposable( const QStringRef& preedit ) const
{
    Q_UNUSED( preedit );
    return false; // ### implement
}

void QskPinyinCompositionModel::handleGroupIndexChanged()
{
    // ### implement
}

#include "moc_QskPinyinCompositionModel.cpp"
