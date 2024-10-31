#pragma once

// Qt includes
#include <QtCore/QtGlobal>
#include <QtQuick/QQuickItem>

// ACF includes
#include <ibase/IApplicationInfo.h>
#include <icomp/CComponentBase.h>
#include <iprm/IEnableableParam.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>
#include <imtbase/CTreeItemModel.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtqml/IQuickObject.h>


namespace imtqml
{


/**
	Component wrapper for the QML object.
 */
class CQuickObjectCompBase:
			public QObject,
			public icomp::CComponentBase,
			virtual public imtqml::IQuickObject
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CQuickObjectCompBase);
		I_REGISTER_INTERFACE(imtqml::IQuickObject);
		I_REGISTER_SUBELEMENT(QuickItemCreated);
		I_REGISTER_SUBELEMENT_INTERFACE(QuickItemCreated, iprm::IEnableableParam, ExtractEnableableParam);
		I_REGISTER_SUBELEMENT_INTERFACE(QuickItemCreated, imod::IModel, ExtractEnableableParam);
		I_REGISTER_SUBELEMENT_INTERFACE(QuickItemCreated, istd::IChangeable, ExtractEnableableParam);
		I_ASSIGN(m_pathToQmlAttrPtr, "QmlFilePath", "This path used for load QML file", true, GetPathToQml());
		I_ASSIGN(m_baseUrlAttrPtr, "BaseUrl", "BaseUrl for AccessManager", false, "");
		I_ASSIGN(m_urlParamPtr, "BaseUrlParam", "Server URL param", false, "BaseUrlParam");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
		I_ASSIGN_MULTI_0(m_modelIdsAttrPtr, "ModelIdsAttr", "If enabled, this Id's used for register models", false);
		I_ASSIGN_MULTI_0(m_modelQueriesAttrPtr, "ModelQueries", "If enabled, this Queries used for get datas", false);
	I_END_COMPONENT;

	CQuickObjectCompBase();

	static QString GetPathToQml();
	void OnUrlParamChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IUrlParam* urlParamPtr);
	virtual void SetBaseUrl(QQmlEngine& qmlEngine) const;

	// reimplemented (imtgui::IQuickObject)
	virtual bool IsItemCreated() const override;
	virtual bool CreateQuickItem(QQuickItem* parentPtr) override;
	virtual bool DestroyQuickItem() override;
	virtual QQuickItem* GetQuickItem() const override;
	virtual void OnTryClose(bool* ignoredPtr = nullptr) override;

protected:
	/**
		Internal creation of the quick item using QML-engine.
	*/
	virtual QQuickItem* CreateItem(QQmlEngine* enginePtr) const;

	/**
		Internal creation of the quick item using QML-engine using some properties for the engine initialization.
	*/
	virtual QQuickItem* CreateItem(QQmlEngine* enginePtr, const QVariantMap& initialProperties) const;

	/**
		Callback method after the quick item was created.
		Default implementation does nothing.
	*/
	virtual void OnItemCreated();

	/**
		Callback method before the quick item will be destroyed.
		Default implementation does nothing.
	*/
	virtual void OnItemDestroyed();

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

public Q_SLOTS:
	void OnModelNeedsReload(imtbase::CTreeItemModel *itemModelPtr = nullptr);

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
	static InterfaceType* ExtractEnableableParam(CQuickObjectCompBase& component)
	{
		return &component.m_itemCreated;
	}

protected:
	imod::TModelWrap<QuickItemCreated> m_itemCreated;

	QQuickItem* m_quickItemPtr;
	QList<imtbase::CTreeItemModel*> m_models;

protected:
	I_ATTR(QString, m_pathToQmlAttrPtr);
	I_ATTR(QString, m_baseUrlAttrPtr);
	I_MULTIATTR(QByteArray, m_modelIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_modelQueriesAttrPtr);
	I_REF(imtbase::IUrlParam, m_urlParamPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);

	imtbase::TModelUpdateBinder<imtbase::IUrlParam, CQuickObjectCompBase> m_urlParamObserver;
};


} // namespace imtqml


