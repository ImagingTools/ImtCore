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
	typedef icomp::TReferenceMember<iprm::IParamsSet> SchemaParamsCompPtr;
	typedef icomp::TReferenceMember<imtsdl::ISdlProcessArgumentsParser> ArgumentParserCompPtr;


};


} // namespace imtsdlgenv2
