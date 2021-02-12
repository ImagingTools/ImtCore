#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtzip/CFileCompressor.h>


namespace imtzip
{


/**
	Component realization of the file compressor/decompressor.
*/
class CFileCompressorComp: public icomp::CComponentBase, public imtzip::CFileCompressor
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFileCompressorComp);
		I_REGISTER_INTERFACE(IFileCompression);
	I_END_COMPONENT;
};


} // namespace imtzip


