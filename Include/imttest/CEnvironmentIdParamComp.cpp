// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttest/CEnvironmentIdParamComp.h>


namespace imttest
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CEnvironmentIdParamComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	const QByteArray variableName = *m_environmentVariableAttrPtr;

	BaseClass2::SetId(qgetenv(variableName.constData()));
}


} // namespace imttest


