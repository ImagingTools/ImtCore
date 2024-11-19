#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>
#include <imtbase/CTreeItemModel.h>
#include <imtrest/IRepresentationController.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtqml/IQuickObject.h>


namespace imtqml
{


class CObserverQmlComp: public QObject, public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CObserverQmlComp);
		I_ASSIGN(m_settingsRepresentationControllerCompPtr, "SettingsRepresentationController", "Controller for representation settings", true, "");
		I_ASSIGN(m_settingsCompPtr, "Settings", "Settings", false, "Settings");
		I_ASSIGN(m_quickObjectCompPtr, "QuickObject", "Main QML Component", true, "QuickObject");
	I_END_COMPONENT;

	CObserverQmlComp();

protected:
	void UpdateSettingsRepresentation();

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnChangeSourceItem(QString src);
	void OnGuiChanged();

private:
	I_REF(iprm::IParamsSet, m_settingsCompPtr);
	I_REF(imtrest::IRepresentationController, m_settingsRepresentationControllerCompPtr);
	I_REF(imtqml::IQuickObject, m_quickObjectCompPtr);

private:
	imtbase::CTreeItemModel m_settingsModel;
	imtbase::CTreeItemModel m_applicationInfoModel;

	imtbase::TModelUpdateBinder<iprm::IParamsSet, CObserverQmlComp> m_settingsObserver;
};


} // namespace imtqml


