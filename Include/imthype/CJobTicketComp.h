// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imthype/CJobTicket.h>


namespace imthype
{


class CJobTicketComp:
		public icomp::CComponentBase,
		public CJobTicket
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CJobTicket BaseClass2;

	I_BEGIN_COMPONENT(CJobTicketComp);
		I_REGISTER_INTERFACE(IJobTicket);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_jobParamsFactPtr, "JobParamsFactory", "Factory used for creation of job params instance", true, "JobParamsFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_FACT(iprm::IParamsSet, m_jobParamsFactPtr);
};


} // namespace imthype


