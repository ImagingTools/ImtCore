#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcol
{


class ICollectionHeadersProvider: virtual public istd::IPolymorphic
{
public:
	struct HeaderInfo
	{
		QByteArray headerId;
		QString headerName;
		bool sortable = true;
		bool filterable = true;
		QByteArray permissionId;

		bool operator == (const HeaderInfo& other) const
		{
			return (headerId == other.headerId) &&
					(headerName == other.headerName) &&
					(sortable == other.sortable) &&
					(filterable == other.filterable) &&
					(permissionId == other.permissionId) ;
		}
	};

	typedef QByteArrayList HeaderIds;

	virtual HeaderIds GetHeaderIds() const = 0;
	virtual bool GetHeaderInfo(const QByteArray& headerId, HeaderInfo& headerInfo) const = 0;
};


} // namespace imtcol


