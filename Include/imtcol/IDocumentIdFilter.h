// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtcol
{


class IDocumentIdFilter: virtual public iser::ISerializable
{
public:
	enum ConditionType
	{
		CT_IN,
		CT_NOT_IN
	};
	I_DECLARE_ENUM(ConditionType, CT_IN, CT_NOT_IN);

	virtual QByteArrayList GetDocumentIds() const = 0;
	virtual void SetDocumentIds(const QByteArrayList& documentIds) = 0;
	virtual ConditionType GetConditionType() const = 0;
	virtual void SetConditionType(ConditionType type) = 0;
};


} // namespace imtcol


