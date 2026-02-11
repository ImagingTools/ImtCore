// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CModelPublisherComp.h>


// ImtCore includes
#include<imtrest/IProtocolEngine.h>


namespace imtservergql
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CModelPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_modelCompPtr.IsValid()){
		m_modelCompPtr->AttachObserver(this);
	}
}


void CModelPublisherComp::OnComponentDestroyed()
{
	if (m_modelCompPtr.IsValid()){
		m_modelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CModelPublisherComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	Publish((*m_useAwsStyle));
}


} // namespace imtservergql


