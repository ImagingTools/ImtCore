// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IIdParam.h>
#include <iauth/ILogin.h>
#include <ibase/ICommandsProvider.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/IModel.h>
#include <imod/TSingleModelObserverBase.h>
#include <iqtgui/TMakeIconProviderCompWrap.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtqml/IQuickObject.h>
#include <imtcom/IServerConnectionInterface.h>


namespace imtauthgui
{


class CAdministrationObserverQmlComp:
			public QObject,
			public iqtgui::TMakeIconProviderCompWrap<ilog::CLoggerComponentBase>,
			public ibase::ICommandsProvider
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAdministrationObserverQmlComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_loginCompPtr, "Login", "Login", false, "Login");
		I_ASSIGN(m_serverConnectionParamCompPtr, "WebSocketUrlParam", "Web socket URL param", false, "WebSocketUrlParam");
		I_ASSIGN(m_quickObjectCompPtr, "QuickObject", "Main QML Component", true, "QuickObject");
		I_ASSIGN(m_quickItemCreatedCompPtr, "QuickItemCreated", "Quick item created", false, "QuickItemCreated");
		I_ASSIGN_TO(m_quickItemCreatedModelCompPtr, m_quickItemCreatedCompPtr, false);
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product-ID", false, "");
	I_END_COMPONENT;

	CAdministrationObserverQmlComp();

protected:
	// reimpemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnCommandsModelChanged(QVariant variant);
	void OnTriggered();

private:
	void OnLoginUpdate(const istd::IChangeable::ChangeSet& changeSet, const iauth::ILogin* objectPtr);
	void OnQuickItemCreatedUpdate(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* objectPtr);
	void OnServerConnectionParamUpdate(const istd::IChangeable::ChangeSet& changeSet, const imtcom::IServerConnectionInterface* objectPtr);

private:
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(imtqml::IQuickObject, m_quickObjectCompPtr);
	I_REF(iprm::IEnableableParam, m_quickItemCreatedCompPtr);
	I_REF(imod::IModel, m_quickItemCreatedModelCompPtr);
	I_REF(imtcom::IServerConnectionInterface, m_serverConnectionParamCompPtr);
	I_ATTR(QByteArray, m_productIdAttrPtr);

	imtbase::TModelUpdateBinder<iauth::ILogin, CAdministrationObserverQmlComp> m_loginObserver;
	imtbase::TModelUpdateBinder<iprm::IEnableableParam, CAdministrationObserverQmlComp> m_quickItemCreatedObserver;
	imtbase::TModelUpdateBinder<imtcom::IServerConnectionInterface, CAdministrationObserverQmlComp> m_serverConnectionParamObserver;
	iqtgui::CHierarchicalCommand m_commandsList;
};


} // namespace imtauthgui


