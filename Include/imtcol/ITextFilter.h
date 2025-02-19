#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtcol
{


class ITextFilter: virtual public iser::ISerializable
{
public:
	virtual QByteArrayList GetFilteringFieldIds() const = 0;
	virtual void SetFilteringFieldIds(const QByteArrayList& fieldIds) = 0;
	virtual QString GetFilterText() const = 0;
	virtual void SetFilterText(const QString& text) = 0;
};


} // namespace imtcol


