#pragma once



// ImCore includes
#include <istd/IChangeable.h>


namespace imtgui
{


class IPageNavigationController: virtual public istd::IChangeable
{
public:
	virtual bool IsNextPageAvailable() const = 0;
	virtual bool IsPreviousPageAvailable() const = 0;
	virtual void GoForward() = 0;
	virtual void GoBack() = 0;
};


} // namespace imtgui


