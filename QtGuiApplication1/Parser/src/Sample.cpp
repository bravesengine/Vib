#include <cstdlib>
#include <iostream>
#include "Functions.h"

#include "Sample.h"

namespace HT {

// Constructor
Sample::Sample()
 :  QObject(),
    m_title(),
    m_titlePresent( false ),
    m_desc(),
    m_descPresent( false ),
    m_data(),
    m_dataPresent( false ),
    m_minRepeatPresent( false ),
    m_maxRepeatPresent( false ),
    m_defaultRepeatPresent( false ),
    m_changed(true)
{
}

// Constructor for parser function
Sample::Sample(XmlStreamReader& xml, QString CloseTag)
 :  QObject(),
    m_title(),
    m_titlePresent( false ),
    m_desc(),
    m_descPresent( false ),
    m_data(),
    m_dataPresent( false ),
    m_minRepeatPresent( false ),
    m_maxRepeatPresent( false ),
    m_defaultRepeatPresent( false ),
    m_changed(true)
{
    QXmlStreamAttributes attr = xml.attributes();
    if ( attr.hasAttribute( "Title" ) )
    {
        if ( !setTitle( attr.value( "Title" ).toString() ) )
            xml.validationError( "error set Title = " + attr.value( "Title" ).toString() );
    }
    if ( attr.hasAttribute( "Desc" ) )
    {
        if ( !setDesc( attr.value( "Desc" ).toString() ) )
            xml.validationError( "error set Desc = " + attr.value( "Desc" ).toString() );
    }
    if ( attr.hasAttribute( "Data" ) )
    {
        if ( !setData( attr.value( "Data" ).toString() ) )
            xml.validationError( "error set Data = " + attr.value( "Data" ).toString() );
    }
    if ( attr.hasAttribute( "MinRepeat" ) )
    {
        if ( !setMinRepeat( attr.value( "MinRepeat" ).toString().toUInt() ) )
            xml.validationError( "error set MinRepeat = " + attr.value( "MinRepeat" ).toString() );
    }
    if ( attr.hasAttribute( "MaxRepeat" ) )
    {
        if ( !setMaxRepeat( attr.value( "MaxRepeat" ).toString().toUInt() ) )
            xml.validationError( "error set MaxRepeat = " + attr.value( "MaxRepeat" ).toString() );
    }
    if ( attr.hasAttribute( "DefaultRepeat" ) )
    {
        if ( !setDefaultRepeat( attr.value( "DefaultRepeat" ).toString().toUInt() ) )
            xml.validationError( "error set DefaultRepeat = " + attr.value( "DefaultRepeat" ).toString() );
    }
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
        default:
            break;
        }
    }
}

// copy constructor
Sample::Sample(const Sample &val)
 :  QObject(),
    m_title( val.m_title ),
    m_titlePresent( val.m_titlePresent ),
    m_desc( val.m_desc ),
    m_descPresent( val.m_descPresent ),
    m_data( val.m_data ),
    m_dataPresent( val.m_dataPresent ),
    m_minRepeat( val.m_minRepeat ),
    m_minRepeatPresent( val.m_minRepeatPresent ),
    m_maxRepeat( val.m_maxRepeat ),
    m_maxRepeatPresent( val.m_maxRepeatPresent ),
    m_defaultRepeat( val.m_defaultRepeat ),
    m_defaultRepeatPresent( val.m_defaultRepeatPresent ),
    m_lastError(),
    m_changed(val.m_changed ),
    m_store(val.m_store )
{
}

// compare
bool Sample::operator==(const Sample &val) {

    if (!(m_titlePresent == val.m_titlePresent)) return false;
    if (!(m_title == val.m_title)) return false;
    if (!(m_descPresent == val.m_descPresent)) return false;
    if (!(m_desc == val.m_desc)) return false;
    if (!(m_dataPresent == val.m_dataPresent)) return false;
    if (!(m_data == val.m_data)) return false;
    if (!(m_minRepeatPresent == val.m_minRepeatPresent)) return false;
    if (!(m_minRepeat == val.m_minRepeat)) return false;
    if (!(m_maxRepeatPresent == val.m_maxRepeatPresent)) return false;
    if (!(m_maxRepeat == val.m_maxRepeat)) return false;
    if (!(m_defaultRepeatPresent == val.m_defaultRepeatPresent)) return false;
    if (!(m_defaultRepeat == val.m_defaultRepeat)) return false;
    return true;
}

// assignement
Sample & Sample::operator=(const Sample &val) {

    m_titlePresent = val.m_titlePresent;
    m_title = val.m_title;
    m_descPresent = val.m_descPresent;
    m_desc = val.m_desc;
    m_dataPresent = val.m_dataPresent;
    m_data = val.m_data;
    m_minRepeatPresent = val.m_minRepeatPresent;
    m_minRepeat = val.m_minRepeat;
    m_maxRepeatPresent = val.m_maxRepeatPresent;
    m_maxRepeat = val.m_maxRepeat;
    m_defaultRepeatPresent = val.m_defaultRepeatPresent;
    m_defaultRepeat = val.m_defaultRepeat;
    m_changed = val.m_changed;
    m_store = val.m_store;
    return *this;
}

