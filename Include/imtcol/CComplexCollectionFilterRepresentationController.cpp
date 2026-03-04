// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcol/CComplexCollectionFilterRepresentationController.h>


// ACF includes
#include <ilog/CMessage.h>

// ImtCore includes
#include <imtbase/CTimeFilterParam.h>


namespace imtcol
{


const QMap<int, imtbase::IComplexCollectionFilter::FieldOperation> CComplexCollectionFilterRepresentationController::s_allowableFlagsCombination = {
	{FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL},
	{FOF_EQUAL | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_EQUAL},

	{FOF_GREATER, imtbase::IComplexCollectionFilter::FieldOperation::FO_GREATER},
	{FOF_GREATER | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_GREATER},
	{FOF_GREATER | FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_LESS},

	{FOF_LESS, imtbase::IComplexCollectionFilter::FieldOperation::FO_LESS},
	{FOF_LESS | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_LESS},
	{FOF_LESS | FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_GREATER},

	{FOF_CONTAINS, imtbase::IComplexCollectionFilter::FieldOperation::FO_CONTAINS}
};


QString CComplexCollectionFilterRepresentationController::GetFlagsAsString(int /* flags */)
{
	QString retVal;
	return retVal;
}


bool CComplexCollectionFilterRepresentationController::ProcessFieldFilter(
			const sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0& /* source */,
			imtbase::IComplexCollectionFilter::FieldFilter& /* target */,
			ilog::IMessageConsumer* /* messageConsumerPtr */)
{
	return false;
}


bool CComplexCollectionFilterRepresentationController::ProcessGroupFilter(
			const sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& /* source */,
			imtbase::IComplexCollectionFilter::FilterExpression& /* target */,
			ilog::IMessageConsumer* /* messageConsumerPtr */)
{
	return true;
}


bool CComplexCollectionFilterRepresentationController::ComplexCollectionFilterRepresentationToModel(
	sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0& /* filterRepresentaion */,
	imtbase::IComplexCollectionFilter& /* filter */,
	ilog::IMessageConsumer* /* messageConsumerPtr */)
{
	return true;
}


void CComplexCollectionFilterRepresentationController::SendErrorMessage(const QString& message, ilog::IMessageConsumer* messageConsumerPtr)
{
	if (messageConsumerPtr != nullptr){
		ilog::IMessageConsumer::MessagePtr messagePtr(new ilog::CMessage(istd::IInformationProvider::IC_ERROR, 0, message, "CComplexCollectionFilterRepresentationController"));

		messageConsumerPtr->AddMessage(messagePtr);
	}
}


} // namespace imtcol


