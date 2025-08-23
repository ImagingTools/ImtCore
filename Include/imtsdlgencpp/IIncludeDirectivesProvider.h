#pragma once

// Qt includes
#include <QtCore/QSet>

// ImtCore includes
#include <imtsdl/imtsdl.h>


namespace iprm
{
class IParamsSet;
}


namespace imtsdlgencpp
{


class IIncludeDirectivesProvider
{
public:
	[[nodiscard]] virtual QSet<imtsdl::IncludeDirective> GetIncludeDirectives(/*const iprm::IParamsSet* paramsPtr*/) const = 0;

	/**
		\returns the directive's type, in case if an instance is also a \c ICxxFileProcessor, usually equals to \c ICxxFileProcessor::GetProcessorTypeId
		\sa ICxxFileProcessor
		\sa ICxxFileProcessor::GetProcessorTypeId
	*/
	[[nodiscard]] virtual QString GetDirectivesTypeId() const = 0;
};




} // namespace imtsdlgencpp
