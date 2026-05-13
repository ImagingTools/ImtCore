// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CSingleObjectProviderComp.h>


namespace imtbase
{


// public methods

CSingleObjectProviderComp::CSingleObjectProviderComp()
	:m_modelUpdateBridge(this)
{
}


// reimplemented (IObjectProvider)

const istd::IChangeable* CSingleObjectProviderComp::GetDataObject() const
{
	return m_dataObjectCompPtr.GetPtr();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSingleObjectProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_dataObjectModelCompPtr.IsValid()){
		m_dataObjectModelCompPtr->AttachObserver(&m_modelUpdateBridge);
	}
}


void CSingleObjectProviderComp::OnComponentDestroyed()
{
	m_modelUpdateBridge.EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


