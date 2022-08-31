#pragma once


//Qt includes


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtqml/IQuickObject.h>
#include <imtbase/CTreeItemModel.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtgql/IGqlMutationDataControllerDelegate.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>

#include <QTimer>


namespace imtqml
{


class CObserverQmlComp: public QObject, public ilog::CLoggerComponentBase
{
    Q_OBJECT
public:
		typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CObserverQmlComp);
		I_ASSIGN(m_pagesDataProviderCompPtr, "PagesDataProviderCompPtr", "List of pages providers for join", false, "");
		I_ASSIGN(m_mutationDataDelegateCompPtr, "MutationDataDelegate", "Mutation data delegate", false, "");
		I_ASSIGN(m_quickObjectComp, "QuickObject", "Main QML Component", true, "QuickObject");
		I_ASSIGN(m_prefixServer, "ServerPrefix", "Prefix Server", true, "/Lisa");
	I_END_COMPONENT;

	CObserverQmlComp();

// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	void OnSettingsUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::CTreeItemModel* settingsModelPtr);
	void ApplyUrl(const imtbase::CTreeItemModel* settingsModelPtr);
private Q_SLOTS:
	void OnChangeSourceItem(QString src);
	void OnModelChanged();

private:
	I_REF(imtqml::IQuickObject, m_quickObjectComp);

	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_pagesDataProviderCompPtr);
	I_REF(imtgql::IGqlMutationDataControllerDelegate, m_mutationDataDelegateCompPtr);
	I_ATTR(QByteArray, m_prefixServer);

	imtbase::CTreeItemModel* m_settingsModelPtr;
	imtbase::TModelUpdateBinder<imtbase::CTreeItemModel, CObserverQmlComp> m_settingsObserver;
};


} // namespace imtqml
