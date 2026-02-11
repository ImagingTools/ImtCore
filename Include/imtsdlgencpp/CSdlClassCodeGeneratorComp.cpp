// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgencpp/CSdlClassCodeGeneratorComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

//Acf includes
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtsdl/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlType.h>


namespace imtsdlgencpp
{

// reimplemented (ICxxFileProcessor)

bool CSdlClassCodeGeneratorComp::ProcessEntry (
			const imtsdl::CSdlEntryBase& sdlEntry, 
			QIODevice* headerDevicePtr,
			QIODevice* sourceDevicePtr,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<const imtsdl::CSdlType*>(&sdlEntry);
	if (sdlTypePtr == nullptr){
		// nothing todo
		Q_ASSERT_X(false, __func__, "Configuration invalid: ONLY TYPES are supported.");

		return false;
	}

	// First create all files with basic methods
	if (!BeginClassFiles(*sdlTypePtr, headerDevicePtr, sourceDevicePtr, paramsPtr)){
		SendErrorMessage(0, QString("Unable to process files"));

		return false;
	}

	// Then let extenders to make changes. Add new transformation methods (JSON, GQL, ...)
	const int extendersCount = m_codeGeneratorExtenderListCompPtr.GetCount();
	for (int i = 0; i < extendersCount; ++i){
		ICxxFileProcessor* extenderPtr = m_codeGeneratorExtenderListCompPtr[i];
		Q_ASSERT(extenderPtr != nullptr);

		bool extendSuccess = extenderPtr->ProcessEntry(*sdlTypePtr, headerDevicePtr, sourceDevicePtr, paramsPtr);
		if (!extendSuccess){
			return false;
		}
	}

	// And complete the processing
	EndClassFiles(*sdlTypePtr, headerDevicePtr, sourceDevicePtr, paramsPtr);
	

	return true;
}


// reimplemented (IIncludeDirectivesProvider)

QList<imtsdl::IncludeDirective> CSdlClassCodeGeneratorComp::GetIncludeDirectives() const
{
	QList<imtsdl::IncludeDirective> retVal;

	if (m_includeDirectivesProviderListCompPtr.IsValid()){
		const int providersCount = m_includeDirectivesProviderListCompPtr.GetCount();
		for (int i = 0; i < providersCount; ++i){
			IIncludeDirectivesProvider* providerPtr = m_includeDirectivesProviderListCompPtr[i];
			Q_ASSERT(providerPtr != nullptr);

			retVal += providerPtr->GetIncludeDirectives();
		}
	}

	/// \todo remove it, make a part of extenders
	retVal += CreateImtDirective("<imtsdl/TListModelBase.h>");
	retVal += CreateImtDirective("<imtsdl/TElementList.h>");
	retVal += CreateQtDirective("<QtCore/QMetaEnum>");

	return retVal;
}


// private methods

bool CSdlClassCodeGeneratorComp::BeginClassFiles(const imtsdl::CSdlType& sdlType, QIODevice* headerPtr, QIODevice* sourcePtr, const iprm::IParamsSet* paramsPtr) const
{
	bool retVal = true;
	if (headerPtr != nullptr){
		retVal = retVal && BeginHeaderClassFile(sdlType, headerPtr, paramsPtr);
	}
	if (sourcePtr != nullptr){
		retVal = retVal && BeginSourceClassFile(sdlType, sourcePtr, paramsPtr);
	}

	return retVal;
}


bool CSdlClassCodeGeneratorComp::BeginHeaderClassFile(const imtsdl::CSdlType& sdlType, QIODevice* headerPtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream stream(headerPtr);

	// class begin
	stream << QStringLiteral("class C") << sdlType.GetName() << '\n';
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("public:");
	FeedStream(stream, 1, false);

	// version enum
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("enum ProtocolVersion");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("PV_") << GetSdlEntryVersion(sdlType, false) << ',';
	FeedStream(stream, 1, false);

	/// \todo later add other versions here

	// add last alias
	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("PV_LAST = PV_") << GetSdlEntryVersion(sdlType, false);
	FeedStream(stream, 1, false);

	// complete protocol version enum
	FeedStreamHorizontally(stream);
	stream << '}' << ';';
	FeedStream(stream, 2, false);

	GenerateVersionStruct(stream, sdlType);

	stream.flush();

	return true;
}


bool CSdlClassCodeGeneratorComp::BeginSourceClassFile(const imtsdl::CSdlType& sdlType, QIODevice* sourcePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream stream(sourcePtr);

	//implement GetVersionId method
	stream << QStringLiteral("QByteArray C");
	stream << sdlType.GetName();
	stream << ':' << ':';
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("::GetVersionId()");
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return QByteArrayLiteral(\"");
	stream << GetTypeVersion(sdlType);
	stream << QStringLiteral("\");");
	FeedStream(stream, 1, false);

	stream << '}';
	FeedStream(stream, 3, false);

	// implement comparation operator
	stream << QStringLiteral("bool C");
	stream << sdlType.GetName();
	stream << ':' << ':';
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("::operator==(const ");
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("& other) const");
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return ");
	FeedStream(stream, 1, true);

	QListIterator sdlFieldIter(sdlType.GetFields());
	while (sdlFieldIter.hasNext()){
		const imtsdl::CSdlField& sdlField = sdlFieldIter.next();

		FeedStreamHorizontally(stream, 4);
		const QString fieldType = sdlField.GetType();

		// use fuzzy compare for double and float
		if (	!sdlField.IsArray() &&(
					fieldType == QStringLiteral("Double") ||
					fieldType == QStringLiteral("Float")))
		{
			stream << sdlField.GetId();
			stream << QStringLiteral(".has_value() == other.");
			stream << sdlField.GetId();
			stream << QStringLiteral(".has_value() &&");
			FeedStream(stream);

			FeedStreamHorizontally(stream, 4);
			stream << '(' << '(';
			stream << sdlField.GetId();
			stream << QStringLiteral(".has_value() && other.");
			stream << sdlField.GetId();
			stream << QStringLiteral(".has_value()) ?");
			FeedStream(stream);

			FeedStreamHorizontally(stream, 5);
			stream << QStringLiteral("qFuzzyCompare(*");
			stream << sdlField.GetId();
			stream << QStringLiteral(", *other.");
			stream << sdlField.GetId();
			stream << QStringLiteral(") : true)");
		}
		else {
			stream << sdlField.GetId();
			stream << QStringLiteral(" == other.");
			stream << sdlField.GetId();
		}

		if (sdlFieldIter.hasNext()){
			stream << QStringLiteral(" &&");
			FeedStream(stream, 1, true);
		}
	}

	stream << ';';
	FeedStream(stream, 1, false);

	stream << '}';
	FeedStream(stream, 3, false);

	return true;
}


bool CSdlClassCodeGeneratorComp::EndClassFiles(const imtsdl::CSdlType& sdlType, QIODevice* headerPtr, QIODevice* sourcePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	const int modifiersCount = m_modifierListCompPtr.GetCount();

	// finish header
	if (headerPtr != nullptr){
		QTextStream headerStream(headerPtr);

		// end of struct
		FeedStreamHorizontally(headerStream, 1);
		headerStream << '}' << ';';
		FeedStream(headerStream, 2, false);

		// member versions
		FeedStreamHorizontally(headerStream);
		headerStream << QStringLiteral("// available version members");
		FeedStream(headerStream, 1, false);

		/// \todo add here ALL versions
		FeedStreamHorizontally(headerStream);
		GenerateVersionMemberDeclaration(headerStream, sdlType, true);
		headerStream << ';';
		FeedStream(headerStream, 2, false);


		// add version-independend read write methods
		if (modifiersCount > 0){
			FeedStreamHorizontally(headerStream);
			headerStream << QStringLiteral("// serialize methods");
		}
		for (int modifierIndex = 0; modifierIndex < modifiersCount; ++modifierIndex){
			ICxxModifier* modifierPtr = m_modifierListCompPtr[modifierIndex];
			Q_ASSERT(modifierPtr != nullptr);

			FeedStream(headerStream, 1, false);

			FeedStreamHorizontally(headerStream);
			GenerateMethodDefinition(headerStream, sdlType, MT_WRITE, *modifierPtr, true);

			FeedStreamHorizontally(headerStream);
			GenerateMethodDefinition(headerStream, sdlType, MT_READ, *modifierPtr, true);

			FeedStreamHorizontally(headerStream);
			GenerateMethodDefinition(headerStream, sdlType, MT_OPT_READ, *modifierPtr, true);
		}

		// end of class
		headerStream << QStringLiteral("};");
		FeedStream(headerStream, 2, false);
	}

	// finish source
	if (sourcePtr != nullptr){
		QTextStream sourceStream(sourcePtr);

		if (modifiersCount > 0){
			sourceStream << QStringLiteral("// serialize methods");
			FeedStream(sourceStream, 2, false);
		}
		for (int modifierIndex = 0; modifierIndex < modifiersCount; ++modifierIndex){
			ICxxModifier* modifierPtr = m_modifierListCompPtr[modifierIndex];
			Q_ASSERT(modifierPtr != nullptr);

			GenerateMethodDefinition(sourceStream, sdlType, MT_WRITE, *modifierPtr, false);
			GenerateMethodImplementation(sourceStream, sdlType, MT_WRITE, *modifierPtr);
			FeedStream(sourceStream, 2, false);

			GenerateMethodDefinition(sourceStream, sdlType, MT_READ, *modifierPtr, false);
			GenerateMethodImplementation(sourceStream, sdlType, MT_READ, *modifierPtr);
			FeedStream(sourceStream, 2, false);

			GenerateMethodDefinition(sourceStream, sdlType, MT_OPT_READ, *modifierPtr, false);
			GenerateMethodImplementation(sourceStream, sdlType, MT_OPT_READ, *modifierPtr);
			FeedStream(sourceStream, 2, false);
		}
		FeedStream(sourceStream, 2, false);
	}

	return true;
}


void CSdlClassCodeGeneratorComp::GenerateMetaInfo(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			uint indents) const
{
	FeedStreamHorizontally(stream, indents);
	// create struct to store field id list
	stream << QStringLiteral("struct ");
	stream << GetCapitalizedValue(sdlType.GetName());
	stream << QStringLiteral("Fields");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, indents);
	stream << '{';

