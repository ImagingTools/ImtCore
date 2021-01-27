#pragma once


// Acf includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IGraphicsController: virtual public istd::IPolymorphic
{
public:
	virtual void InitScene() = 0;
};


} // namespace imtloggui


