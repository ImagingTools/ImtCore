// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QList>

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
	/// \todo use \c std::set later
	[[nodiscard]] virtual QList<imtsdl::IncludeDirective> GetIncludeDirectives(/*const iprm::IParamsSet* paramsPtr*/) const = 0;

	/**
		\returns the directive's type, in case if an instance is also a \c ICxxFileProcessor, usually equals to \c ICxxFileProcessor::GetProcessorTypeId
		\sa ICxxFileProcessor
		\sa ICxxFileProcessor::GetProcessorTypeId
	*/
	[[nodiscard]] virtual QString GetDirectivesTypeId() const = 0;
};




} // namespace imtsdlgencpp
