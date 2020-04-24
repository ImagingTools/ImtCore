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

	enum MessageId
	{
		MI_APPLICATION_START = 1000,
		MI_APPLICATION_SHUTDOWN,
		MI_USER_LOGIN,
		MI_USER_LOGOUT,
		MI_PRODUCING_PART,
		MI_CHANGES_PRODUCTION_PARAMETERS,
		MI_CHANGES_HARDWARE_PARAMETERS
	};

	virtual GroupInfo GetMessageGroupInfo(const istd::IInformationProvider* messagePtr) const = 0;
};


} // namespace imtbase


