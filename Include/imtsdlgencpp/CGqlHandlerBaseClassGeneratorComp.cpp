// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgencpp/CGqlHandlerBaseClassGeneratorComp.h>


// Qt includes
#include <QtCore/QTextStream>


//Acf includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtsdl/CSdlRequest.h>
#include <imtsdlgencpp/CSdlUnionConverter.h>


namespace imtsdlgencpp
{


// reimplemented (IIncludeDirectivesProvider)

bool CGqlHandlerBaseClassGeneratorComp::ProcessEntry(
				const imtsdl::CSdlEntryBase& sdlEntry,
				QIODevice* headerDevicePtr,
				QIODevice* sourceDevicePtr,
				const iprm::IParamsSet* /*paramsPtr*/) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_baseClassExtenderCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());


	const imtsdl::SdlRequestList requestList = m_sdlRequestListCompPtr->GetRequests(true);
	if (requestList.isEmpty()){
		// nothing todo
		return true;
	}

	bool retVal = true;
	if (headerDevicePtr != nullptr){
		retVal = retVal && ProcessHeaderClassFile(sdlEntry, headerDevicePtr);
	}
	if (sourceDevicePtr != nullptr){
		retVal = retVal && ProcessSourceClassFile(sdlEntry, sourceDevicePtr);
	}

	return retVal;
}


// reimplemented (IIncludeDirectivesProvider)

QList<imtsdl::IncludeDirective> CGqlHandlerBaseClassGeneratorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {};

	return retVal;
}



bool CGqlHandlerBaseClassGeneratorComp::ProcessHeaderClassFile(const imtsdl::CSdlEntryBase& /*sdlEntry*/, QIODevice* headerDevicePtr) const
{
	QTextStream ifStream(headerDevicePtr);

	// class begin
	ifStream << QStringLiteral("class CGraphQlHandlerCompBase: public ::imtservergql::CPermissibleGqlRequestHandlerComp");
	FeedStream(ifStream, 1, false);

	ifStream << QStringLiteral("{");
	FeedStream(ifStream, 2, false);

	// public section
	ifStream << QStringLiteral("public:");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("typedef ::imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;");
	FeedStream(ifStream, 2, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("I_BEGIN_BASE_COMPONENT(CGraphQlHandlerCompBase)");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("I_END_COMPONENT");
	FeedStream(ifStream, 2, false);


	// base class methods override definition
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("virtual ::imtbase::CTreeItemModel* CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;");
	FeedStream(ifStream, 2, false);

	// protected section
	// definition of pure virtual methods (to be reimplemented)
	ifStream << QStringLiteral("protected:");
	FeedStream(ifStream, 1, false);

	AddMethodsForDocument(ifStream, 1);

	// class end
	ifStream << QStringLiteral("};");
	FeedStream(ifStream, 3, false);

	return true;
}


bool CGqlHandlerBaseClassGeneratorComp::ProcessSourceClassFile(const imtsdl::CSdlEntryBase& /*sdlEntry*/, QIODevice* sourceDevicePtr) const
{
	QTextStream ifStream(sourceDevicePtr);

	AddCollectionMethodsImplForDocument(ifStream);

	return true;
}


void CGqlHandlerBaseClassGeneratorComp::AddMethodsForDocument(QTextStream& stream, uint hIndents) const
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// abstract methods");
	FeedStream(stream, 1, false);

	const imtsdl::SdlRequestList requestList = m_sdlRequestListCompPtr->GetRequests(true);
	for (const imtsdl::CSdlRequest& sdlRequest: requestList){
		if (sdlRequest.GetType() != imtsdl::CSdlRequest::T_SUBSCRIPTION) {
			AddMethodForDocument(stream, sdlRequest, hIndents);
		}
	}
}


void CGqlHandlerBaseClassGeneratorComp::AddMethodForDocument(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents) const
{
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
				m_customSchemaParamsCompPtr,
				m_argumentParserCompPtr,
				false);

	imtsdl::CSdlField outputArgument = sdlRequest.GetOutputArgument();
	CStructNamespaceConverter structNameConverter(
			outputArgument,
			sdlNamespace,
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("virtual ");
	stream << structNameConverter.GetString();
	stream << QStringLiteral(" On");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("(const C");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("GqlRequest& ");
	stream << GetDecapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("Request, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;");

	FeedStream(stream, 1, false);
}

