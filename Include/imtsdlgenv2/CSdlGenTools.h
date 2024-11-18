#pragma once

// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <icomp/TReferenceMember.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>


class QTextStream;

namespace iprm
{
class IParamsSet;
}


namespace imtsdl
{
class ISdlProcessArgumentsParser;
}


namespace imtsdlgenv2
{


class CSdlGenTools
{
public:


public:
	/**
		\returns namespace from \c schemaParams, accorging to V2 Generation
	 */
	[[nodiscard]] static QString GetNamespaceFromSchemaParams(const iprm::IParamsSet& schemaParams, const QString& prefix = "sdl");

};


} // namespace imtsdlgenv2
