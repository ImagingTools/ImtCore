#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include<istd/IPolymorphic.h>


namespace sdl::imtpay::Receipt::V1_0
{
	class CReceipt;
} // namespace sdl::imtpay::Receipt::V1_0

namespace iprm
{
class IParamsSet;
} // namespace iprm


namespace imtpay
{


class CReceiptConverter: public virtual istd::IPolymorphic
{
public:
	static bool CreateSdlFromParams(sdl::imtpay::Receipt::V1_0::CReceipt& receipt, const iprm::IParamsSet& params);
	static bool CreateParamsFromSdl(iprm::IParamsSet& params, const sdl::imtpay::Receipt::V1_0::CReceipt& receipt);
};


} // namespace imtpay
