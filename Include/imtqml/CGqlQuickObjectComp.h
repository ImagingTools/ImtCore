// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IEnableableParam.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/IApplicationInfoController.h>
#include <imtcom/IServerConnectionInterface.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtqml/CQuickObjectCompBase.h>


namespace imtqml
{


/**
	Component wrapper for the QML object.
 */
class CGqlQuickObjectComp: public QObject, public CQuickObjectCompBase
{
	Q_OBJECT
public:
	typedef CQuickObjectCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlQuickObjectComp);
		I_REGISTER_SUBELEMENT(QuickItemCreated);
		I_REGISTER_SUBELEMENT_INTERFACE(QuickItemCreated, iprm::IEnableableParam, ExtractEnableableParam);
		I_REGISTER_SUBELEMENT_INTERFACE(QuickItemCreated, imod::IModel, ExtractEnableableParam);
		I_REGISTER_SUBELEMENT_INTERFACE(QuickItemCreated, istd::IChangeable, ExtractEnableableParam);
		I_ASSIGN(m_baseUrlAttrPtr, "BaseUrl", "BaseUrl for AccessManager", false, "");
		I_ASSIGN(m_serverConnectionParamPtr, "ServerConnectionParam", "ServerConnectionParam", false, "ServerConnectionParam");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfoController", "Application info controller", true, "ApplicationInfo");
	I_END_COMPONENT;

	CGqlQuickObjectComp();

protected:
	// reimplemented (CQuickObjectCompBase)
	virtual bool InitializeEngine(QQmlEngine& qmlEngine) const override;
	virtual void OnItemCreated() override;
	virtual void OnItemDestroyed() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void OnServerConnectionParamChanged(const istd::IChangeable::ChangeSet& changeSet, const imtcom::IServerConnectionInterface* serverConnectionParamPtr);
	void OnApplicationInfoChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IApplicationInfoController* applicationInfoControllerPtr);
	virtual void SetBaseUrl() const;

private:
	class QuickItemCreated : public iprm::IEnableableParam
	{
	public:
		QuickItemCreated();

		void SetQuickItemCreated(bool isCreated);

		// reimplemented (iprm::IEnableableParam)
		virtual bool IsEnabled() const;
		virtual bool IsEnablingAllowed() const;
		virtual bool SetEnabled(bool isEnabled = true);

		// reimplemented (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive);

	private:
		bool m_isCreated;
	};

	// static template methods for sub element access
	template <class InterfaceType>
	static InterfaceType* ExtractEnableableParam(CGqlQuickObjectComp& component)
	{
		return &component.m_itemCreated;
	}

protected:
	imod::TModelWrap<QuickItemCreated> m_itemCreated;

protected:
	I_ATTR(QString, m_baseUrlAttrPtr);
	I_MULTIATTR(QByteArray, m_modelIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_modelQueriesAttrPtr);
	I_REF(imtcom::IServerConnectionInterface, m_serverConnectionParamPtr);
	I_REF(imtbase::IApplicationInfoController, m_applicationInfoCompPtr);

	imtbase::TModelUpdateBinder<imtcom::IServerConnectionInterface, CGqlQuickObjectComp> m_serverConnectionObserver;
	imtbase::TModelUpdateBinder<imtbase::IApplicationInfoController, CGqlQuickObjectComp> m_applicationInfoObserver;
};


} // namespace imtqml


