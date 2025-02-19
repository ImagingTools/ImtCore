#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtcol/CObjectTypeIdFilter.h>
#include <imtcol/CTextFilter.h>


namespace ImtColPck
{


typedef icomp::TModelCompWrap<icomp::TMakeComponentWrap<imtcol::CObjectTypeIdFilter, imtcol::IObjectTypeIdFilter, iser::ISerializable>> ObjectTypeIdFilter;
typedef icomp::TModelCompWrap<icomp::TMakeComponentWrap<imtcol::CTextFilter, imtcol::ITextFilter, iser::ISerializable>> TextFilter;


} // namespace ImtColPck



