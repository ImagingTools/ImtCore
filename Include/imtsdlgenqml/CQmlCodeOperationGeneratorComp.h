// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <QtCore/qstring.h>
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdl/ISdlRequestListProvider.h>


namespace imtsdlgenqml
{


/**
	\brief Generates a typed \c <RequestName>Operation.qml file for every
	SDL Query or Mutation declared in the schema.

	\details Each generated file extends the \c SdlOperation base component
	and provides:
	\list
	\li Typed QML properties for every field of the SDL input type.
	\li A pre-allocated typed \c result instance matching the SDL output type.
	\li An \c execute() override that populates the input object and
	    dispatches the request via \c _sendRequest().
	\endlist

	The generator is wired into \c QmlCodeGenFramework after the existing
	\c QmlCodeRequestGenerator, so both the legacy \c ModelProvider /
	\c ModelController files and the new \c Operation files are produced
	side-by-side, maintaining full backward compatibility.

	Subscriptions are intentionally skipped in this MVP.
*/
class CQmlCodeOperationGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CQmlCodeOperationGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlDocumentListCompPtr, "SdlDocumentListProvider", "SDL documents used to create a GraphQL wrap code", true, "SdlDocumentListProvider")
		I_ASSIGN(m_qmldirFilePersistanceCompPtr, "QmldirFilePersistance", "Qmldir file persistance, used to update qmldir file with new modules", true, "QmldirFilePersistance")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
		I_ASSIGN(m_sdlRequestListCompPtr, "SdlRequestListProvider", "SDL requests used to create a GraphQL wrap code", true, "SdlRequestListProvider")
	I_END_COMPONENT

	// reimplemented (iproc::IProcessor)
	virtual iproc::IProcessor::TaskState DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	/// \brief QML property type and default value for a single SDL input field.
	struct InputPropertyInfo
	{
		QString qmlType;    ///< QML property type keyword (string, int, bool, real, var)
		QString defaultVal; ///< QML default value literal
	};

	struct QmldirFileInfo
	{
		bool IsSingleton = false;
		QString Type;
		QString Version;
		QString FileName;
	};

	static bool UpdateQmldirParams(iprm::IParamsSet& qmldirDataParams, const QList<QmldirFileInfo>& infoList);

	/// \brief Resolves the QML import declaration string for an SDL field's type.
	QString GetQmlImportDeclarationByField(const imtsdl::CSdlField& sdlField) const;

	/// \brief Maps an SDL scalar type name + required flag to a QML property
	///        type keyword and its default value literal.
	static InputPropertyInfo ResolveInputPropertyInfo(const imtsdl::CSdlField& sdlField);

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlDocumentTypeListProvider, m_sdlDocumentListCompPtr);
	I_REF(ifile::IFilePersistence, m_qmldirFilePersistanceCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_REF(imtsdl::ISdlRequestListProvider, m_sdlRequestListCompPtr);
};


} // namespace imtsdlgenqml
