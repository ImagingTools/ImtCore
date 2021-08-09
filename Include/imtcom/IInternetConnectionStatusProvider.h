#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <iprm/IEnableableParam.h>
#include <icomp/CComponentBase.h>


namespace imtcom
{


class IInternetConnectionStatusProvider: virtual public istd::IChangeable
{
public:
	enum InternetConnectionStatus
	{
		ICS_UNKNOWN = 0,
		ICS_OFFLINE,
		ICS_ONLINE
	};

	virtual InternetConnectionStatus GetIntenetConnectionStatus() const = 0;
};


} // namespace imtcom


