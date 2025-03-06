#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>
#include <iprm/IEnableableParam.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>
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
		I_ASSIGN(m_urlParamPtr, "BaseUrlParam", "Server URL param", false, "BaseUrlParam");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
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
	void OnUrlParamChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IUrlParam* urlParamPtr);
	virtual void SetBaseUrl(QQmlEngine& qmlEngine) const;

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
	I_REF(imtbase::IUrlParam, m_urlParamPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);

	imtbase::TModelUpdateBinder<imtbase::IUrlParam, CGqlQuickObjectComp> m_urlParamObserver;
};


} // namespace imtqml


