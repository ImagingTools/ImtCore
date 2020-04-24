#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <istd/IInformationProvider.h>


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

	virtual GroupInfo GetMessageGroupInfo(const istd::IInformationProvider* messagePtr) const = 0;
};


} // namespace imtbase