void CGqlHandlerBaseClassGeneratorComp::AddCollectionMethodsImplForDocument(QTextStream& stream, uint hIndents) const
{
	const QString className = QStringLiteral("CGraphQlHandlerCompBase");
	const imtsdl::SdlRequestList requestList = m_sdlRequestListCompPtr->GetRequests(true);

	// add IsRequestSupported() method
	GenerateIsRequestSupportedMethodImpl(stream, requestList, className);
	FeedStream(stream, 2, false);

	// add CreateInternalResponse method
	// declare method
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("::imtbase::CTreeItemModel* ");
	stream << className;
	stream << ':' << ':';
	stream << QStringLiteral("CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const");
	FeedStream(stream, 1, false);

	// method's body
	FeedStreamHorizontally(stream, hIndents);
	stream << '{';
	FeedStream(stream, 1, false);
	if (requestList.isEmpty()){
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return BaseClass::CreateInternalResponse");
		stream << QStringLiteral("(gqlRequest, errorMessage);");
		FeedStream(stream, 1, false);
	}
	else {
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("const QByteArray commandId = gqlRequest.GetCommandId();");
		FeedStream(stream, 1, false);

		/// create a carrier model GQL spec	\link https://spec.graphql.org/draft/#sec-Response-Format
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("istd::TDelPtr<::imtbase::CTreeItemModel> modelPtr(new ::imtbase::CTreeItemModel);");
		FeedStream(stream, 1, false);

		/// \todo add errors model and don't forget to fill it	\link https://spec.graphql.org/draft/#sec-Errors

		// create dataModel - child of a carrier model
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("::imtbase::CTreeItemModel* dataModelPtr = modelPtr->AddTreeModel(\"data\");");
		FeedStream(stream, 2, false);

		// create sections for expected command IDs
		for (const imtsdl::CSdlRequest& sdlRequest: requestList){
			if (sdlRequest.GetType() != imtsdl::CSdlRequest::T_SUBSCRIPTION) {
				AddImplCodeForRequest(stream, sdlRequest, hIndents + 1);
			}
		}

		// create default section
		// add error message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// send log message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		// return
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return nullptr;");
		FeedStream(stream, 1, false);


		// end of method
		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 3, false);
	}
}


void CGqlHandlerBaseClassGeneratorComp::AddImplCodeForRequest(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents) const
{
	FeedStreamHorizontally(stream, hIndents);
	stream << '/' << '/' << ' ' << sdlRequest.GetName();
	FeedStream(stream, 1, false);

	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		m_customSchemaParamsCompPtr,
		m_argumentParserCompPtr,
		false);
	const QString requestClassName = sdlRequest.GetName() + QStringLiteral("GqlRequest");
	imtsdl::CSdlField outputArgument = sdlRequest.GetOutputArgument();
	CStructNamespaceConverter structNameConverter(
		outputArgument,
		sdlNamespace,
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		*m_sdlUnionListCompPtr,
		false);

	// [1] command ID check
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (commandId == C");
	stream << requestClassName;
	stream << QStringLiteral("::GetCommandId()){");
	FeedStream(stream, 1, false);

	// [1] create SDL request variable
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << 'C' <<  requestClassName;
	stream << ' ' << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral("(gqlRequest, false);");
	FeedStream(stream, 1, false);

	// [1->2] SDL request validate
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	stream << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral(".IsValid()){");
	FeedStream(stream, 1, false);

	// [2] add error message if SDL request is not valid
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected request for command-ID: '%1'\").arg(qPrintable(commandId));");
	FeedStream(stream, 1, false);

	// [2] add log message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
	FeedStream(stream, 2, false);

	// [2] return
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	// [2->1] end of SDL request validate
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 2, false);

	// [1] create payload variable by calling reimplemented method
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << structNameConverter.GetString();
	stream << QStringLiteral(" replyPayload = On");
	stream << sdlRequest.GetName() << '(';
	stream << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral(", gqlRequest, errorMessage);");
	FeedStream(stream, 1, false);

	// [1->2] ensure, the derived call didn't return an error
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!errorMessage.isEmpty()){");
	FeedStream(stream, 1, false);

	// [2] send error message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("SendErrorMessage(0, QString(\"The derived call [");
	stream << QStringLiteral("On") << sdlRequest.GetName();
	stream << QStringLiteral("] returned an error: %1\").arg(errorMessage));");
	FeedStream(stream, 2, false);

	// [2] return
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	// [2->1] end of derived error checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 2, false);

	imtsdl::CSdlUnion foundUnion;
	const bool isUnion = GetSdlUnionForField(outputArgument, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);
	if (isUnion){
		const static QString unionSourceVarName = QStringLiteral("&replyPayload");
		CSdlUnionConverter::WriteConversionFromUnion(stream,
					foundUnion,
					unionSourceVarName,
					outputArgument.GetId(),
					m_originalSchemaNamespaceCompPtr->GetText(),
					outputArgument.GetId(),
					QString(),
					*m_sdlTypeListCompPtr,
					*m_sdlEnumListCompPtr,
					*m_sdlUnionListCompPtr,
					hIndents + 1,
					CSdlUnionConverter::CT_MODEL_SCALAR,
					QStringLiteral("dataModelPtr->SetData("), //QString(),
					QStringLiteral("*dataModelPtr"),
					QStringLiteral("nullptr"));
	}
	else{
		// [1] write payload variable in model and create variable, to check if it success
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("const bool isModelCreated = ");
		stream << QStringLiteral("replyPayload.WriteToModel(*dataModelPtr);");
		FeedStream(stream, 1, false);

		// [1->2] check if payload write to TreeModel is failed
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (!isModelCreated){");
		FeedStream(stream, 1, false);

		// [2] set error message
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("errorMessage = QString(\"Internal error. Unable to create response for command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// [2] add log message
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("SendCriticalMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		// [2] return
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("return nullptr;");
		FeedStream(stream, 1, false);

		// [2->1] end of payload write to TreeModel checks
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << '}';
		FeedStream(stream, 2, false);
	}


	// [1] return pop ptr
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return modelPtr.PopPtr();");
	FeedStream(stream, 1, false);

	// end of section
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 2, false);
}


} // namespace imtsdlgencpp

