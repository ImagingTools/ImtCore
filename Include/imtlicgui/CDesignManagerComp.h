#pragma once


// ImtCore includes
#include <imtstyle/CDesignManagerCompBase.h>


namespace imtlicgui
{


class CDesignManagerComp: public imtgui::CDesignManagerCompBase
{
public:
	typedef imtgui::CDesignManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDesignManagerComp);
	I_END_COMPONENT;

protected:
	virtual void RegisterResourcesFunctions() override;
};


} // namespace imtlicgui


