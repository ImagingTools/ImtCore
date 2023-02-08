#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IApplicationInfoRepresentation.h>
#include <imtbase/CTreeItemModel.h>
#include <imtbase/IRepresentationController.h>
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
		I_ASSIGN(m_prefixServer, "ServerPrefix", "Prefix Server", true, "/");
		I_ASSIGN(m_applicationInfoRepresentationCompPtr, "ApplicationInfoRepresentation", "Application info representation", true, "ApplicationInfoRepresentation");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
	I_END_COMPONENT;

	CObserverQmlComp();

protected:
	void ApplyUrl() const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnChangeSourceItem(QString src);
	void OnGuiChanged();

private:
	I_ATTR(QByteArray, m_prefixServer);
	I_REF(iprm::IParamsSet, m_settingsCompPtr);
	I_REF(imtbase::IRepresentationController, m_settingsRepresentationControllerCompPtr);
	I_REF(imtqml::IQuickObject, m_quickObjectCompPtr);
	I_REF(imtbase::IApplicationInfoRepresentation, m_applicationInfoRepresentationCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);

private:
	imtbase::CTreeItemModel* m_settingsModelPtr;
	imtbase::CTreeItemModel* m_applicationInfoModelPtr;
};


} // namespace imtqml


