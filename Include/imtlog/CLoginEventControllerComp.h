// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iauth/ILogin.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/TSingleModelObserverBase.h>


namespace imtlog
{


class CLoginEventControllerComp:
			public ilog::CLoggerComponentBase,
			public imod::TSingleModelObserverBase<iauth::ILogin>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imod::TSingleModelObserverBase<iauth::ILogin> BaseClass2;

	I_BEGIN_COMPONENT(CLoginEventControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void BeforeUpdate(imod::IModel* modelPtr) override;
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	QString m_lastUserName;
};


} // namespace imtlog


