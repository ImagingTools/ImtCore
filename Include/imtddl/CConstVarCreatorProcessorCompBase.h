// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QFile>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/TSyncProcessorCompBase.h>


namespace imtddl
{

class CConstVarCreatorProcessorCompBase: public iproc::CSyncProcessorCompBase
{
public:
	typedef iproc::CSyncProcessorCompBase BaseClass;

protected:
	virtual bool OpenFiles(const iprm::IParamsSet* paramsPtr);
	virtual bool OpenFile(const QString& filePath, QFile& file, QFile::OpenMode openMode = QFile::ReadOnly);
	virtual bool CloseFiles(const iprm::IParamsSet* paramsPtr);
	virtual QString GetTemplateDirPath(const iprm::IParamsSet* paramsPtr) const;
	virtual QString GetCppDirPath(const iprm::IParamsSet* paramsPtr) const;
	virtual QString GetQmlDirPath(const iprm::IParamsSet* paramsPtr) const;
//	virtual QByteArray GetOutputFileKey();
	virtual QString GetModuleName(const iprm::IParamsSet* paramsPtr);
	virtual bool CreateBody(const QString& moduleName, const QJsonDocument& templateDocument, const iprm::IParamsSet* paramsPtr);

	// reimplemented (iproc::IProcessor)
	virtual TaskState DoProcessing(
		const iprm::IParamsSet* paramsPtr,
		const istd::IPolymorphic* inputPtr,
		istd::IChangeable* outputPtr,
		ibase::IProgressManager* progressManagerPtr = NULL) override;

protected:
//	I_REF(IDdlCodeCreatorArgumentsParser, m_argumentParserAttrPtr);
};


} // namespace imtddl

