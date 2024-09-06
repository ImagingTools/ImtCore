#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>
#include <ifile/IFilePersistence.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/CSdlTools.h>

namespace imtsdl
{

/**
	A base QML generator of SDL types
*/
class CQmlCodeMetaGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CQmlCodeMetaGeneratorComp)
		I_ASSIGN(m_qmlKeysFileSuffixAttrPtr, "QmlKeysFileSuffix", "QML Keys file's suffix for file name.\nNote: default value with underscore to avoid ambiguity", true, "MetaInfo")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_qmldirFilePersistanceCompPtr, "QmldirFilePersistance", "Qmldir file persistance, used to update qmldir file with new modules", true, "QmldirFilePersistance")
	I_END_COMPONENT;

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool ReOpenFiles();
	bool CloseFiles();
	bool BeginQmlFile(const CSdlType& sdlType);
	bool EndQmlFile(const CSdlType& sdlType);
	void AbortCurrentProcessing();
	QString GetQmlKeysWrappedName(const QString& originalName) const;

private:
	I_ATTR(QString, m_qmlKeysFileSuffixAttrPtr);
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(ifile::IFilePersistence, m_qmldirFilePersistanceCompPtr);

	istd::TDelPtr<QFile> m_qmlKeysFilePtr;
};


} // namespace imtsdl

