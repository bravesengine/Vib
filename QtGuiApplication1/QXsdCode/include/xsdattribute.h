/*
 *  XSDAttribute.h
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

#ifndef __XSDATTR_H__
#define __XSDATTR_H__

#include <iostream>
#include <QtCore>
namespace qXsd2Code
{
	class XSDAttribute : public QObject {
		Q_OBJECT

	public:
		XSDAttribute(QString name, QString type, bool required, QString fixed);
		QString name();

		void setType(QString type);
		QString type();

		void setFixed(QString val);
		bool isFixed();
		QString fixed();

		void setRequired(bool req);
		bool required();

		void setDocumentation(QString doc);
		QString doc();

		void setElement(bool req);
		bool isElement();
		bool isScalar();
		bool isSimpleElement();

		void appendEnumeration(QString enum1);
		QVector<QString> enumeration();

		void setMinOccurs(int min);
		bool hasMinOccurs();
		int minOccurs();

		void setMaxOccurs(int max);
		bool hasMaxOccurs();
		int maxOccurs();

		void setUnbounded();
		bool isUnbounded();

		void setMinLength(int length);
		bool hasMinLength();
		int minLength();

		void setMaxLength(int length);
		bool hasMaxLength();
		int maxLength();

		void setDigits(int length);
		int digits();
		bool hasDigits();

		void setMinExclusive(double min);
		bool hasMinExclusive();
		double minExclusive();

		void setMaxExclusive(double max);
		bool hasMaxExclusive();
		double maxExclusive();

		void setMinInclusive(double min);
		bool hasMinInclusive();
		double minInclusive();

		void setMaxInclusive(double max);
		bool hasMaxInclusive();
		double maxInclusive();

		void dump();

	private:
		QString m_name;
		QString m_type;
		bool m_isFixed;
		QString m_fixed;
		bool m_required;
		QString m_doc;
		bool m_element;
		QVector<QString> m_enums;
		bool m_hasMinOccurs;
		int m_minOccurs;
		bool m_hasMaxOccurs;
		int m_maxOccurs;
		bool m_unbounded;
		bool m_hasMinLength;
		int m_minLength;
		bool m_hasMaxLength;
		int m_maxLength;
		bool m_hasDigits;
		int m_digits;
		bool m_hasMinExclusive;
		double m_minExclusive;
		bool m_hasMaxExclusive;
		double m_maxExclusive;
		bool m_hasMinInclusive;
		double m_minInclusive;
		bool m_hasMaxInclusive;
		double m_maxInclusive;
	};
}
#endif
