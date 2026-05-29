// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter>


namespace imtcol
{


class CComplexCollectionFilterRepresentationController
{
public:
	static bool ComplexCollectionFilterRepresentationToModel(
		sdl::V1_0::imtbase::CComplexCollectionFilter& filterRepresentaion,
		imtbase::IComplexCollectionFilter& filter,
		ilog::IMessageConsumer* messageConsumerPtr);

private:
	enum FilterOperationFlags
	{
		FOF_NOT = 1,
		FOF_EQUAL = 2,
		FOF_LESS = 4,
		FOF_GREATER = 8,
		FOF_CONTAINS = 16
	};

private:
	static QString GetFlagsAsString(int flags);
	static bool ProcessFieldFilter(
		const sdl::V1_0::imtbase::CFieldFilter& source,
		imtbase::IComplexCollectionFilter::FieldFilter& target,
		ilog::IMessageConsumer* messageConsumerPtr);
	static bool ProcessGroupFilter(
		const sdl::V1_0::imtbase::CGroupFilter& source,
		imtbase::IComplexCollectionFilter::FilterExpression& target,
		ilog::IMessageConsumer* messageConsumerPtr);

	static void SendErrorMessage(const QString& message, ilog::IMessageConsumer* messageConsumerPtr);

private:
	static const QMap<int, imtbase::IComplexCollectionFilter::FieldOperation> s_allowableFlagsCombination;
};


} // namespace imtgql


