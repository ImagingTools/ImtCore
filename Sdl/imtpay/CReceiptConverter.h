#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include<istd/IPolymorphic.h>

#include <GeneratedFiles/imtpaysdl/SDL/1.0/CPP/ImtPay.h>


namespace iprm
{
class IParamsSet;
} // namespace iprm


namespace imtpaysdl
{


class CReceiptConverter: public virtual istd::IPolymorphic
{
public:
	static bool CreateSdlFromParams(sdl::imtpay::ImtPay::CReceipt::V1_0& receipt, const iprm::IParamsSet& params);
	static bool CreateParamsFromSdl(iprm::IParamsSet& params, const sdl::imtpay::ImtPay::CReceipt::V1_0& receipt);
};


} // namespace imtpaysdl
