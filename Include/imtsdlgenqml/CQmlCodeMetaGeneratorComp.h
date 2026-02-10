// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


//Qt includes
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

namespace imtsdlgenqml
{

/**
	A base QML generator of SDL types
*/
class CQmlCodeMetaGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CQmlCodeMetaGeneratorComp)
		I_ASSIGN(m_qmlKeysFileSuffixAttrPtr, "QmlKeysFileSuffix", "QML Keys file's suffix for file name.", true, "TypeMetaInfo")
		I_ASSIGN(m_collectionInfoFileMaskAttrPtr, "CollectionInfoFileNameMask", "Mask, used to name a collectio info QML file. \nSupported placeholders:\n\t$(Type); $(Subtype)", true, "$(Type)$(Subtype)CollectionInfo")
		I_ASSIGN(m_qmlEnumValuesFileSuffixAttrPtr, "EnumValuesFileSuffix", "QML flie's suffix for the file name", true, "EnumValues")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_sdlDocumentListCompPtr, "SdlDocumentListProvider", "SDL documents used to create a GraphQL wrap code", true, "SdlDocumentListProvider")
		I_ASSIGN(m_qmldirFilePersistanceCompPtr, "QmldirFilePersistance", "Qmldir file persistance, used to update qmldir file with new modules", true, "QmldirFilePersistance")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
	I_END_COMPONENT

	//reimplemented(iproc::IProcessor)
	virtual iproc::IProcessor::TaskState DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	/// \todo generalize it maybe in base library...
	struct QmldirFileInfo
	{
		bool IsSingleton = false;
		QString Type;
		QString Version;
		QString FileName;
	};
	static bool UpdateQmldirParams(iprm::IParamsSet& qmldirDataParams, const QList<QmldirFileInfo>& infoList);

private:
	bool ReOpenFiles();
	bool CloseFiles();
	bool BeginQmlFile(const imtsdl::CSdlType& sdlType);
	bool EndQmlFile(const imtsdl::CSdlType& sdlType);
	void AbortCurrentProcessing();
	bool GenerateCollectionSchemaInfo();
	bool GenerateCollectionSchemaInfoForType(const imtsdl::CSdlDocumentType& documentType, const QString& typeName, const QString& subtypeName);
	[[nodiscard]] QString GetQmlKeysWrappedName(const QString& originalName) const;
	[[nodiscard]] QString GetQmlCollectionInfoWrappedName(const QString& typeName, const QString& subtypeName) const;

private:
	I_ATTR(QString, m_qmlKeysFileSuffixAttrPtr);
	I_ATTR(QString, m_collectionInfoFileMaskAttrPtr);
	I_ATTR(QString, m_qmlEnumValuesFileSuffixAttrPtr);
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(imtsdl::ISdlDocumentTypeListProvider, m_sdlDocumentListCompPtr);
	I_REF(ifile::IFilePersistence, m_qmldirFilePersistanceCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);

	istd::TDelPtr<QFile> m_qmlKeysFilePtr;
};


} // namespace imtsdlgenqml