// setter for Sample
bool Sample::setTitle(QString val) {

    m_titlePresent = true;
    m_title = val;
    m_changed = true;
    return true;
}

// getter for Sample
QString Sample::getTitle() const {

    return m_title;
}

// setter for Sample
bool Sample::setDesc(QString val) {

    m_descPresent = true;
    m_desc = val;
    m_changed = true;
    return true;
}

// getter for Sample
QString Sample::getDesc() const {

    return m_desc;
}

// setter for Sample
bool Sample::setData(QString val) {

    m_dataPresent = true;
    m_data = val;
    m_changed = true;
    return true;
}

// getter for Sample
QString Sample::getData() const {

    return m_data;
}

// setter for Sample
bool Sample::setMinRepeat(unsigned int val) {

    m_minRepeatPresent = true;
    m_minRepeat = val;
    m_changed = true;
    return true;
}

// getter for Sample
unsigned int Sample::getMinRepeat() const {

    return m_minRepeat;
}

// setter for Sample
bool Sample::setMaxRepeat(unsigned int val) {

    m_maxRepeatPresent = true;
    m_maxRepeat = val;
    m_changed = true;
    return true;
}

// getter for Sample
unsigned int Sample::getMaxRepeat() const {

    return m_maxRepeat;
}

// setter for Sample
bool Sample::setDefaultRepeat(unsigned int val) {

    m_defaultRepeatPresent = true;
    m_defaultRepeat = val;
    m_changed = true;
    return true;
}

// getter for Sample
unsigned int Sample::getDefaultRepeat() const {

    return m_defaultRepeat;
}

// Get XML Representation
const QString& Sample::toXML(bool outputNamespace, QString xmlTag, bool bWriteTag ) {

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
            // check for presence of required  attribute
        if ( m_titlePresent) {
            xml.append(" Title=\"" + encode (m_title) + endAttr);
        } else { // required attribute not present
            m_lastError = "Title not set";
            m_store  = QString::null;
            return m_store;
        }
            // check for presence of required  attribute
        if ( m_descPresent) {
            xml.append(" Desc=\"" + encode (m_desc) + endAttr);
        } else { // required attribute not present
            m_lastError = "Desc not set";
            m_store  = QString::null;
            return m_store;
        }
            // check for presence of required  attribute
        if ( m_dataPresent) {
            xml.append(" Data=\"" + encode (m_data) + endAttr);
        } else { // required attribute not present
            m_lastError = "Data not set";
            m_store  = QString::null;
            return m_store;
        }
            // check for presence of required  attribute
        if ( m_minRepeatPresent) {
            xml.append(" MinRepeat=\"" + QString::number( m_minRepeat ) + endAttr);
        } else { // required attribute not present
            m_lastError = "MinRepeat not set";
            m_store  = QString::null;
            return m_store;
        }
            // check for presence of required  attribute
        if ( m_maxRepeatPresent) {
            xml.append(" MaxRepeat=\"" + QString::number( m_maxRepeat ) + endAttr);
        } else { // required attribute not present
            m_lastError = "MaxRepeat not set";
            m_store  = QString::null;
            return m_store;
        }
            // check for presence of required  attribute
        if ( m_defaultRepeatPresent) {
            xml.append(" DefaultRepeat=\"" + QString::number( m_defaultRepeat ) + endAttr);
        } else { // required attribute not present
            m_lastError = "DefaultRepeat not set";
            m_store  = QString::null;
            return m_store;
        }
        if(bWriteTag) xml.append("/>\n");
        m_store = xml;
        m_changed = false;
    }
    return m_store;
}

// Get String Representation
QString Sample::toString() const{

    return toString("");
}

// Get String Representation with a lead
QString Sample::toString(QString lead, QString xmlTag) const{

    const static QString endAttr( "\n" );
    QString str = lead + xmlTag;
    str.append( lead + "Title=" + m_title + endAttr);
    str.append( lead + "Desc=" + m_desc + endAttr);
    str.append( lead + "Data=" + m_data + endAttr);
    str.append( lead + "MinRepeat=" + QString::number( m_minRepeat ) + endAttr);
    str.append( lead + "MaxRepeat=" + QString::number( m_maxRepeat ) + endAttr);
    str.append( lead + "DefaultRepeat=" + QString::number( m_defaultRepeat ) + endAttr);
    return str;
}

const QString& Sample::lastError() const {
    return m_lastError;
}

const bool& Sample::changed() const {
    return m_changed;
}

const QString& Sample::store() const {
    return m_store;
}


} //end ns
