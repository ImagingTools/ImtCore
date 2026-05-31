// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdesk/IEntityTypeProvider.h>


namespace imtdesk
{


class CDelegatedEntityTypeProviderComp:
			public icomp::CComponentBase,
			virtual public IEntityTypeProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedEntityTypeProviderComp)
		I_REGISTER_INTERFACE(IEntityTypeProvider)
		I_ASSIGN(m_slaveEntityTypeProviderCompPtr, "SlaveEntityTypeProvider", "Slave entity type provider", false, "SlaveEntityTypeProvider");
	I_END_COMPONENT

	// reimplemented (imtdesk::IEntityTypeProvider)
	virtual QString GetEntityTypeId() const override;
	virtual QString GetEntityTypeName() const override;

private:
	I_REF(IEntityTypeProvider, m_slaveEntityTypeProviderCompPtr);
};


} // namespace imtdesk

