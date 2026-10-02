// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IAccessContext.h>


namespace imtbase
{


/**
	Access context for trusted system operations.
	A database engine using this context is not restricted by tenant isolation. Use it only for the engine
	instances of migrations and maintenance components.
*/
class CSystemAccessContextComp:
			public icomp::CComponentBase,
			virtual public IAccessContext
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSystemAccessContextComp);
		I_REGISTER_INTERFACE(IAccessContext);
	I_END_COMPONENT;

	// reimplemented (imtbase::IAccessContext)
	virtual AccessMode GetAccessMode() const override;
	virtual QByteArray GetTenantId() const override;
	virtual QByteArray GetUserId() const override;
};


} // namespace imtbase


