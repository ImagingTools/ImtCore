#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtcrypter/CCrypter.h>

namespace ImtCrypterPck
{

typedef icomp::TMakeComponentWrap<
					imtcrypter::CCrypter,
					imtcrypter::ICrypter> Crypter;


} // namespace ImtCrypterPck


