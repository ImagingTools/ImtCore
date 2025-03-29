#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtrest/IJsonRepresentationController.h>
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

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private Q_SLOTS:
	void OnSettingsChanged(QString jsonData);

private:
	I_REF(iprm::IParamsSet, m_settingsCompPtr);
	I_REF(imtrest::IJsonRepresentationController, m_settingsRepresentationControllerCompPtr);
	I_REF(imtqml::IQuickObject, m_quickObjectCompPtr);
};


} // namespace imtqml


