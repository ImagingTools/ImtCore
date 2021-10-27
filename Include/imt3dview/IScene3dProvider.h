#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imt3dview
{


class IScene3d;



class IScene3dProvider: virtual public istd::IPolymorphic
{
public:
	virtual IScene3d* GetScene() const = 0;
};


} // namespace imt3dview


