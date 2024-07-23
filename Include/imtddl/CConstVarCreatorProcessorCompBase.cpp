#include <imtddl/CConstVarCreatorProcessorCompBase.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QTextStream>

// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>
#include <istd/CSystem.h>


namespace imtddl
{


bool CConstVarCreatorProcessorCompBase::OpenFiles(const iprm::IParamsSet* /*paramsPtr*/)
{
	return true;
}


bool CConstVarCreatorProcessorCompBase::OpenFile(const QString filePath, QFile& file, QFile::OpenMode openMode)
{
	QFileInfo fileInfo(filePath);

	if (!istd::CSystem::EnsurePathExists(fileInfo.dir().absolutePath())){
		SendErrorMessage(__LINE__, QObject::tr("Cannot create path to file") + ": " + filePath);

		return false;
	}

	file.setFileName(filePath);

	if (!file.open(openMode)){
		SendErrorMessage(__LINE__, QString("Cannot read file '%1'; Error: '%2'").arg(file.fileName(), file.errorString()));

		return false;
	}

	return true;
}



bool CConstVarCreatorProcessorCompBase::CloseFiles(const iprm::IParamsSet* /*paramsPtr*/)
{
	return true;
}


QString CConstVarCreatorProcessorCompBase::GetTemplateDirPath(const iprm::IParamsSet* paramsPtr) const
{
	iprm::TParamsPtr<iprm::ITextParam> dirPathParamPtr(paramsPtr, "TemplateDirPath");

	if (!dirPathParamPtr.IsValid()){
		SendErrorMessage(__LINE__, QString("Template directory path is invalid"));

		return QString();
	}

	return dirPathParamPtr->GetText();
}


QString CConstVarCreatorProcessorCompBase::GetCppDirPath(const iprm::IParamsSet *paramsPtr) const
{
	iprm::TParamsPtr<iprm::ITextParam> dirPathParamPtr(paramsPtr, "CppDirPath");

	if (!dirPathParamPtr.IsValid()){
		SendErrorMessage(__LINE__, QString("Cpp directory path is invalid"));

		return QString();
	}

	return dirPathParamPtr->GetText();
}


QString CConstVarCreatorProcessorCompBase::GetQmlDirPath(const iprm::IParamsSet *paramsPtr) const
{
	iprm::TParamsPtr<iprm::ITextParam> dirPathParamPtr(paramsPtr, "QmlDirPath");

	if (!dirPathParamPtr.IsValid()){
		SendErrorMessage(__LINE__, QString("Qml directory path is invalid"));

		return QString();
	}

	return dirPathParamPtr->GetText();
}


QString CConstVarCreatorProcessorCompBase::GetModuleName(const iprm::IParamsSet* /*paramsPtr*/)
{
	return QString();
}


bool CConstVarCreatorProcessorCompBase::CreateBody(const QString moduleName, const QJsonDocument& /*templateDocument*/, const iprm::IParamsSet* /*paramsPtr*/)
{
	return false;
}


// reimplemented (iproc::IProcessor)

int CConstVarCreatorProcessorCompBase::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	QString moduleName = GetModuleName(paramsPtr);

	if (moduleName.isEmpty()){
		SendErrorMessage(__LINE__, QString("The module name is invalid"));

		return TS_INVALID;
	}

	iprm::TParamsPtr<iprm::ITextParam> templateDirPathParamPtr(paramsPtr, "TemplateDirPath");
	if (!templateDirPathParamPtr.IsValid()){
		SendErrorMessage(__LINE__, QString("The template dir path is invalid"));

		return TS_INVALID;
	}

	if (!OpenFiles(paramsPtr)){
		return TS_INVALID;
	}

	QString templateDirPath = templateDirPathParamPtr->GetText();
	QDir templateDir(templateDirPath);
	templateDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList list = templateDir.entryInfoList();

	for (QFileInfo fileInfo: list){
		QFile templateFile(fileInfo.filePath());

		if (!templateFile.open(QFile::ReadOnly)){
			SendErrorMessage(__LINE__, QString("Cannot read file '%1'; Error: '%2'").arg(templateFile.fileName(), templateFile.errorString()));

			return TS_INVALID;
		}

		QByteArray fileData = templateFile.readAll();
		QJsonParseError jsonParseError;
		QJsonDocument templateDocument = QJsonDocument::fromJson(fileData, &jsonParseError);

		if (jsonParseError.error != QJsonParseError::NoError){
			SendErrorMessage(__LINE__, QString("The document template is invalid. Error: '%1'").arg(jsonParseError.errorString()));

			return TS_INVALID;
		}

		CreateBody(moduleName, templateDocument, paramsPtr);
	}

	if (!CloseFiles(paramsPtr)){
		return TS_INVALID;
	}

	return TS_OK;
}


} // namespace imtddl


