#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtgql/IItemBasedRepresentationDataProvider.h>
#include <imtqml/IQuickObject.h>
#include <imtgql/IGqlModelEditor.h>


namespace imtqml
{


class CObserverQmlComp: public QObject, public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CObserverQmlComp);
		I_ASSIGN(m_pagesDataProviderCompPtr, "PagesDataProviderCompPtr", "List of pages providers for join", false, "");
		I_ASSIGN(m_languageParameterCompPtr, "LanguageParameter", "Language parameter", false, "");
		I_ASSIGN(m_mutationDataDelegateCompPtr, "MutationDataDelegate", "Mutation data delegate", false, "");
		I_ASSIGN(m_quickObjectComp, "QuickObject", "Main QML Component", true, "QuickObject");
		I_ASSIGN(m_prefixServer, "ServerPrefix", "Prefix Server", true, "/");
	I_END_COMPONENT;

	CObserverQmlComp();

protected:
	void OnSettingsUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::CTreeItemModel* settingsModelPtr);
	void ApplyUrl() const;
	const imtbase::CTreeItemModel* GetPageModel(const QByteArray& pageId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnChangeSourceItem(QString src);
	void OnModelChanged(const QString& page);

private:
	I_REF(imtqml::IQuickObject, m_quickObjectComp);
	I_REF(iser::ISerializable, m_languageParameterCompPtr);
	I_REF(imtgql::IItemBasedRepresentationDataProvider, m_pagesDataProviderCompPtr);
	I_REF(imtgql::IGqlModelEditor, m_mutationDataDelegateCompPtr);
	I_ATTR(QByteArray, m_prefixServer);

private:
	imtbase::CTreeItemModel* m_settingsModelPtr;
	imtbase::TModelUpdateBinder<imtbase::CTreeItemModel, CObserverQmlComp> m_settingsObserver;
};


} // namespace imtqml


