// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iprm/IParamsManager.h>
#include <iproc/TSyncProcessorCompBase.h>
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/CSdlTools.h>

namespace imtsdlgenqml
{

/**
	A base QML generator of SDL types
*/
class CQmlCodeCommandIdGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CQmlCodeCommandIdGeneratorComp)
		I_ASSIGN(m_qmlCommandIdsFileSuffixAttrPtr, "QmlCommandIdFileSuffix", "QML command IDs file's suffix for file name.\nNote: default value with underscore to avoid ambiguity", true, "CommandIds")
		/// \todo remove/separate it!
		I_ASSIGN(m_qmlDocumentTypesFileSuffixAttrPtr, "QmlDocumentTypesFileSuffix", "QML command IDs file's suffix for file name.\nNote: default value with underscore to avoid ambiguity", true, "DocumentTypes")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlRequestListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_qmldirFilePersistanceCompPtr, "QmldirFilePersistance", "Qmldir file persistance, used to update qmldir file with new modules", true, "QmldirFilePersistance")
		I_ASSIGN(m_qrcFilePersistanceCompPtr, "QrcFilePersistance", "QRC file persistance, used to update resource file with new modules", true, "QrcFilePersistance")
		I_ASSIGN(m_paramsFactComp, "QrcParamsFactory", "Qt resource model, used to update resource file with new modules", true, "QrcParamsFactory")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")

	I_END_COMPONENT

	//reimplemented(iproc::IProcessor)
	virtual iproc::IProcessor::TaskState DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool ReOpenFiles();
	bool CloseFiles();
	bool ProcessQmlFile();
	void AbortCurrentProcessing();
	QString GetQmlCommandIdsFileName() const;
	QString GetQmlCommandIdsFilePath() const;

private:
	I_ATTR(QString, m_qmlCommandIdsFileSuffixAttrPtr);
	/// \todo remove/separate it!
	I_ATTR(QString, m_qmlDocumentTypesFileSuffixAttrPtr);
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlRequestListProvider, m_sdlRequestListCompPtr);
	I_REF(ifile::IFilePersistence, m_qmldirFilePersistanceCompPtr);
	I_REF(ifile::IFilePersistence, m_qrcFilePersistanceCompPtr);
	I_FACT(iprm::IParamsManager, m_paramsFactComp);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);

	istd::TDelPtr<QFile> m_qmlCommandsFilePtr;
};


} // namespace imtsdlgenqml

