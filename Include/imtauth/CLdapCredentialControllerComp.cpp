#include <imtauth/CLdapCredentialControllerComp.h>


// Windows includes
#ifdef Q_OS_WIN
#include <windows.h>
#include <LMaccess.h>
#include <Lmapibuf.h>
#pragma comment(lib, "netapi32.lib")
#endif


namespace imtauth
{


// protected methods

// reimplemented (imtauth::ICredentialController)

bool CLdapCredentialControllerComp::CheckCredential(const QByteArray& login, const QByteArray& password) const
{
	int result = 0;

#ifdef Q_OS_WIN
	HANDLE  hUser;
	result = LogonUser(qUtf16Printable(login), L".", qUtf16Printable(password), LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hUser);
#endif

	return result > 0;
}


} // namespace imtauth


