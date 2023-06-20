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

	Response responseHandler;
	bool retVal = m_apiClientCompPtr->SendRequest(request, responseHandler);
	if (retVal){
		imtbase::CTreeItemModel* resultModelPtr = responseHandler.GetResult();
		if (resultModelPtr == nullptr){
			return false;
		}

		imtbase::CTreeItemModel* dataModelPtr = resultModelPtr->GetTreeItemModel("data");
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


// reimplemented (iser::ISerializable)

bool CSimpleLoginWrapComp::Serialize(iser::IArchive& archive)
{
	return true;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSimpleLoginWrapComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


// private methods

CSimpleLoginWrapComp::Response::Response()
	:m_replyResultPtr(nullptr)
{
}


// reimplemented (imtgql::IGqlClient::ResponseHandler)

imtbase::CTreeItemModel* CSimpleLoginWrapComp::Response::GetResult()
{
	return m_replyResultPtr;
}


void CSimpleLoginWrapComp::Response::OnReply(const imtgql::IGqlRequest& request, const QByteArray& replyData)
{
	m_replyResultPtr = new imtbase::CTreeItemModel();
	QJsonDocument document = QJsonDocument::fromJson(replyData);
	if (document.isObject()){
		QJsonObject dataObject = document.object();

		if (dataObject.contains("errors")){
			QJsonValue jsonValue = dataObject.value("errors");
			if (jsonValue.isObject()){
				QJsonObject errorObj = jsonValue.toObject();
				if (errorObj.contains(request.GetCommandId())){
					QJsonValue bodyValue = errorObj.value(request.GetCommandId());
					dataObject = QJsonObject();
					dataObject.insert("errors", bodyValue);
					document.setObject(dataObject);
					QByteArray parserData = document.toJson(QJsonDocument::Compact);
					m_replyResultPtr->CreateFromJson(parserData);
				}
			}
		}

		dataObject = document.object().value("data").toObject();
		if (!dataObject.isEmpty()){
			QJsonValue bodyValue = dataObject.value(request.GetCommandId());
			if (!bodyValue.isNull()){
				dataObject = QJsonObject();
				dataObject.insert("data", bodyValue);
				document.setObject(dataObject);
				QByteArray parserData = document.toJson(QJsonDocument::Compact);
				m_replyResultPtr->CreateFromJson(parserData);
			}
		}
	}
}

} // namespace imtqml


