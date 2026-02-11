// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for a object containing URL.
*/
class IMimeType: virtual public istd::IChangeable
{
public:
	typedef QMap<QString, QString> Parameters;

	virtual const QString& GetType() const = 0;
	virtual bool SetType(const QString& type) = 0;
	virtual QStringList GetTree() const = 0;
	virtual bool SetTree(const QStringList& tree) = 0;
	virtual const QString& GetSubType() const = 0;
	virtual bool SetSubType(const QString& subType) = 0;
	virtual const QString& GetSuffix() const = 0;
	virtual bool SetSuffix(const QString& suffix) = 0;
	virtual const Parameters& GetParameters() const = 0;
	virtual bool SetParameters(const Parameters& parameters) = 0;
	virtual QString ToString() = 0;
	virtual bool FromString(const QString& string) = 0;
};


} // namespace imtbase


