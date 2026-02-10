// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <istd/istd.h>


namespace imtgeo
{


enum AddressTypes
{
	AT_COUNTRY				= 0,
	AT_REGION				= 1,
	AT_AREA					= 2,
	AT_CITY					= 3,
	AT_SETTLEMENT			= 4,
	AT_DISTRICT				= 5,
	AT_STREET				= 6,
	AT_BUILDING				= 7,
	AT_APARTMENT			= 8,
	AT_SECTION				= 9,
	AT_SNT					= 10,
	AT_ALLEY				= 11,
	AT_OTHER				= 12
};

I_DECLARE_ENUM(AddressTypes, AT_COUNTRY, AT_REGION, AT_AREA, AT_CITY, AT_SETTLEMENT, AT_DISTRICT,
			   AT_STREET, AT_BUILDING, AT_APARTMENT, AT_SECTION, AT_SNT, AT_ALLEY, AT_OTHER);




} // namespace imtgeo


Q_DECLARE_METATYPE(imtgeo::AddressTypes);

