#pragma once


// ACF includes
#include <istd/IInformationProvider.h>
#include <istd/IChangeable.h>


namespace imtauth
{


class ISessionChangeNotifier: virtual public istd::IPolymorphic
{
public:
	virtual void OnSessionModelChanged(const istd::IChangeable::ChangeSet& changeSet, const QByteArray& sessionId) = 0;
};


} // namespace imtauth


