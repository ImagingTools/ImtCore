#pragma once


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iattr/CAttributesManager.h>
#include <iprm/COptionsManager.h>

// Cells includes
#include <imtpy/IScriptEngine.h>


namespace imtpy
{


class CPythonScriptEngineComp:
			public ilog::CLoggerComponentBase,
			virtual public IScriptEngine
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef iattr::CAttributesManager BaseClass2;

	I_BEGIN_COMPONENT(CPythonScriptEngineComp);
		I_REGISTER_INTERFACE(IScriptEngine);
		I_ASSIGN(m_rootFolderParamIdAttrPtr, "RootFolderParamId", "ID of the root folder parameter in the parameter set", true, "ScriptFolder");
		I_ASSIGN(m_defaultParamsCompPtr, "DefaultEngineParams", "Default parameters used for engine initialization", false, "DefaultEngineParams");
	I_END_COMPONENT;

	// reimplemented (IScriptEngine)
	virtual bool InitializeEngine(const iprm::IParamsSet* paramsPtr = nullptr) override;
	virtual const iprm::IOptionsList& GetScriptInfoList() const override;
	virtual QByteArray AddScript(const QString& scriptFilePath, IScriptManager* managerPtr) override;
	virtual void RemoveScript(const QByteArray& scriptId) override;
	virtual const iattr::IAttributesMetaInfoProvider* GetAttributeMetaInfoManager(const QByteArray& scriptId) const override;
	virtual iattr::IAttributesProvider* GetAttributeManager(const QByteArray& scriptId) const override;
	virtual bool SetAttributeValue(const QByteArray& scriptId, const QByteArray& attributeId, const iser::IObject* valuePtr) override;
	virtual int ExecuteScript(const QByteArray& scriptId, ibase::IProgressManager* progressPtr, ilog::IMessageConsumer* logPtr) const override;

protected:
	struct Script: public iattr::CAttributesManager, virtual public iattr::IAttributesMetaInfoProvider
	{
		Script()
		{
			uuid = QUuid::createUuid().toByteArray();
		}

		QString filePath;
		QByteArray uuid;
		QString moduleName;
		QString functionName;

		// reimplemented (IAttributesMetaInfoProvider)
		virtual iattr::IAttributesProvider::AttributeIds GetAttributeMetaIds() const override;
		virtual const iattr::IAttributeMetaInfo* GetAttributeMetaInfo(const QByteArray & attributeId) const override;
	};

	typedef istd::TSmartPtr<Script> ScriptPtr;
	typedef QVector<ScriptPtr> Scripts;

protected:
	ScriptPtr FindScriptById(const QByteArray& scriptId) const;

	static void AddSearchPath(const QString& path);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	Scripts m_scripts;
	iprm::COptionsManager m_scriptInfoList;

	typedef QMap<QByteArray, IScriptManager*> IScriptManagerMap;
	IScriptManagerMap m_scriptManagerMap;

	I_ATTR(QByteArray, m_rootFolderParamIdAttrPtr);
	I_REF(iprm::IParamsSet, m_defaultParamsCompPtr);
};


} // namespace imtpy


