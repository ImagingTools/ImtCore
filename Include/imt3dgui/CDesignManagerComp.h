#pragma once


// ImtCore includes
#include <imtstyle/CDesignManagerCompBase.h>


namespace imt3dgui
{


class CDesignManagerComp: public imtstyle::CDesignManagerCompBase
{
public:
	typedef imtstyle::CDesignManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDesignManagerComp);
	I_END_COMPONENT;

protected:
	virtual void RegisterResourcesFunctions() override;
};


} // namespace imt3dgui


