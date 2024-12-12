#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


class ITableViewParam: virtual public iser::ISerializable
{
public:
	struct HeaderInfo
	{
		QByteArray headerId;
		double size = 0;
		bool visible = false;
		int order = 0;

		bool operator == (const HeaderInfo& other) const
		{
			return (headerId == other.headerId) &&
					(size == other.size) &&
					(order == other.order) &&
					(visible == other.visible);
		}
	};

	virtual QByteArrayList GetHeaderIds() const = 0;
	virtual HeaderInfo GetHeaderInfo(const QByteArray& headerId) const = 0;
	virtual bool SetHeaderInfo(QByteArray headerId, ITableViewParam::HeaderInfo headerInfo) = 0;
};


} // namespace imtbase


