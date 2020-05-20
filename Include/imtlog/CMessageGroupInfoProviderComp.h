#pragma once


// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>


namespace imtlog
{


class CMessageGroupInfoProviderComp: public IMessageGroupInfoProvider
{
public:
	virtual GroupInfo GetMessageGroupInfo(const istd::IInformationProvider* messagePtr) const;
};


} // namespace imtlog


