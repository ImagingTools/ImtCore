// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/CProductPermissionSnapshot.h>


namespace imtauth
{


class CProductPermissionSnapshotComp:
			public icomp::CComponentBase,
			public CIdentifiableProductPermissionSnapshot
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductPermissionSnapshotComp);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT;
};


} // namespace imtauth
