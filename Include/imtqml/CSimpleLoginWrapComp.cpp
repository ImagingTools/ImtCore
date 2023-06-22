#include <imtqml/CSimpleLoginWrapComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtqml
{


const istd::IChangeable::ChangeSet s_loginChangeSet(iauth::ILogin::CF_LOGIN, QObject::tr("Login"));
const istd::IChangeable::ChangeSet s_logoutChangeSet(iauth::ILogin::CF_LOGOUT, QObject::tr("Logout"));


// public methods

CSimpleLoginWrapComp::CSimpleLoginWrapComp()
{
}


// reimplemented (iauth::ILogin)

iauth::CUser* CSimpleLoginWrapComp::GetLoggedUser() const
{
	if (!m_loggedUserId.isEmpty()){
		istd::TDelPtr<iauth::CUser> userPtr(new iauth::CUser);
		userPtr->SetUserName(m_loggedUserId);

		return userPtr.PopPtr();
	}

	return nullptr;
}


bool CSimpleLoginWrapComp::Login(const QString& userName, const QString& password)
{
	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, "UserToken");
	imtgql::CGqlObject inputObject("input");
	inputObject.InsertField("Login", userName);
	inputObject.InsertField("Password", password);
	request.AddParam(inputObject);

	imtbase::CTreeItemModel responseModel;
	bool retVal = SendModelRequest(request, responseModel);
	if (retVal){
		imtbase::CTreeItemModel* dataModelPtr = responseModel.GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
			return false;
		}

		if (dataModelPtr->ContainsKey("Login")){
			istd::CChangeNotifier notifier(this, &s_loginChangeSet);
			Q_UNUSED(notifier);

			m_loggedUserId = dataModelPtr->GetData("Login").toByteArray();
		}

		if (dataModelPtr->ContainsKey("Token")){
			m_loggedUserToken = dataModelPtr->GetData("Token").toByteArray();
		}

		return true;
	}

	return false;
}


bool CSimpleLoginWrapComp::Logout()
{
	if (!m_loggedUserId.isEmpty()){
		istd::CChangeNotifier notifier(this, &s_logoutChangeSet);
		Q_UNUSED(notifier);

		m_loggedUserId.clear();

		return true;
	}

	return false;
}


// reimplemented (imtauth::IAccessTokenProvider)

QByteArray CSimpleLoginWrapComp::GetToken(const QByteArray& /*userId*/) const
{
	return m_loggedUserToken;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSimpleLoginWrapComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


} // namespace imtqml


