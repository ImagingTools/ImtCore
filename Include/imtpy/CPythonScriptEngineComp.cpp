#include <imtpy/CPythonScriptEngineComp.h>


// Qt includes
#include <QtCore/QFileInfo.h>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <ifile/IFileNameParam.h>
#include <ifile/CFileListProviderComp.h>
#include <iproc/IProcessor.h>

// PyBind includes
#undef slots
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

// Cells includes
#include <imtpy/CPythonOutputRedirector.h>


namespace imtpy
{


// reimplemented (IScriptEngine)

bool CPythonScriptEngineComp::InitializeEngine(const iprm::IParamsSet* paramsPtr)
{
	istd::CChangeNotifier changeNotifier(this);

	m_scriptInfoList.ResetOptions();
	m_scripts.clear();

	iprm::TParamsPtr<ifile::IFileNameParam> rootFolderParamPtr(paramsPtr != nullptr ? paramsPtr : m_defaultParamsCompPtr.GetPtr(), *m_rootFolderParamIdAttrPtr);
	if (rootFolderParamPtr.IsValid()){
		QString rootFolderPath = rootFolderParamPtr->GetPath();

		QFileInfoList scriptFiles;
		if (ifile::CFileListProviderComp::CreateFileList(QDir(rootFolderPath), 0, 1, QStringList() << "*.py", QDir::Name, scriptFiles)){
			for (int i = 0; i < scriptFiles.count(); ++i){
				QByteArray scriptId = AddScript(scriptFiles[i].absoluteFilePath(), nullptr);
				if (!scriptId.isEmpty()){
					m_scriptInfoList.InsertOption(scriptFiles[i].baseName(), scriptId, scriptFiles[i].absoluteFilePath());
				}
			}
		}
	}

	return true;
}


const iprm::IOptionsList& CPythonScriptEngineComp::GetScriptInfoList() const
{
	return m_scriptInfoList;
}


QByteArray CPythonScriptEngineComp::AddScript(const QString& scriptFilePath, IScriptManager* managerPtr)
{
	if (QFileInfo(scriptFilePath).exists()){
		istd::CChangeNotifier changeNotifier(this);

		ScriptPtr scriptPtr(new Script);
		scriptPtr->filePath = scriptFilePath;
		scriptPtr->moduleName = QFileInfo(scriptFilePath).baseName();

		bool retVal = false;
		if ((managerPtr != nullptr) && managerPtr->SetupScriptMetaInfo(scriptPtr->filePath, *scriptPtr, scriptPtr->functionName)){
			retVal = true;
		}

		if (retVal){
			Q_ASSERT(managerPtr != nullptr);

			m_scriptManagerMap[scriptPtr->uuid] = managerPtr;

			AddSearchPath(QFileInfo(scriptFilePath).absolutePath());

			m_scripts.push_back(scriptPtr);

			return scriptPtr->uuid;
		}
	}

	return QByteArray();
}


void CPythonScriptEngineComp::RemoveScript(const QByteArray& scriptId)
{
	for (int i = 0; i < m_scripts.count(); ++i){
		if (m_scripts[i]->uuid == scriptId){
			istd::CChangeNotifier changeNotifier(this);

			m_scripts.removeAt(i);
			m_scriptManagerMap.remove(scriptId);

			break;
		}
	}
}


const iattr::IAttributesMetaInfoProvider* CPythonScriptEngineComp::GetAttributeMetaInfoManager(const QByteArray& scriptId) const
{
	ScriptPtr scriptPtr = FindScriptById(scriptId);
	if (!scriptPtr.IsValid()){
		return nullptr;
	}

	return scriptPtr.GetPtr();
}


iattr::IAttributesProvider* CPythonScriptEngineComp::GetAttributeManager(const QByteArray& scriptId) const
{
	ScriptPtr scriptPtr = FindScriptById(scriptId);
	if (!scriptPtr.IsValid()){
		return nullptr;
	}

	return scriptPtr.GetPtr();
}


bool CPythonScriptEngineComp::SetAttributeValue(
			const QByteArray& scriptId,
			const QByteArray& attributeId,
			const iser::IObject* valuePtr)
{
	ScriptPtr scriptPtr = FindScriptById(scriptId);
	if (!scriptPtr.IsValid()){
		return false;
	}

	if (valuePtr != nullptr){
		iser::IObject* attributeObjectPtr = scriptPtr->GetAttribute(attributeId);
		if (attributeObjectPtr != NULL){
			return attributeObjectPtr->CopyFrom(*valuePtr);
		}
	}

	return true;
}


int CPythonScriptEngineComp::ExecuteScript(
			const QByteArray& scriptId,
			ibase::IProgressManager* /*progressPtr*/,
			ilog::IMessageConsumer* /*logPtr*/) const
{
	// Get script by ID:
	ScriptPtr scriptPtr = FindScriptById(scriptId);
	if (!scriptPtr.IsValid()){
		return iproc::IProcessor::TS_INVALID;
	}

	// Prepare Python environment:
	const QString appPath = qApp->applicationDirPath();
	AddSearchPath(appPath);

	// Execute script:
	int retVal = iproc::IProcessor::TS_OK;

	try{
		pybind11::module moduleInstance = pybind11::module_::import(scriptPtr->moduleName.toUtf8().constData());

		Q_ASSERT(m_scriptManagerMap.contains(scriptId));

		IScriptManager* managerPtr = m_scriptManagerMap[scriptId];
		if (managerPtr != nullptr){
			CPythonOutputRedirector pythonOutputRedirector;

			bool success = managerPtr->RunMethod(*scriptPtr, moduleInstance, scriptPtr->functionName, scriptPtr->filePath);

			QString errorMessageBuffer = QString::fromStdString(pythonOutputRedirector.GetErrorOutput());
			QStringList errorMessages = errorMessageBuffer.split("\n");
			for(const QString& errorMessage : errorMessages){
				if (!errorMessage.isEmpty()){
					SendErrorMessage(0, errorMessage, scriptPtr->moduleName);
				}
			}

			QString infoMessageBuffer = QString::fromStdString(pythonOutputRedirector.GetStdOutput());
			QStringList infoMessages = infoMessageBuffer.split("\n");
			for(const QString& infoMessage : infoMessages){
				if (!infoMessage.isEmpty()){
					SendInfoMessage(0, infoMessage, scriptPtr->moduleName);
				}
			}

			if (!success){
				retVal = iproc::IProcessor::TS_INVALID;
			}
		}
	}
	catch (std::exception& e){
		SendErrorMessage(0, QString::fromStdString(e.what()));

		retVal = iproc::IProcessor::TS_INVALID;
	}

	return retVal;
}


// protected methods

CPythonScriptEngineComp::ScriptPtr CPythonScriptEngineComp::FindScriptById(const QByteArray& scriptId) const
{
	for (ScriptPtr script : m_scripts){
		if (script->uuid == scriptId){
			return script;
		}
	}

	return ScriptPtr();
}


void CPythonScriptEngineComp::AddSearchPath(const QString& path)
{
	using namespace pybind11::literals;
	auto locals = pybind11::dict("searchPath"_a = path.toStdString());

	pybind11::exec(R"(
				import sys
				sys.path.append(searchPath))",
				pybind11::globals(),
				locals);
}


// reimplemented (icomp::CComponentBase)

void CPythonScriptEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	InitializeEngine();
}


// public methods of the embedded class Script

// reimplemented (IAttributesMetaInfoProvider)

iattr::IAttributesProvider::AttributeIds CPythonScriptEngineComp::Script::GetAttributeMetaIds() const
{
	return IAttributesProvider::AttributeIds();
}


const iattr::IAttributeMetaInfo* CPythonScriptEngineComp::Script::GetAttributeMetaInfo(const QByteArray& attributeId) const
{
	return nullptr;
}


} // namespace imtpy


