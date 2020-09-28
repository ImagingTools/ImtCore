#pragma once


// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IOptionsList.h>
#include <ibase/IProgressManager.h>
#include <ilog/IMessageConsumer.h>
#include <iattr/IAttributesMetaInfoProvider.h>
#include <iattr/IAttributesManager.h>

// ImtCore includes
#include <imtpy/IScriptRunner.h>


namespace imtpy
{


class IScriptEngine: virtual public istd::IChangeable
{
public:
	/**
		Initialize the script engine with the given parameters.
	*/
	virtual bool InitializeEngine(const iprm::IParamsSet* paramsPtr = nullptr) = 0;

	/**
		Get the informations about available/registered scripts.
	*/
	virtual const iprm::IOptionsList& GetScriptInfoList() const = 0;

	/**
		Create script object for a given file.
		The method returns an UUID for the created script instance or an empty ID, if the operation failed.
	*/
	virtual QByteArray RegisterScript(const QString& scriptFilePath, IScriptRunner* runnerPtr = nullptr) = 0;

	/**
		Remove all script-related data from the engine.
	*/
	virtual void UnregisterScript(const QByteArray& scriptId) = 0;

	/**
		Get access to the provider of the meta informations for the script attributes.
	*/
	virtual const iattr::IAttributesMetaInfoProvider* GetAttributeMetaInfoManager(const QByteArray& scriptId) const = 0;

	/**
		Get access to the provider of the script attributes.
	*/
	virtual iattr::IAttributesProvider* GetAttributeManager(const QByteArray& scriptId) const = 0;

	/**
		Set attribute value to a given attribute of the defined script.
	*/
	virtual bool SetAttributeValue(
				const QByteArray& scriptId,
				const QByteArray& attributeId,
				const iser::IObject* valuePtr) = 0;

	/**
		Execute script with the given ID.
		The method returns the processing state as a iproc::IProcessor::TaskState.
		\sa iproc::IProcessor
	*/
	virtual int ExecuteScript(
				const QByteArray& scriptId,
				ibase::IProgressManager* progressPtr,
				ilog::IMessageConsumer* logPtr) const = 0;
};


} // namespace imtpy


