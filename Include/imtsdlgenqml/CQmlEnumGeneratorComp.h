// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>
#include <ifile/IFilePersistence.h>
#include <iprm/IParamsManager.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>
#include <imtsdl/CSdlTools.h>

namespace iprm
{
class CParamsSet;
}


namespace imtsdlgenqml
{

/**
	A base QML generator of SDL types
*/
class CQmlEnumGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CQmlEnumGeneratorComp)
		I_ASSIGN(m_qmlFileSuffixAttrPtr, "QmlFileSuffix", "QML file's suffix for file name.", true, "Enum")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlEnumListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_qmldirFilePersistanceCompPtr, "QmldirFilePersistance", "Qmldir file persistance, used to update qmldir file with new modules", true, "QmldirFilePersistance")
		I_ASSIGN(m_qrcFilePersistanceCompPtr, "QrResourceFilePersistance", "QRC file persistance, used to update file with new modules", true, "QrResourceFilePersistance")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
		I_ASSIGN(m_paramsFactComp, "QrcParamsFactory", "Qt resource model, used to update resource file with new modules", true, "QrcParamsFactory")
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
	static bool UpdateQmldirParams(iprm::IParamsSet& qrcParamsPtr, const QList<QmldirFileInfo>& infoList);

private:
	bool ReOpenFiles();
	bool CloseFiles();
	bool ProcessQmlFile(const imtsdl::CSdlEnum& sdlEnum);
	void AbortCurrentProcessing();
	[[nodiscard]] QString GetQmlWrappedName(const QString& originalName) const;
	[[nodiscard]] QString GetQmlFileName(const QString& originalName) const;
	[[nodiscard]] QString GetQmlFilePath(const QString& originalName) const;
	/// \todo move it in CSdlQmlTools
	bool AddQrcEntry(const imtsdl::CSdlEnum& sdlEnum, iprm::CParamsSet& qmldirDataParams) const;
	bool AddQmldirEntry(const imtsdl::CSdlEnum& sdlEnum, iprm::IParamsManager& qrcParamsPtr) const;

private:
	I_ATTR(QString, m_qmlFileSuffixAttrPtr);
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlEnumListProvider, m_sdlEnumListCompPtr);
	I_REF(ifile::IFilePersistence, m_qmldirFilePersistanceCompPtr);
	I_REF(ifile::IFilePersistence, m_qrcFilePersistanceCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);
	I_FACT(iprm::IParamsManager, m_paramsFactComp);

	istd::TDelPtr<QFile> m_qmlFilePtr;
};


} // namespace imtsdlgenqml

