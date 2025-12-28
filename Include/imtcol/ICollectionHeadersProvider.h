#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcol
{


class ICollectionHeadersProvider: virtual public istd::IPolymorphic
{
public:
	enum FieldType
	{
		FT_SCALAR,
		FT_ARRAY
	};
	I_DECLARE_ENUM(FieldType, FT_SCALAR, FT_ARRAY);

	struct HeaderInfo
	{
		QByteArray headerId;
		QString headerName;
		bool sortable = true;
		bool filterable = true;
		QByteArray permissionId;
		FieldType fieldType = FT_SCALAR;

		bool operator == (const HeaderInfo& other) const
		{
			return (headerId == other.headerId) &&
					(fieldType == other.fieldType) &&
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


