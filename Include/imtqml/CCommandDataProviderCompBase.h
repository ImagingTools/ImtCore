#pragma once

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtqml/CCommandDataEnumProviderComp.h>


namespace imtqml
{


/**
	Command.
	\ingroup LicenseManagement
*/
class CCommandDataProviderCompBase:
		public icomp::CComponentBase,
		public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::IItemBasedRepresentationDataProvider BaseClass2;
	typedef imtqml::CCommandDataEnumProviderComp CommandEnum;

	I_BEGIN_COMPONENT(CCommandDataProviderCompBase);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_commandsModelIdAttrPtr, "CommandsModelId", "Command model-ID", true, "");
		I_ASSIGN_MULTI_0(m_commandIdAttrPtr, "CommandId", "Command Id", true);
		I_ASSIGN_MULTI_0(m_commandNameAttrPtr, "CommandName", "Command name", true);
		I_ASSIGN_MULTI_0(m_commandDefaultStatusIcon, "CommandDefaultStatusIcon", "Path of status icon used by default", false);
	I_END_COMPONENT;

	CCommandDataProviderCompBase();

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields) override;

private:
	I_ATTR(QByteArray, m_commandsModelIdAttrPtr);
	I_MULTIATTR(QString, m_commandIdAttrPtr);
	I_MULTITEXTATTR(m_commandNameAttrPtr);
	I_MULTIATTR(QString, m_commandDefaultStatusIcon);

};


} // namespace imtqml