	// create variables for all fields
	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("static const inline QString ");
		stream << GetCapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" = \"");
		stream << sdlField.GetId();
		stream << '"' << ';';
	}

	// finish struct
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, indents);
	stream << '}' << ';';
	FeedStream(stream, 1, false);
}


void CSdlClassCodeGeneratorComp::GenerateVersionStruct(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			uint indents) const
{
	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());

	// main struct begin
	const QString sdlVersion = GetSdlEntryVersion(sdlType);
	FeedStreamHorizontally(stream, indents);
	stream << QStringLiteral("// ");
	stream << sdlVersion;
	stream << QStringLiteral(" struct");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, indents);
	stream << QStringLiteral("struct ");
	stream << sdlVersion;
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, indents);
	stream << '{';
	FeedStream(stream, 1, false);

	// add metainfo
	GenerateMetaInfo(stream, sdlType, indents + 1);
	FeedStream(stream, 1, false);

	// add types members
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		const QString convertedType = OptListConvertTypeWithNamespaceStruct(
			field,
			sdlNamespace,
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			true,
			nullptr,
			nullptr,
			nullptr,
			nullptr);
		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("istd::TSharedNullable<");
		stream << convertedType;
		stream << QStringLiteral("> ");
		stream << field.GetId() << ';';
		FeedStream(stream, 1, false);
	}
	
	FeedStream(stream, 1, false);

	// add GetVersionId method
	FeedStreamHorizontally(stream, indents + 1);
	stream << QStringLiteral("static QByteArray GetVersionId();");
	FeedStream(stream, 2, false);

	// add coparation operators
	FeedStreamHorizontally(stream, indents + 1);
	stream << QStringLiteral("[[nodiscard]] bool operator==(const ");
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("& other) const;");
	FeedStream(stream, 1, false);

	// add inverted coparation operator
	FeedStreamHorizontally(stream, indents + 1);
	stream << QStringLiteral("[[nodiscard]] bool operator!=(const ");
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("& other) const {return !(operator==(other));}");
	FeedStream(stream, 1, false);
}


