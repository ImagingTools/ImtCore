#pragma once


//Qt includes
#include "icomp/CComponentBase.h"
#include <QtCore/QFile>

// ACF includes
#include <QtCore/qstring.h>
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/CSdlTools.h>

namespace imtsdlgen
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
		I_ASSIGN(m_qmlEnumValuesFileSuffixAttrPtr, "EnumValuesFileSuffix", "QML flie's suffix for the file name", true, "EnumValues")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_qmldirFilePersistanceCompPtr, "QmldirFilePersistance", "Qmldir file persistance, used to update qmldir file with new modules", true, "QmldirFilePersistance")
		I_ASSIGN(m_customSchemaParamsCompPtr, "CustomSchemaParams", "Custom schema parameters, that contains additional options", false, "CustomSchemaParams")
	I_END_COMPONENT

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	bool ReOpenFiles();
	bool CloseFiles();
	bool BeginQmlFile(const imtsdl::CSdlType& sdlType);
	bool EndQmlFile(const imtsdl::CSdlType& sdlType);
	void AbortCurrentProcessing();
	QString GetQmlKeysWrappedName(const QString& originalName) const;

private:
	I_ATTR(QString, m_qmlKeysFileSuffixAttrPtr);
	I_ATTR(QString, m_qmlEnumValuesFileSuffixAttrPtr);
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(ifile::IFilePersistence, m_qmldirFilePersistanceCompPtr);
	I_REF(iprm::IParamsSet, m_customSchemaParamsCompPtr);

	istd::TDelPtr<QFile> m_qmlKeysFilePtr;
};


} // namespace imtsdlgen

