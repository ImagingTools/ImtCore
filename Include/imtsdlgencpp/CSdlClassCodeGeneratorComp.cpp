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

	// add metainfo (fields struct)
	GenerateMetaInfo(stream, sdlType);
	FeedStream(stream, 1, false);

	// add field members directly in the class
	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());
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
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("istd::TSharedNullable<");
		stream << convertedType;
		stream << QStringLiteral("> ");
		stream << field.GetId() << ';';
		FeedStream(stream, 1, false);
	}

	FeedStream(stream, 1, false);

	// add GetVersionId method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("static QByteArray GetVersionId();");
	FeedStream(stream, 2, false);

	// add comparison operators
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("[[nodiscard]] bool operator==(const C");
	stream << sdlType.GetName();
	stream << QStringLiteral("& other) const;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("[[nodiscard]] bool operator!=(const C");
	stream << sdlType.GetName();
	stream << QStringLiteral("& other) const {return !(operator==(other));}");
	FeedStream(stream, 1, false);

	stream.flush();

	return true;
}


bool CSdlClassCodeGeneratorComp::BeginSourceClassFile(const imtsdl::CSdlType& sdlType, QIODevice* sourcePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream stream(sourcePtr);

	//implement GetVersionId method
	stream << QStringLiteral("QByteArray C");
	stream << sdlType.GetName();
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

	// implement comparison operator
	stream << QStringLiteral("bool C");
	stream << sdlType.GetName();
	stream << QStringLiteral("::operator==(const C");
	stream << sdlType.GetName();
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
	Q_UNUSED(sdlType);

	// finish header
	if (headerPtr != nullptr){
		QTextStream headerStream(headerPtr);

		// end of class
		headerStream << QStringLiteral("};");
		FeedStream(headerStream, 2, false);
	}

	// finish source (nothing needed - modifiers already wrote implementations)
	if (sourcePtr != nullptr){
		QTextStream sourceStream(sourcePtr);
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


} // namespace imtsdlgencpp

