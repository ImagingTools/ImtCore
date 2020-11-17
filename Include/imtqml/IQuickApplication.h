#pragma once


// ACF includes
#include <ibase/IApplication.h>
#include <imtqml/IQuickObject.h>


namespace imtqml
{


/**
	Interface for Qt Quick based application with GUI.
*/
class IQuickApplication : virtual public ibase::IApplication
{
public:
	/**
		Get the main UI object of the application.
	*/
	virtual const imtqml::IQuickObject* GetApplicationItem() const = 0;
};


} // namespace imtqml