void CSdlClassCodeGeneratorComp::GenerateMethodDefinition(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			MetdodType methodType,
			ICxxModifier& modifier,
			bool forHeader) const
{
	// type
	if (forHeader){
		stream << QStringLiteral("[[nodiscard]] ");
	}
	stream << QStringLiteral("bool ");

	if (!forHeader){
		stream << 'C' << GetCapitalizedValue(sdlType.GetName()) << ':' << ':';
	}

	// optional?
	switch (methodType){
	case MT_OPT_READ:
	case MT_OPT_WRITE:
		stream << QStringLiteral("Opt");
		break;
	default:
		break;
	}

	// name
	switch (methodType){
	case MT_OPT_READ:
	case MT_READ:
		stream << modifier.GetReadMethodName();
		break;
	case MT_OPT_WRITE:
	case MT_WRITE:
		stream << modifier.GetWriteMethodName();
		break;
	default:
		break;
	}

	stream << '(';
	// arguments
	ICxxModifier::ArgumentList argumentList = modifier.GetArguments();
	Q_ASSERT(!argumentList.isEmpty());

	// container argument
	switch (methodType){
	case MT_OPT_READ:
	case MT_READ:
		stream << QStringLiteral("const ");
		break;
	default:
		break;
	}

	ICxxModifier::Argument containerArgument = argumentList.takeFirst();
	stream << containerArgument.Type << '&' << ' ' << containerArgument.Name;

	// other arguments
	for (const ICxxModifier::Argument& argument: std::as_const(argumentList)){
		stream << ',' << ' ' << argument.Type << ' ' << argument.Name;
		if (forHeader && !argument.DefaultValue.isNull()){
			stream << ' ' << '=' << ' ' << argument.DefaultValue;
		}
	}

	stream << QStringLiteral(", ProtocolVersion version");
	if (forHeader){
		switch (methodType){
			case MT_WRITE:
			case MT_OPT_WRITE:
				stream << QStringLiteral(" = PV_AUTO");
			break;
			default:
				stream << QStringLiteral(" = PV_LAST");
			break;
		}
	}
	stream << ')';

	// const modifier
	switch (methodType){
	case MT_WRITE:
	case MT_OPT_WRITE:
		stream << QStringLiteral(" const");
	break;
	default:
	break;
	}

	// finalize
	if (forHeader){
		stream << ';';
	}

	FeedStream(stream, 1, false);
}


