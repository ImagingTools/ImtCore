#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <istd/TSmartPtr.h>


namespace imtbase
{


typedef istd::TSmartPtr<istd::IChangeable> IChangeablePtr;


} // namespace imtbase


Q_DECLARE_METATYPE(imtbase::IChangeablePtr);


