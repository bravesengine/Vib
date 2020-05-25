#include <QRegExp>

#include "Functions.h"


#include "Parser.h"

namespace HT {

// Constructor
Parser::Parser() {

    m_xml = new XmlStreamReader;
    connect( m_xml, SIGNAL(signalValidationError(const QString&)),
             this,  SIGNAL(signalValidationError(const QString&)) );
}

// Destructor
Parser::~Parser() {

    if(m_xml != NULL)
		delete m_xml;
    m_xml = NULL;
}

// the actual parsing routine
bool Parser::parseXMLString(QString data, bool cont) { 

     // add the data to what was left over from a previous parse run
    // search the buffer for the nearest closetag
    int index = 0;
    QRegExp rx( "</Samples>");
    m_xml->addData( data );
    if ( (index = rx.indexIn( data )) != -1 )
    {
        // end found in last part
        QString residu( data );
        do
        {
            parse();
            m_xml->clear();

            // add make residu
            int len = index + rx.matchedLength();
            residu = residu.right( residu.length() - len );
            m_xml->addData( residu );

            // loop until no end found in residu
        } while ( (index = rx.indexIn( residu )) != -1 );
    }

    if (!cont) {
        m_xml->clear();
    }
    return true;
}

QString composeMessage( const QXmlStreamReader& xml ) {
    QString errorstr( xml.errorString() );
    errorstr += " at line " + QString::number(xml.lineNumber());
    errorstr += " (column " + QString::number(xml.columnNumber());
    errorstr += ")";
    return errorstr;
}

void Parser::parse() {

    bool stop( false );
    while(!m_xml->atEnd() && !stop)
    {
        QXmlStreamReader::TokenType token = m_xml->readNext();
        if ( token == QXmlStreamReader::StartElement )
        {
            if( m_xml->name()=="Samples" )
            {
                Samples obj( *m_xml );
                if ( m_xml->name() != "Samples" )
                    m_xml->raiseError( "tag mismatch Samples" );
                else
                {
                    emit signalSamples( obj );
                    stop = true;
                }
            }
        }
    }

    if ( m_xml->hasError() )
    {
        switch ( m_xml->error() )
        {
        case QXmlStreamReader::CustomError:
            emit( signalValidationError( composeMessage( *m_xml ) ) );
            break;
        case QXmlStreamReader::PrematureEndOfDocumentError:
            emit( signalWarning( composeMessage( *m_xml ) ) );
            break;
        case QXmlStreamReader::NotWellFormedError:
        case QXmlStreamReader::UnexpectedElementError:
            emit( signalError( composeMessage( *m_xml ) ) );
            break;
        case QXmlStreamReader::NoError:
            break;
        }
    }
}

} //end ns