QString CSdlClassCodeGeneratorComp::GetVersionMemberVariableName(
			const imtsdl::CSdlType& sdlType,
			int /*versionIndex*/) const
{
	return QStringLiteral("Version_") + GetSdlEntryVersion(sdlType, false);
}


void CSdlClassCodeGeneratorComp::GenerateVersionMemberDeclaration(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			bool optWrap,
			int versionIndex) const
{
	if (optWrap){
		stream << QStringLiteral("istd::TSharedNullable<");
	}
	stream << GetSdlEntryVersion(sdlType);
	if (optWrap){
		stream << '>';
	}
	stream << ' ' << GetVersionMemberVariableName(sdlType, versionIndex);
}


void CSdlClassCodeGeneratorComp::GenerateMethodImplementation(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			MetdodType methodType,
			ICxxModifier& modifier) const
{
	stream << '{';
	FeedStream(stream, 1, false);

	// auto version check
	switch (methodType){
		case MT_WRITE:
		case MT_OPT_WRITE:
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("if (version == PV_AUTO){");
			FeedStream(stream, 1, false);

			/// \todo add all versions
			// check if version member is valid
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("if (");
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral("){");
			FeedStream(stream, 1, false);

			// call method of  version member
			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("return ");
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral("->");
			GenerateMethodCall(stream, sdlType, methodType, modifier);
			stream << ';';
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << '}';
			FeedStream(stream, 1, false);

			// if all versions is invalid, return
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("else {");
			FeedStream(stream, 1, false);

			AddErrorReport(stream, QStringLiteral("For auto version, 'version' object not initialized."), 3);

			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << '}';
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << '}';
			FeedStream(stream, 1, false);
		break;

		// if AUTO protocol is not allowed
		default:
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("if (version == PV_AUTO){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("qCritical() << \"AUTO protocol is NOT supported for read methods!\";");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("Q_ASSERT_X(false, __func__, \"AUTO protocol is NOT supported for read methods!\");");
			FeedStream(stream, 2, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << '}';
			FeedStream(stream, 1, false);
		break;
	}

	/// \todo add all versions
	// specific versions
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("else if (version == PV_");
	stream << GetSdlEntryVersion(sdlType, false);
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// auto-init(for read) and checks(for write)
	switch (methodType){
		case MT_READ:
		case MT_OPT_READ:
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("if (!");
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral("){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 3);
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral(" = ");
			stream << GetSdlEntryVersion(sdlType);
			stream << QStringLiteral("();");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << '}';
			FeedStream(stream, 2, false);
		break;
		default:
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("if (!");
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral("){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("qCritical() << \"Uninitialized version member\";");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("Q_ASSERT_X(false, __func__, \"Uninitialized version member\");");
			FeedStream(stream, 2, false);

			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << '}';
			FeedStream(stream, 2, false);
		break;
	}

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return ");
	stream << GetVersionMemberVariableName(sdlType);
	stream << QStringLiteral("->");
	GenerateMethodCall(stream, sdlType, methodType, modifier);
	stream << ';';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << '}';
	FeedStream(stream, 2, false);

	// default error if invalid version and return
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("qCritical() << \"Invalid version\";");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_ASSERT_X(false, __func__, \"Invalid version\");");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	// end of method
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlClassCodeGeneratorComp::GenerateMethodCall(
			QTextStream& stream,
			const imtsdl::CSdlType& /*sdlType*/,
			MetdodType methodType,
			ICxxModifier& modifier) const
{
	// optional?
	switch (methodType){
	case MT_OPT_READ:
	case MT_OPT_WRITE:
		stream << QStringLiteral("Opt");
		break;
	default:
		break;
	}

	// name
	switch (methodType){
	case MT_OPT_READ:
	case MT_READ:
		stream << modifier.GetReadMethodName();
		break;
	case MT_OPT_WRITE:
	case MT_WRITE:
		stream << modifier.GetWriteMethodName();
		break;
	default:
		break;
	}

	stream << '(';
	// arguments
	ICxxModifier::ArgumentList argumentList = modifier.GetArguments();
	Q_ASSERT(!argumentList.isEmpty());

	ICxxModifier::Argument containerArgument = argumentList.takeFirst();
	stream << containerArgument.Name;

	// other arguments
	for (const ICxxModifier::Argument& argument: std::as_const(argumentList)){
		stream << ',' << ' ' << argument.Name;
	}
	stream << ')';
}


} // namespace imtsdlgencpp

