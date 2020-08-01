#include <imtgui/CPageNavigationControllerComp.h>


// ACF includes
#include <istd/CIdManipBase.h>
#include <iprm/IOptionsList.h>


namespace imtgui
{


// public methods

CPageNavigationControllerComp::CPageNavigationControllerComp()
	:m_currentPageIndex(-1),
	m_isUpdateBlocked(false),
	m_pageSubselectionObserver(*this),
	m_subselectionModelIndex(0)
{
}


// reimplemented (IPageNavigationController)

bool CPageNavigationControllerComp::IsNextPageAvailable() const
{
	return m_currentPageIndex < (m_visitedPages.count() - 1);
}


bool CPageNavigationControllerComp::IsPreviousPageAvailable() const
{
	return (m_visitedPages.count() > 1) && (m_currentPageIndex > 0);
}


void CPageNavigationControllerComp::GoForward()
{
	istd::CChangeNotifier changeNotifier(this);

	Q_ASSERT(m_currentPageIndex + 1 < m_visitedPages.count());

	GoToPage(m_visitedPages[++m_currentPageIndex]);
}


void CPageNavigationControllerComp::GoBack()
{
	istd::CChangeNotifier changeNotifier(this);

	Q_ASSERT(m_currentPageIndex - 1 >= 0);

	GoToPage(m_visitedPages[--m_currentPageIndex]);
}


// protected methods

void CPageNavigationControllerComp::AddPageStep(const QByteArray& pageId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_visitedPages.push_back(pageId);

	m_currentPageIndex = m_visitedPages.count() - 1;
}


void CPageNavigationControllerComp::OnPageSelectionChanged()
{
	if (m_isUpdateBlocked){
		return;
	}

	Q_ASSERT(m_pageSelectionCompPtr.IsValid());

	QByteArray selectedPageId;

	int selectedIndex = m_pageSelectionCompPtr->GetSelectedOptionIndex();
	if (selectedIndex >= 0){
		const iprm::IOptionsList* pageListPtr = m_pageSelectionCompPtr->GetSelectionConstraints();
		if (pageListPtr != nullptr){
			selectedPageId = pageListPtr->GetOptionId(selectedIndex);

			const iprm::ISelectionParam* subSelectionPtr = m_pageSelectionCompPtr->GetSubselection(selectedIndex);
			if (subSelectionPtr != nullptr){
				int subPageIndex = subSelectionPtr->GetSelectedOptionIndex();
				if (subPageIndex >= 0) {
					const iprm::IOptionsList* subPageListPtr = subSelectionPtr->GetSelectionConstraints();
					if (subPageListPtr != nullptr) {
						QByteArray subPageId = subPageListPtr->GetOptionId(subPageIndex);

						selectedPageId += "/" + subPageId;
					}
				}
			}
		}
	}

	if (m_pages.contains(selectedPageId)){
		if (!m_visitedPages.isEmpty()){
			QByteArray lastPageId = m_visitedPages.last();
			if (selectedPageId != lastPageId){
				AddPageStep(selectedPageId);
			}
		}
		else{
			AddPageStep(selectedPageId);
		}
	}
}


void CPageNavigationControllerComp::GoToPage(const QByteArray & pageId)
{
	m_isUpdateBlocked = true;

	QByteArray baseId;
	QByteArray subId;
	istd::CIdManipBase::SplitId(pageId, baseId, subId);

	int index = iprm::FindOptionIndexById(baseId, *m_pageSelectionCompPtr->GetSelectionConstraints());
	
	m_pageSelectionCompPtr->SetSelectedOptionIndex(index);

	if (!subId.isEmpty()){
		iprm::ISelectionParam* subSelectionPtr = m_pageSelectionCompPtr->GetSubselection(index);
		if (subSelectionPtr != nullptr){
			int subIndex = iprm::FindOptionIndexById(subId, *subSelectionPtr->GetSelectionConstraints());

			subSelectionPtr->SetSelectedOptionIndex(subIndex);
		}
	}

	m_isUpdateBlocked = false;
}


void CPageNavigationControllerComp::CreatePageList(const iprm::ISelectionParam& selection, const QByteArray& parentId)
{
	const iprm::IOptionsList* pageListPtr = selection.GetSelectionConstraints();
	if (pageListPtr != nullptr){
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex){
			QByteArray pageId = pageListPtr->GetOptionId(pageIndex);
			const iprm::ISelectionParam* subSelectionPtr = selection.GetSubselection(pageIndex);

			if (parentId.isEmpty()){
				if ((subSelectionPtr == nullptr)){
					m_pages.push_back(pageId);
				}
			}
			else{
				QByteArray composedId = parentId + "/" + pageId;

				m_pages.push_back(composedId);
			}

			if (subSelectionPtr != nullptr){
				const imod::IModel* constModelPtr = dynamic_cast<const imod::IModel*>(subSelectionPtr);
				imod::IModel* modelPtr = const_cast<imod::IModel*>(constModelPtr);
				if (modelPtr != nullptr){
					m_pageSubselectionObserver.RegisterModel(modelPtr, m_subselectionModelIndex++);
				}

				CreatePageList(*subSelectionPtr, pageId);
			}
		}
	}
}

// reimplemented (imod::CSingleModelObserverBase)

void CPageNavigationControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	OnPageSelectionChanged();
}


// reimplemented (icomp::CComponentBase)

void CPageNavigationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_isUpdateBlocked = true;

	if (m_pageSelectionModelCompPtr.IsValid() && m_pageSelectionCompPtr.IsValid()){
		m_pageSelectionModelCompPtr->AttachObserver(this);

		CreatePageList(*m_pageSelectionCompPtr, "");
	}

	m_isUpdateBlocked = false;
}


void CPageNavigationControllerComp::OnComponentDestroyed()
{
	if (m_pageSelectionModelCompPtr.IsValid() && m_pageSelectionCompPtr.IsValid()){
		m_pageSelectionModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}



// protected methods of embedded class PageSubselectionObserver

CPageNavigationControllerComp::PageSubselectionObserver::PageSubselectionObserver(CPageNavigationControllerComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CPageNavigationControllerComp::PageSubselectionObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.OnPageSelectionChanged();
}


} // namespace imtgui


