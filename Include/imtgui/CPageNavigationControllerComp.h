#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtgui/IPageNavigationController.h>


namespace imtgui
{


class CPageNavigationControllerComp:
			public icomp::CComponentBase,
			virtual public IPageNavigationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPageNavigationControllerComp);
		I_REGISTER_INTERFACE(IPageNavigationController);
	I_END_COMPONENT;

	// reimplemented (IPageNavigationController)
	virtual bool IsNextPageAvailable() const override;
	virtual bool IsPreviousPageAvailable() const override;
	virtual void GoForward() override;
	virtual void GoBack() override;

protected:
	virtual void AddPageStep(const QByteArray& currentPageId);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();
	virtual void OnComponentDestroyed();

private:
	QByteArrayList m_visitedPages;
	int m_currentPageIndex;
};


} // namespace imtgui


