// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <imtserverapp/TJsonRepresentationControllerWrap.h>
#include <imtserverapp/CTimeFilterParamRepresentationController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ComplexCollectionFilter.h>


namespace imtserverapp
{


class CComplexCollectionFilterRepresentationController:
			public imtserverapp::TJsonRepresentationControllerWrap<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0>
{
public:
	enum FilterOperationFlags
	{
		FOF_NOT = 1,
		FOF_EQUAL = 2,
		FOF_LESS = 4,
		FOF_GREATER = 8,
		FOF_CONTAINS = 16,
		FOF_ARRAY_IS_EMPTY = 32,
		FOF_ARRAY_HAS_ANY = 64,
		FOF_ARRAY_HAS_ALL = 128,
		FOF_ARRAY_ILIKE_ANY = 256
	};

	CComplexCollectionFilterRepresentationController();

	// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0& sdlRepresentation) const override;

private:
	QString GetFlagsAsString(int flags) const;
	bool GetFieldFilterFromSdlRepresentation(
				const sdl::imtbase::ComplexCollectionFilter::FieldFilterUnion& representation,
				imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter) const;
	bool GetSdlRepresentationFromFieldFilter(
				const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter,
				sdl::imtbase::ComplexCollectionFilter::FieldFilterUnion& representation) const;
	bool GetGroupFilterFromSdlRepresentation(
				const sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& representation,
				imtbase::IComplexCollectionFilter::FilterExpression& groupFilter) const;
	bool GetSdlRepresentationFromGroupFilter(
				const imtbase::IComplexCollectionFilter::FilterExpression& groupFilter,
				sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& representation) const;

	bool GetSdlMetaTypeFromVariantType(const int& typeId, sdl::imtbase::ComplexCollectionFilter::ValueType& sdlType) const;
	bool GetQVariantFromSdlValue(const QString& sdlValue, const sdl::imtbase::ComplexCollectionFilter::ValueType& valueType, QVariant& value) const;
	int ComputeFlagsFromSdlOperations(
				const imtsdl::TElementList<sdl::imtbase::ComplexCollectionFilter::FilterOperation>& filterOperations) const;
	void MapFieldOperationToSdlOperations(
				imtbase::IComplexCollectionFilter::FieldOperation op,
				imtsdl::TElementList<sdl::imtbase::ComplexCollectionFilter::FilterOperation>& outOperations) const;

	bool FieldIsExists(const imtbase::IComplexCollectionFilter::Fields& fields, const QByteArray& fieldId) const;
	bool CreateOrUpdateFieldInfo(
				imtbase::IComplexCollectionFilter::Fields& fields,
				const QByteArray& fieldId,
				std::optional<imtbase::IComplexCollectionFilter::FieldType> fieldType = std::nullopt,
				std::optional<bool> filterable = std::nullopt,
				std::optional<bool> sortable = std::nullopt,
				std::optional<bool> isDistinct = std::nullopt,
				std::optional<imtbase::IComplexCollectionFilter::SortingOrder> sortingOrder = std::nullopt
			) const;

private:
	static const QMap<int, imtbase::IComplexCollectionFilter::FieldOperation> s_allowableFlagsCombination;
	imtserverapp::CTimeFilterParamRepresentationController m_timeFilterParamRepresentationController;
};


} // namespace imtserverapp


