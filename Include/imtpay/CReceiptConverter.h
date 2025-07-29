#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include<istd/IPolymorphic.h>

#include <GeneratedFiles/imtpaysdl/SDL/1.0/CPP/Receipt.h>


namespace iprm
{
class IParamsSet;
} // namespace iprm


namespace imtpay
{


class CReceiptConverter: public virtual istd::IPolymorphic
{
public:
	static bool CreateSdlFromParams(sdl::imtpay::Receipt::CReceipt::V1_0& receipt, const iprm::IParamsSet& params);
	static bool CreateParamsFromSdl(iprm::IParamsSet& params, const sdl::imtpay::Receipt::CReceipt::V1_0& receipt);
};


} // namespace imtpay
