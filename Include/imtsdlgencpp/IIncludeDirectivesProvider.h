#pragma once

// Qt includes
#include <QtCore/QList>

// ImtCore includes
#include <imtsdl/imtsdl.h>


namespace imtsdlgencpp
{


class IIncludeDirectivesProvider
{
public:
	[[nodiscard]] virtual QList<imtsdl::IncludeDirective> GetIncludeDirectives() const = 0;
};




} // namespace imtsdlgencpp
