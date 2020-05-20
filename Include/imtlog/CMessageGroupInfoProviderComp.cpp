#include <imtlog/CMessageGroupInfoProviderComp.h>


namespace imtlog
{


IMessageGroupInfoProvider::GroupInfo CMessageGroupInfoProviderComp::GetMessageGroupInfo(const istd::IInformationProvider* messagePtr) const
{
	GroupInfo groupInfo;

	switch (messagePtr->GetInformationId()){
	case MI_USER_LOGIN:
	case MI_USER_LOGOUT:
		groupInfo.id = "UserAction";
		groupInfo.description = "UserAction";
		groupInfo.name = "UserAction";
		break;
	default:
		groupInfo.id = "General";
		groupInfo.description = "General group";
		groupInfo.name = "General";
	}
	
	return groupInfo;
}


} // namespace imtlog


