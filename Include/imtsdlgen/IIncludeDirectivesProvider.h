#pragma once

// Qt includes
#include <QtCore/QList>


namespace imtsdlgen
{


class IIncludeDirectivesProvider
{
public:
	enum Priority
	{
		P_C = 0,
		P_OS_API,
		P_QT,
		P_ACF,
		P_IMT,
		P_CUSTOM
	};
	enum Type
	{
		T_HEADER,
		T_SOURCE
	};

	struct IncludeDirective
	{
		Priority priority = P_C;
		QString remark;
		QString path;
		Type type = T_HEADER;
	};

	[[nodiscard]] virtual QList<IncludeDirective> GetIncludeDirectives() const = 0;
};




} // namespace imtsdlgen
