#include <imtrest/CJsonModelBasedServletComp.h>


// STL includes
#include <stdlib.h>
#include <time.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QRandomGenerator>
#include <QtGui/QColor>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtrest/IRequestServlet.h>
#include <imtrest/CHttpRequest.h>


namespace imtrest
{


// public methods

CJsonModelBasedServletComp::CJsonModelBasedServletComp()
{
}


// reimplemented (IRequestServlet)

IRequestServlet::ConstResponsePtr CJsonModelBasedServletComp::ProcessRequest(const IRequest& request) const
{
	const IProtocolEngine& engine = request.GetProtocolEngine();
	QByteArray errorBody = "<html><head><title>Error</title></head><body><p>File resource was not found</p></body></html>";
	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	ConstResponsePtr errorResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_RESOURCE_NOT_AVAILABLE, errorBody, reponseTypeId));

	auto generateErrorResponsePtr = [&request, &engine, reponseTypeId](QByteArray errorBody, int errorCode = IProtocolEngine::SC_INTERNAL_ERROR){
		qCritical() << __FILE__ << __LINE__ << "Error occurred" << errorBody;
		QByteArray generatedErrorBody = "<html><head><title>Error</title></head><body><p>";
		generatedErrorBody.append(errorBody);
		generatedErrorBody.append("</p></body></html>");
		return ConstResponsePtr(engine.CreateResponse(request, errorCode, generatedErrorBody, reponseTypeId));
	};

	QByteArray body;
	QByteArray commandId = request.GetCommandId();
	QByteArray commandIdBase = *this->m_commandIdAttrPtr;
	if (commandIdBase.endsWith('*'))
	{
		commandIdBase.chop(1);
	}
	QByteArray modelName = commandId.replace(commandIdBase, "");
	if (modelName.endsWith('/')){
		modelName.chop(1);
	}

	while (modelName.startsWith('/'))
	{
		modelName.remove(0,1);
	}

	if (modelName == "__HELLO_MODEL__")
	{
		reponseTypeId = "application/json";
		body = QByteArray(R"({"I'm seyin to you": "HELLO WORLD"})");
	}
	else if (modelName == "__EXAMPLE_1__")
	{
		reponseTypeId = "application/json";
		body = QByteArray(R"({"adresses":[{"city":"Moscou","country":"Russia","postalCode":644099,"street":"Lenina 10"},{"city":"Munic","country":"Germany","postalCode":123456,"street":"Street St"}],"firstName":"Ivan","lastName":"Ivanov","nickName":"NicIvan"})");
	}
	else if (modelName == "__EXAMPLE_2__")
	{
		reponseTypeId = "application/json";
		body = QByteArray(R"({"firstName":"Ivan","lastName":"Ivanov","nickName":"NicIvan","adresses":[{"country":"Russia","city":"Moscow","postalCode":644099,"street":"Lenina 10"},{"country":"Germany","city":"Munic","postalCode":123456,"street":"Street St"}]})");
	}
	else if (modelName == "__COLORS_EXAMPLE__")
	{
		reponseTypeId = "application/json";
		int sizeParam = request.GetCommandParams().value("size").toInt();
		if (sizeParam <= 0)
		{
			sizeParam = 50;
		}
		auto generateRandomNumber = [](int min = 0, int max = 255){
			::srand((unsigned) ::rand());
			return min + ::rand() % (max - min);
		};

		QJsonArray allArray;
		for (int j = 0; j < sizeParam; ++j)
		{
			QJsonArray colorsArray;
			for (int i = 0; i < sizeParam ; ++i)
			{
				quint32 r = generateRandomNumber(1,254);
				quint32 g = generateRandomNumber(2,253);
				quint32 b = generateRandomNumber(3,252);
				colorsArray << QJsonObject({QPair<QString, QJsonValue>(QString("Color ").append(QString::number(i)),QColor::fromRgb(r,g,b).name(QColor::HexRgb))});
			}

			allArray << QJsonObject({QPair<QString, QJsonValue>(QString("data ").append(QString::number(j)),colorsArray)});
		}

		body = QJsonDocument(QJsonObject({QPair<QString, QJsonValue>("data",allArray)})).toJson(QJsonDocument::Compact);
	}
	else if (modelName == "__EXAMPLE_2__"){
		reponseTypeId = "application/json";
		body = QByteArray(R"({"firstName":"Ivan","lastName":"Ivanov","nickName":"NicIvan","adresses":[{"country":"Russia","city":"Moscow","postalCode":644099,"street":"Lenina 10"},{"country":"Germany","city":"Munic","postalCode":123456,"street":"Street St"}]})");
	}
	else if (modelName == "__CREATE_COLORS_IN_CTREE__"){
		using namespace imtbase;
		imtbase::CTreeItemModel treeItemModel;
		treeItemModel.AddTreeModel("data");

		auto header = treeItemModel.GetTreeItemModel("data");

		int sizeParam = request.GetCommandParams().value("size").toInt();
		if (sizeParam <= 0)
		{
			sizeParam = 50;
		}

		auto generateRandomNumber = [](int min = 0, int max = 255){
			::srand((unsigned) ::rand());
			return min + ::rand() % (max - min);
		};

		for (int i = 0; i < sizeParam ; ++i)
		{
			QByteArray subModelName = QByteArray("data ") + QByteArray::number(i);
			int index = header->InsertNewItem();
			header->AddTreeModel(subModelName, index);
			CTreeItemModel* treeModelPtr = header->GetTreeItemModel(subModelName,index);
			for (int j = 0; j < sizeParam; ++j)
			{
				quint32 r = generateRandomNumber(1,254);
				quint32 g = generateRandomNumber(2,253);
				quint32 b = generateRandomNumber(3,252);
				int newIndex = treeModelPtr->InsertNewItem();
				treeModelPtr->SetData(QByteArray("Color ") + QByteArray::number(j), QColor::fromRgb(r,g,b).name(QColor::HexRgb), newIndex);
			}
		}

		reponseTypeId = "application/json";

		{
//			QByteArray jsonData;
			iser::CJsonMemWriteArchive archive(body);
			treeItemModel.Serialize(archive);
//			body = jsonData;
		}
	}
	else if (m_representationDataProvider.IsValid())
	{
		imtrest::CHttpRequest* httpRequestPtr = dynamic_cast<imtrest::CHttpRequest*>(const_cast<imtrest::IRequest*>(&request));
		if (httpRequestPtr != nullptr && httpRequestPtr->GetMethodType() == imtrest::CHttpRequest::MT_POST)
		{
			QByteArray requestBody = httpRequestPtr->GetBody();
			reponseTypeId = "application/json";
			if (m_representationDataProvider->SetRepresentationData(
						imtrest::IRepresentationDataProvider::RF_JSON,
						requestBody,modelName) == false){
				return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_NOT_IMPLEMENTED, "FAIL(", "plain/text; charset=utf-8"));
			}
			else{
				return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, "Sucsess", "plain/text; charset=utf-8"));
			}

		}
		else{
			reponseTypeId = "application/json";
			QList<QByteArray> query = modelName.split('/');
			imtrest::QueryParams params = request.GetCommandParams();
			if (m_representationDataProvider->GetRepresentationData(
						imtrest::IRepresentationDataProvider::RF_JSON,
						body, query, params) == false){
				return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_NOT_IMPLEMENTED, "FAIL(", "plain/text; charset=utf-8"));
			}
			else {
				return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_OK, body, reponseTypeId));
			}
		}

	}
	else
	{
//		return generateErrorResponsePtr(QByteArray("Cannot init ModelProcessor OffLine OR Not created OR not setted"), IProtocolEngine::SC_UNKNOWN_ERROR);
		return ConstResponsePtr(engine.CreateResponse(request, IProtocolEngine::SC_NOT_IMPLEMENTED, "FAIL(", "plain/text; charset=utf-8"));
	}

	ConstResponsePtr responsePtr(engine.CreateResponse(request, IProtocolEngine::SC_I_AM_A_TEAPOT, body, reponseTypeId));

	return responsePtr;
}


QByteArray CJsonModelBasedServletComp::GetSupportedCommandId() const
{
	return *m_commandIdAttrPtr;
}


} // namespace imtrest


