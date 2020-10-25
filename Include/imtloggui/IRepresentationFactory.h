#pragma once


// Qt includes
#include <QtCore/QDateTime.h>

// ACF Includes
#include <istd/TSmartPtr.h>
#include <ilog/IMessageContainer.h>

// ImtCore includes
#include <imtlog/CTimeRange.h>


namespace imtloggui
{


class IRepresentationFactory: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSmartPtr<istd::IChangeable> RepresentationObjectPtr;

	virtual RepresentationObjectPtr CreateRepresentationObject(const imtlog::CTimeRange& timeRange, const QList<int>& messageIdFilter) const = 0;
};


} // namespace imtloggui


