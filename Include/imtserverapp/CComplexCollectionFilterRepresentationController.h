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
		FOF_CONTAINS = 16
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
				const sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0& representation,
				imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter) const;
	bool GetSdlRepresentationFromFieldFilter(
				const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter,
				sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0& representation) const;
	bool GetGroupFilterFromSdlRepresentation(
				const sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& representation,
				imtbase::IComplexCollectionFilter::GroupFilter& groupFilter) const;
	bool GetSdlRepresentationFromGroupFilter(
				const imtbase::IComplexCollectionFilter::GroupFilter& groupFilter,
				sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& representation) const;

private:
	static const QMap<int, imtbase::IComplexCollectionFilter::FieldOperation> s_allowableFlagsCombination;
	imtserverapp::CTimeFilterParamRepresentationController m_timeFilterParamRepresentationController;
};


} // namespace imtserverapp


