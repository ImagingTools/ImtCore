#pragma once

// Qt includes

// ImtCore includes
#include <imtsdl/imtsdl.h>


class QFile;


namespace imtsdlgencpp
{


class ICxxFileProcessor
{
public:
	/**
		\brief Processing a file, by modifying file's content
		\param sdlType			- type to process
		\param headerFilePtr	- header (.h) file, to write data
		\param sourceFilePtr	- source(.cpp) file, to write data
		\returns true - if processing is successful and false otherwise
		\todo replace \c QFile to \c QIODevice
	 */
	[[nodiscard]] virtual bool ProcessType (const imtsdl::CSdlType& sdlType, QFile* headerFilePtr, QFile* sourceFilePtr) = 0;
};


} // namespace imtsdlgencpp
