#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgql/IGqlClient.h>
#include <imtbase/CTreeItemModel.h>


namespace imtgql
{


template <class Base>
class TClientRequestManagerCompWrap: public Base
{
public:
	typedef Base BaseClass;

	I_BEGIN_BASE_COMPONENT(TClientRequestManagerCompWrap);
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "API Client", true, "ApiClient");
	I_END_COMPONENT;

protected:
	virtual bool SendModelRequest(const IGqlRequest& request, imtbase::CTreeItemModel& responseModel) const;

private:
	class Response: virtual public imtgql::IGqlResponseHandler
	{
	public:
		Response();
		virtual imtbase::CTreeItemModel* GetResult();

		// reimplemented (imtgql::IGqlClient::ResponseHandler)
		virtual void OnReply(const imtgql::IGqlRequest& request, const QByteArray& replyData) override;

	private:
		imtbase::CTreeItemModel* m_replyResultPtr;
	};

protected:
	I_REF(imtgql::IGqlClient, m_apiClientCompPtr);
};


template<class Base>
bool TClientRequestManagerCompWrap<Base>::SendModelRequest(const IGqlRequest& request, imtbase::CTreeItemModel& responseModel) const
{
	if (!m_apiClientCompPtr.IsValid()){
		return false;
	}

	Response responseHandler;
	bool retVal = m_apiClientCompPtr->SendRequest(request, responseHandler);
	if (retVal){
		imtbase::CTreeItemModel* resultModelPtr = responseHandler.GetResult();
		if (resultModelPtr == nullptr){
			return false;
		}

		return responseModel.Copy(resultModelPtr);
	}

	return false;
}


// public methods of the embedded class Response

template <class Base>
TClientRequestManagerCompWrap<Base>::Response::Response()
	:m_replyResultPtr(nullptr)
{
}


template <class Base>
imtbase::CTreeItemModel* TClientRequestManagerCompWrap<Base>::Response::GetResult()
{
	return m_replyResultPtr;
}


// reimplemented (imtgql::IGqlClient::ResponseHandler)

template <class Base>
void TClientRequestManagerCompWrap<Base>::Response::OnReply(const imtgql::IGqlRequest& request, const QByteArray& replyData)
{
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

					m_replyResultPtr = new imtbase::CTreeItemModel();

					bool ok = m_replyResultPtr->CreateFromJson(parserData);
					if (!ok){
						qDebug() << QString("Error when try create model from json: %1").arg(qPrintable(parserData));
					}
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
				
				m_replyResultPtr = new imtbase::CTreeItemModel();

				m_replyResultPtr->CreateFromJson(parserData);
			}
		}
	}
}


typedef TClientRequestManagerCompWrap<icomp::CComponentBase> CClientRequestManagerCompBase;


} // namespace imtgql


