#pragma once


// ImtCore includes
#include <imtauth/ISessionChangeNotifier.h>
#include <imtservergql/CGqlSubscriberControllerCompBase.h>


namespace imtauthgql
{


class CSessionModelSubscriberControllerComp:
			public imtservergql::CGqlSubscriberControllerCompBase,
			virtual public imtauth::ISessionChangeNotifier
{
public:
	typedef imtservergql::CGqlSubscriberControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSessionModelSubscriberControllerComp);
		I_REGISTER_INTERFACE(imtauth::ISessionChangeNotifier);
	I_END_COMPONENT;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnSessionModelChanged(const istd::IChangeable::ChangeSet& changeSet, const QByteArray& sessionId) override;
};


} // namespace imtauthgql


