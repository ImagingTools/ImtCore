// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CMigrationControllerCompBase.h>


namespace imtdb
{


// protected methods

// reimplemented (imtdb::IMigrationController)

istd::CIntRange CMigrationControllerCompBase::GetMigrationRange() const
{
	return m_range;
}


// reimplemented (icomp::CComponentBase)

void CMigrationControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int min = *m_rangeFromAttrPtr;
	int max = *m_rangeToAttrPtr;

	if (min < 0){
		min = 0;
	}

	m_range = istd::CIntRange(min, max);
}


} // namespace imtdb


