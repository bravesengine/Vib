/*
 *  Handler.h
 *
 *  schema2code is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Foobar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  Created by Lukassen on 11/06/08.
 *  Copyright 2008
 *
 */

#ifndef __HANDLER_H__
#define __HANDLER_H__

//#include <qxml.h>
#include <QXmlDefaultHandler>
#include <QtCore>
#include <iostream>
#include "xsdobject.h"
namespace qXsd2Code
{
	class Handler : public QXmlDefaultHandler {

	public:
		Handler() { };

		bool startDocument();
		bool startElement(const QString & namespaceURI,
			const QString & localName,
			const QString & qName,
			const QXmlAttributes & atts);

		bool characters(const QString & ch);
		bool endElement(const QString & namespaceURI,
			const QString & localName,
			const QString & qName);
		bool endDocument();
		bool error(const QXmlParseException & exception);

		QVector<XSDObject*> objects();
		void handleStartOfElement(QString qName, QXmlAttributes atts, bool isTypeDef);
		QString stripNameSpace(QString tag);

	private:
		QStack<XSDObject*> m_objStack;
		QStack<XSDAttribute*>m_attrStack;
		QVector<XSDObject*> m_objects;
		QString m_doc;
	};
}
#endif
