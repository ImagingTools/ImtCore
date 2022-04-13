#pragma once


//Qt includes


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtqml/IQuickObject.h>
#include <imtbase/CTreeItemModel.h>
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
	I_END_COMPONENT;

// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:

private Q_SLOTS:
	void OnChangeSourceItem(QString src);

private:
    I_REF(imtqml::IQuickObject, m_quickObjectComp);
	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_pagesDataProviderCompPtr);
	I_REF(imtgql::IGqlMutationDataControllerDelegate, m_mutationDataDelegateCompPtr);
};


} // namespace imtqml
