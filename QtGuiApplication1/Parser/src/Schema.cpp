#include <cstdlib>
#include <iostream>
#include "Functions.h"

#include "Schema.h"

namespace HT {

// Constructor
Schema::Schema()
 :  QObject(),
    m_changed(true)
{
}

// Constructor for parser function
Schema::Schema(XmlStreamReader& xml, QString CloseTag)
 :  QObject(),
    m_changed(true)
{
}

// copy constructor
Schema::Schema(const Schema &val)
 :  QObject(),
    m_lastError(),
    m_changed(val.m_changed ),
    m_store(val.m_store )
{
}

// compare
bool Schema::operator==(const Schema &val) {

    return true;
}

// assignement
Schema & Schema::operator=(const Schema &val) {

    m_changed = val.m_changed;
    m_store = val.m_store;
    return *this;
}

// getter for Schema
QString Schema::getAttributeFormDefault() const {

    return "unqualified";
}

// getter for Schema
QString Schema::getElementFormDefault() const {

    return "qualified";
}

// Get XML Representation
const QString& Schema::toXML(bool outputNamespace, QString xmlTag, bool bWriteTag ) {

    if ( m_changed ) {
        const static QString endAttr( "\"" );
        QString xml("");
        if(bWriteTag) xml = "<"+ xmlTag;
        if (outputNamespace)
        {
            xml.append(" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
            xml.append(" xmlns=\"none\"");
        }
        if(bWriteTag) xml.append("/>\n");
        m_store = xml;
        m_changed = false;
    }
    return m_store;
}

// Get String Representation
QString Schema::toString() const{

    return toString("");
}

// Get String Representation with a lead
QString Schema::toString(QString lead, QString xmlTag) const{

    const static QString endAttr( "\n" );
    QString str = lead + xmlTag;
    return str;
}

const QString& Schema::lastError() const {
    return m_lastError;
}

const bool& Schema::changed() const {
    return m_changed;
}

const QString& Schema::store() const {
    return m_store;
}


} //end ns
