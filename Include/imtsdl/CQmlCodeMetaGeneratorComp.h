#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <istd/TDelPtr.h>
#include <iproc/TSyncProcessorCompBase.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CGqlSchemaParser.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{

/**
	A base QML generator of SDL types
*/
class CQmlCodeMetaGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			public CGqlSchemaParser,
			private CSdlTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;
	typedef CGqlSchemaParser BaseClass2;

	I_BEGIN_COMPONENT(CQmlCodeMetaGeneratorComp)
		I_ASSIGN(m_qmlKeysFileSuffixAttrPtr, "QmlKeysFileSuffix", "QML Keys file's suffix for file name.\nNote: default value with underscore to avoid ambiguity", true, "_MetaInfo")
		/// \todo remove it! 2
		I_ASSIGN(m_qmlCommandIdsFileSuffixAttrPtr, "QmlCommandIdFileSuffix", "QML command IDs file's suffix for file name.\nNote: default value with underscore to avoid ambiguity", true, "_GqlCommandIds")
		I_ASSIGN(m_qmlDocumentTypesFileSuffixAttrPtr, "QmlDocumentTypesFileSuffix", "QML command IDs file's suffix for file name.\nNote: default value with underscore to avoid ambiguity", true, "_GqlDocumentTypes")
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
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

	/// \todo remove it!
	QString GetQmlCommandIdsWrappedName(const QString& originalName) const;

private:
	I_ATTR(QString, m_qmlKeysFileSuffixAttrPtr);
	/// \todo remove it! 2
	I_ATTR(QString, m_qmlCommandIdsFileSuffixAttrPtr);
	I_ATTR(QString, m_qmlDocumentTypesFileSuffixAttrPtr);
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(ISdlTypeListProvider, m_sdlTypeListCompPtr);

	istd::TDelPtr<QFile> m_qmlKeysFilePtr;
};


} // namespace imtsdl

