#pragma once


// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>


namespace imtlog
{


class CMessageGroupInfoProviderComp: virtual public IMessageGroupInfoProvider
{
public:
	// reimplemented (IMessageGroupInfoProvider)
	virtual GroupInfo GetMessageGroupInfo(const istd::IInformationProvider* messagePtr) const;
};


} // namespace imtlog


