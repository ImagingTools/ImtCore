// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ISessionChangeNotifier.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtauthgql
{


class CSessionModelSubscriberControllerComp:
			public imtservergql::CGqlPublisherCompBase,
			virtual public imtauth::ISessionChangeNotifier
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CSessionModelSubscriberControllerComp);
		I_REGISTER_INTERFACE(imtauth::ISessionChangeNotifier);
	I_END_COMPONENT;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnSessionModelChanged(const istd::IChangeable::ChangeSet& changeSet, const QByteArray& sessionId) override;
};


} // namespace imtauthgql


