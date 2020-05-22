#include <imtlog/CMessageGroupInfoProviderComp.h>


// Qt includes
#include <QtCore/QObject>


namespace imtlog
{


// public methods

// reimplemented (IMessageGroupInfoProvider)

IMessageGroupInfoProvider::GroupInfo CMessageGroupInfoProviderComp::GetMessageGroupInfo(const istd::IInformationProvider* messagePtr) const
{
	GroupInfo groupInfo;

	switch (messagePtr->GetInformationId()){
	case MI_USER_LOGIN:
	case MI_USER_LOGOUT:
		groupInfo.id = "UserAction";
		groupInfo.description = QObject::tr("User action event group");
		groupInfo.name = QObject::tr("User Action");
		break;
	default:
		groupInfo.id = "General";
		groupInfo.description = QObject::tr("General event group");
		groupInfo.name = QObject::tr("General");
	}
	
	return groupInfo;
}

IMessageGroupInfoProvider::GroupInfos CMessageGroupInfoProviderComp::GetMessageGroupInfos() const
{
	IMessageGroupInfoProvider::GroupInfos groupInfos;
	GroupInfo groupInfo;

	groupInfo.id = "UserAction";
	groupInfo.description = QObject::tr("User action event group");
	groupInfo.name = QObject::tr("User Action");
	groupInfos.append(groupInfo);

	groupInfo.id = "General";
	groupInfo.description = QObject::tr("General event group");
	groupInfo.name = QObject::tr("General");
	groupInfos.append(groupInfo);

	return groupInfos;
}


} // namespace imtlog


