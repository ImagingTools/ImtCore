#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/IParamsSet.h>
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
        I_ASSIGN(m_settingsCompPtr, "Settings", "Settings", false, "Settings");
		I_ASSIGN(m_mutationDataDelegateCompPtr, "MutationDataDelegate", "Mutation data delegate", false, "");
        I_ASSIGN(m_quickObjectCompPtr, "QuickObject", "Main QML Component", true, "QuickObject");
		I_ASSIGN(m_prefixServer, "ServerPrefix", "Prefix Server", true, "/");
	I_END_COMPONENT;

	CObserverQmlComp();

protected:
	void ApplyUrl() const;
    imtbase::CHierarchicalItemModelPtr CreatePageModel(const QByteArray& pageId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnChangeSourceItem(QString src);
	void OnModelChanged(const QString& page);

private:
    I_REF(imtqml::IQuickObject, m_quickObjectCompPtr);
    I_REF(iprm::IParamsSet, m_settingsCompPtr);
	I_REF(imtgql::IItemBasedRepresentationDataProvider, m_pagesDataProviderCompPtr);
	I_REF(imtgql::IGqlModelEditor, m_mutationDataDelegateCompPtr);
	I_ATTR(QByteArray, m_prefixServer);

private:
	imtbase::CTreeItemModel* m_settingsModelPtr;
};


} // namespace imtqml


