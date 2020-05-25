#include <cstdlib>
#include <iostream>
#include "Functions.h"

#include "Samples.h"


#include "Sample.h"
namespace HT {

// Constructor
Samples::Samples()
 :  QObject(),
    m_changed(true)
{
}

// Constructor for parser function
Samples::Samples(XmlStreamReader& xml, QString CloseTag)
 :  QObject(),
    m_changed(true)
{
    bool stop(false);
    while(!xml.atEnd() && !stop)
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        switch ( token )
        {
        case QXmlStreamReader::EndElement:
            if (  xml.name() ==CloseTag)
                stop = true;
            break;
        case QXmlStreamReader::StartElement:
            if ( xml.name() == "Sample" )
            {
                Sample val( xml );
                if ( xml.name() != "Sample" )
                    xml.raiseError( "tag mismatch Sample" );
                else if ( !addSample( val ) )
                    xml.validationError( "error add Sample"  );
            }
            else
            {
                xml.validationError( "unexpected element " + xml.name().toString() );
            }
            break;
        default:
            break;
        }
    }
}

// copy constructor
Samples::Samples(const Samples &val)
 :  QObject(),
    m_samples( val.m_samples ),
    m_lastError(),
    m_changed(val.m_changed ),
    m_store(val.m_store )
{
}

// compare
bool Samples::operator==(const Samples &val) {

    if (!(m_samples == val.m_samples)) return false;
    return true;
}

// assignement
Samples & Samples::operator=(const Samples &val) {

    m_samples = val.m_samples;
    m_changed = val.m_changed;
    m_store = val.m_store;
    return *this;
}

// remover for Samples
bool Samples::removeSample(const Sample& val) {

    m_changed = true;
    return m_samples.removeOne(val);
}

// setter for Samples
bool Samples::addSample(const Sample& val) {

    m_samples.append(val);
    m_changed = true;
    return true;
}

// getter for Samples
const Sample& Samples::getSampleAt(int i) const {

    return m_samples.at(i);
}

// count for Samples
int Samples::countOfSamples() const {

    return m_samples.count();
}

// Get XML Representation
const QString& Samples::toXML(bool outputNamespace, QString xmlTag, bool bWriteTag ) {

    if ( m_changed ) {
        const static QString endAttr( "\"" );
        QString xml("");
        if(bWriteTag) xml = "<"+ xmlTag;
        if (outputNamespace)
        {
            xml.append(" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
            xml.append(" xmlns=\"none\"");
        }
        QString dataMember;
        if(bWriteTag) xml.append(">\n");
        // add all included data
        for(int i=0; i < m_samples.count(); i++ ) {
            Sample attribute = m_samples.at(i);
            dataMember = attribute.toXML(false);
            if (dataMember != QString::null) {
               xml.append( dataMember );
            } else {
                m_lastError = "Sample:" + attribute.lastError();
                m_store  = QString::null;
                return m_store;
            }
        }
        xml.append( "</" + xmlTag + ">\n");
        m_store = xml;
        m_changed = false;
    }
    return m_store;
}

// Get String Representation
QString Samples::toString() const{

    return toString("");
}

// Get String Representation with a lead
QString Samples::toString(QString lead, QString xmlTag) const{

    const static QString endAttr( "\n" );
    QString str = lead + xmlTag;
    // add all included data
    for(int i=0; i < m_samples.count(); i++ ) {
        Sample attribute = m_samples.at(i);
        str.append( attribute.toString( lead + "    " ) );
    }
    return str;
}

const QString& Samples::lastError() const {
    return m_lastError;
}

const bool& Samples::changed() const {
    return m_changed;
}

const QString& Samples::store() const {
    return m_store;
}


} //end ns
