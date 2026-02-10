// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtauth/ISessionChangeNotifier.h>


namespace imtauthgql
{


class CSessionModelObserverComp:
			public icomp::CComponentBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSessionModelObserverComp);
		I_ASSIGN(m_sessionChangeNotifierCompPtr, "SessionChangeNotifier", "Session change notifier", false, "SessionChangeNotifier");
		I_ASSIGN(m_modelCompPtr, "Model", "Model", true, "Model");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	I_REF(imtauth::ISessionChangeNotifier, m_sessionChangeNotifierCompPtr);
	I_REF(imod::IModel, m_modelCompPtr);
};


} // namespace imtauthgql


