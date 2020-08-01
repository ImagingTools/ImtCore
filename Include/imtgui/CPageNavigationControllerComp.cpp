#include <imtgui/CPageNavigationControllerComp.h>


namespace imtgui
{


// public methods

// reimplemented (IPageNavigationController)

bool CPageNavigationControllerComp::IsNextPageAvailable() const
{
	return m_currentPageIndex < (m_visitedPages.count() - 1);
}


bool CPageNavigationControllerComp::IsPreviousPageAvailable() const
{
	return !m_visitedPages.isEmpty() && (m_currentPageIndex > 0);
}


void CPageNavigationControllerComp::GoForward()
{
	istd::CChangeNotifier changeNotifier(this);

	Q_ASSERT(m_currentPageIndex + 1 < m_visitedPages.count());

	m_currentPageIndex++;
}


void CPageNavigationControllerComp::GoBack()
{
	istd::CChangeNotifier changeNotifier(this);

	Q_ASSERT(m_currentPageIndex - 1 > 0);

	m_currentPageIndex--;
}


// protected methods

void CPageNavigationControllerComp::AddPageStep(const QByteArray& currentPageId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_visitedPages.push_back(currentPageId);

	m_currentPageIndex = m_visitedPages.count() - 1;
}


// reimplemented (icomp::CComponentBase)

void CPageNavigationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CPageNavigationControllerComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtgui


