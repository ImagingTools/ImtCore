#pragma once


// ImtCore includes
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtqml
{


class CCommandDataProviderComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CCommandDataProviderComp);
		I_ASSIGN_MULTI_0(m_commandsIdsAttrPtr, "CommandsIds", "Commands Ids", true);
		I_ASSIGN_MULTI_0(m_commandsNamesAttrPtr, "CommandsNames", "Commands names", true);
		I_ASSIGN_MULTI_0(m_commandsDefaultModesAttrPtr, "CommandsDefaultModes", "Default commands modes", true);
		I_ASSIGN_MULTI_0(m_commandsDefaultStatusIconAttrPtr, "CommandsDefaultStatusIcon", "Path of status icon used by default", false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_MULTIATTR(QString, m_commandsIdsAttrPtr);
	I_MULTITEXTATTR(m_commandsNamesAttrPtr);
	I_MULTIATTR(QString, m_commandsDefaultModesAttrPtr);
	I_MULTIATTR(QString, m_commandsDefaultStatusIconAttrPtr);
};


} // namespace imtqml


