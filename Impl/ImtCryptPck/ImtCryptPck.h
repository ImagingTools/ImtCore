#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcrypt/CCrypter.h>


namespace ImtCryptPck
{


typedef icomp::TMakeComponentWrap<
			imtcrypt::CCrypter,
			imtcrypt::ICrypter> Crypter;


} // namespace ImtCryptPck


