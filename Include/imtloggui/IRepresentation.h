#pragma once


// ACF Includes
#include <imod/IModel.h>
#include <imod/IObserver.h>


namespace imtloggui
{


class IRepresentation: virtual public istd::IChangeable
{
	virtual imod::IModel* GetRepresentationModel() = 0;
	virtual imod::IObserver* GetTimeRangeObserver() = 0;
};


} // namespace imtloggui


