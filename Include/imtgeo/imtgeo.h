#pragma once


// Qt includes
#include <QtCore>

// ACF includes
#include <istd/istd.h>


namespace imtgeo
{


enum AddressTypes
{
	AT_COUNTRY				= 0,
	AT_REGION				= 1,
	AT_CITY					= 2,
	AT_DISTRICT				= 3,
	AT_STREET				= 4,
	AT_BUILDING				= 5,
	AT_APARTMENT			= 6,
	AT_OTHER				= 7
};

I_DECLARE_ENUM(AddressTypes, AT_COUNTRY, AT_REGION, AT_CITY, AT_DISTRICT,
			   AT_STREET, AT_BUILDING, AT_APARTMENT, AT_OTHER);




} // namespace imtgeo


Q_DECLARE_METATYPE(imtgeo::AddressTypes);

