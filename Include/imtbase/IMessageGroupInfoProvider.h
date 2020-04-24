#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


class IMessageGroupInfoProvider: virtual public istd::IChangeable
{
public:
	struct GroupInfo
	{
		QString name;
		QString description;
		QByteArray groupId;
	};

	virtual GroupInfo GetMessageGroupInfo(int messageId) const = 0;
};


} // namespace imtbase


