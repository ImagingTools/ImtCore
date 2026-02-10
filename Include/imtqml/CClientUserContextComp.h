// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtQml/QQmlEngine>

// ACF includes
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtqml/IQuickObject.h>


namespace imtqml
{


class CClientUserContextComp: public QObject, public icomp::CComponentBase
{
	Q_OBJECT
	Q_PROPERTY(QString language READ GetLanguage WRITE SetLanguage NOTIFY LanguageChanged)

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CClientUserContextComp);
		I_ASSIGN(m_languageParamPtr, "LanguageParam", "Language selection param", false, "LanguageParam");
		I_ASSIGN(m_quickObjectCompPtr, "QuickObject", "Main QML Component", true, "QuickObject");
		I_ASSIGN(m_quickObjectCreatedCompPtr, "QuickObjectCreated", "Quick object created", true, "QuickObjectCreated");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

	CClientUserContextComp();

Q_SIGNALS:
	void LanguageChanged();

public:
	void OnLanguageChanged(const istd::IChangeable::ChangeSet& changeSet, const iprm::ISelectionParam* objectPtr);
	void OnQuickObjectCreatedChanged(const istd::IChangeable::ChangeSet& changeSet, const iprm::IEnableableParam* objectPtr);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

public:
	void SetLanguage(QString translation);
	QString GetLanguage() const;

private:
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
	I_REF(iprm::ISelectionParam, m_languageParamPtr);
	I_REF(imtqml::IQuickObject, m_quickObjectCompPtr);
	I_REF(iprm::IEnableableParam, m_quickObjectCreatedCompPtr);

	QQmlEngine* m_qmlEngine;
	QString m_language;
	imtbase::TModelUpdateBinder<iprm::ISelectionParam, CClientUserContextComp> m_languageParamObserver;
	imtbase::TModelUpdateBinder<iprm::IEnableableParam, CClientUserContextComp> m_quickObjectCreatedObserver;
};


} // namespace imtqml


