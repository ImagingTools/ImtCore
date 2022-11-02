#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtqml/CCommandDataEnumProviderComp.h>
#include <imtgql/CUserOptionsProviderBase.h>


namespace imtqml
{


class CCommandDataProviderCompBase:
		public icomp::CComponentBase,
		public imtgql::CUserOptionsProviderBase,
		public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtqml::CCommandDataEnumProviderComp CommandEnum;

	I_BEGIN_COMPONENT(CCommandDataProviderCompBase);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_commandsModelIdAttrPtr, "CommandsModelId", "Command model-ID", true, "");
		I_ASSIGN_MULTI_0(m_commandsIdsAttrPtr, "CommandsIds", "Commands Ids", true);
		I_ASSIGN_MULTI_0(m_commandsNamesAttrPtr, "CommandsNames", "Commands names", true);
		I_ASSIGN_MULTI_0(m_commandsDefaultModesAttrPtr, "CommandsDefaultModes", "Default commands modes", true);
		I_ASSIGN_MULTI_0(m_commandsDefaultStatusIconAttrPtr, "CommandsDefaultStatusIcon", "Path of status icon used by default", false);
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params, const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

private:
	I_ATTR(QByteArray, m_commandsModelIdAttrPtr);
	I_MULTIATTR(QString, m_commandsIdsAttrPtr);
	I_MULTITEXTATTR(m_commandsNamesAttrPtr);
	I_MULTIATTR(QString, m_commandsDefaultModesAttrPtr);
	I_MULTIATTR(QString, m_commandsDefaultStatusIconAttrPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtqml


