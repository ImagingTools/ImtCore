#pragma once


// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IOptionsList.h>
#include <ibase/IProgressManager.h>
#include <ilog/IMessageConsumer.h>
#include <iattr/IAttributesMetaInfoProvider.h>
#include <iattr/IAttributesManager.h>


namespace pybind11
{
	class module_;
}


namespace imtpy
{


/**
	Common interface for the management of the scripts
*/
class IScriptManager: virtual public istd::IPolymorphic
{
public:
	/**
		Fill meta-infos for the given script file.
	*/
	virtual bool SetupScriptMetaInfo(
				const QString& scriptFilePath,
				iattr::IAttributesManager& scriptAttributesManager,
				QString& runFunctionName) = 0;

	/**
		Run a method defined in the script file.
	*/
	virtual bool RunMethod(
				iattr::IAttributesManager& scriptAttributesManager,
				pybind11::module_& moduleInstance,
				const QString& methodName,
				const QString& filePath) = 0;
};


} // namespace imtpy


