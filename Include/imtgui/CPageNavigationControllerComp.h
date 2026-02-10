// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TSingleModelObserverBase.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtgui/IPageNavigationController.h>


namespace imtgui
{


class CPageNavigationControllerComp:
			public icomp::CComponentBase,
			protected imod::TSingleModelObserverBase<iprm::ISelectionParam>,
			virtual public IPageNavigationController
			
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::TSingleModelObserverBase<iprm::ISelectionParam> BaseClass2;

	I_BEGIN_COMPONENT(CPageNavigationControllerComp);
		I_REGISTER_INTERFACE(IPageNavigationController);
		I_ASSIGN(m_pageSelectionCompPtr, "PageModel", "Page selection model", true, "PageModel");
		I_ASSIGN_TO(m_pageSelectionModelCompPtr, m_pageSelectionCompPtr, true);
	I_END_COMPONENT;
	
	CPageNavigationControllerComp();

	// reimplemented (IPageNavigationController)
	virtual bool IsNextPageAvailable() const override;
	virtual bool IsPreviousPageAvailable() const override;
	virtual void GoForward() override;
	virtual void GoBack() override;

protected:
	virtual void AddPageStep(const QByteArray& pageId);
	void OnPageSelectionChanged();
	void GoToPage(const QByteArray& pageId);
	void CreatePageList(const iprm::ISelectionParam& selection, const QByteArray& parentId);

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);

	class PageSubselectionObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		explicit PageSubselectionObserver(CPageNavigationControllerComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CPageNavigationControllerComp& m_parent;
	};

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	QByteArrayList m_visitedPages;
	QByteArrayList m_pages;
	qsizetype m_currentPageIndex;

	I_REF(iprm::ISelectionParam, m_pageSelectionCompPtr);
	I_REF(imod::IModel, m_pageSelectionModelCompPtr);


	PageSubselectionObserver m_pageSubselectionObserver;
	int m_subselectionModelIndex;

	bool m_isUpdateBlocked;
};


} // namespace imtgui


