#pragma once


//Acf includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdlgenv2
{


class ISdlProcessArgumentsParser: virtual public imtsdl::ISdlProcessArgumentsParser
{
public:
	virtual quint16 GetGenerationVersion() const = 0;

};


} // namespace imtsdlgenv2


