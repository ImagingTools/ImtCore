// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IProductInfo.h>


namespace imtauth
{


/**
	The permission trees of the products a contract manager checks the given permissions against.
*/
class CContractManagerProductScopeComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CContractManagerProductScopeComp);
		I_ASSIGN(m_permissionTreeCompPtr, "PermissionTree", "Permission tree of this server product", false, "PermissionTree");
		I_ASSIGN(m_snapshotCollectionCompPtr, "SnapshotCollection", "Published permission trees of the products", true, "SnapshotCollection");
	I_END_COMPONENT;

protected:
	I_REF(imtbase::IObjectCollection, m_snapshotCollectionCompPtr);
	I_REF(imtlic::IProductInfo, m_permissionTreeCompPtr);
};


} // namespace imtauth


