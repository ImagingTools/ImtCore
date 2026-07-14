#pragma once


// Qt include
#include <QtCore/QtAssert>

// ACF includes
#include <istd/TNullableValue.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtmdbx/IMdbxDatabaseEngine.h>
#include <imtmdbx/CDoubleMask.h>
#include <imtmdbx/CMaskContainer.h>
#include <imtmdbx/CMemoryMask.h>
#include <imtmdbx/imtmdbx.h>
#include <imtmdbx/CMdbxDocumentTableManager.h>
#include <imtserverapp/CParamSetRepresentationController.h>
#include <imtserverapp/CComplexCollectionFilterRepresentationController.h>
#include <imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtmdbx
{


class CMdbxCollectionControllerBase: virtual public istd::IPolymorphic
{
public:
	using Parameter = ::sdl::V1_0::imtbase::CParameter;

	CMdbxCollectionControllerBase();
	
protected:
	struct FilterViewParams{
		std::optional<int> count;
		std::optional<int> offset;
		std::optional<QString> sortId;
		std::optional<QString> sortingOrder;
		std::optional<QString> textFilter;
	};

protected:
	/**
		\brief Process complex filter from the input parameter set to extract sorting info and text filter.
		\param retVal - reference to \struct FilterViewParams to fill with extracted parameters.
		\param filterParams - parameter set containing filter criteria.
		\param selectionParams - SDL object containing selection parameters.
		\return True if processing was successful, otherwise false.
	*/
	bool processComplexFilterFromParamSet(
				FilterViewParams& retVal,
				iprm::CParamsSet* filterParams,
				const sdl::V1_0::imtbase::CParamsSet& selectionParams) const;
	/**
		\brief Get count of elements in the table after applying masks and text filter.
		\param maskContainer - container with applied masks.
		\param tablePtr - pointer to the table to get count from.
		\param noTextFilter - flag indicating whether text filter is applied or not.
				If true, calculations are aborted.
	*/
	int GetElementCount(
				imtmdbx::CMaskContainer& maskContainer,
				DocumentTablePtr tablePtr,
				bool noTextFilter) const;
	/**
		\brief Checks if a specific bit in a mask container is set.

		Looks up the mask at the given offset, using the cache to reduce container accesses.
		If not in cache, retrieves from \c maskContainer and updates \c itemCache.
		Oldest cache entries are removed if \c itemCache exceeds \c cacheLimit.

		\param offset - 64-bit block offset in the mask container
		\param bitPosition - Bit index (0–63) within the block
		\param cacheLimit - Maximum number of items in \c itemCache
		\param maskContainer - Reference to the mask container
		\param itemCache - Reference to the local cache of <mask, offset> pairs
		\return true if the bit at \c bitPosition is set, false otherwise

		\note \c itemCache is modified when fetching a new block from \c maskContainer
	*/
	bool CheckMaskMatch(
				quint64 offset,
				quint64 bitPosition,
				int cacheLimit,
				imtmdbx::CMaskContainer& maskContainer,
				QList<QPair<quint64, quint64>>& itemCache) const;

	// Deprecated. Kept for previous API compatibility
	/**
		\brief Process view parameters (count, offset, sorting info) from the input object.
		\param viewParams - SDL object containing view parameters.
		\return \struct FilterViewParams with processed parameters.
	*/
	FilterViewParams processViewParams(
				const istd::TNullableValue<::sdl::V1_0::imtbase::CCollectionViewParams> viewParams) const;
	/**
		\brief Process complex filter from the input SDL object to extract sorting info and text filter.
		\param retVal - reference to \struct FilterViewParams to fill with extracted parameters.
		\param complexFilter - SDL object containing complex filter criteria.
	*/
	void processSdlComplexFilter(
				FilterViewParams& retVal,
				const istd::TNullableValue<::sdl::V1_0::imtbase::CComplexCollectionFilter> complexFilter) const;
	// end Deprecated

protected:
	/**
		\brief Convert comma-separated UUIDs to their serial id representation.
		\param parentIdsStr - Comma-separated string of UUIDs.
		\param addressTable - \sa imtmdbx::IDocumentTable shared ptr to table for address elements.
		\return Comma-separated string of serial IDs. Empty string if any UUID not found.
	*/
	QString ConvertParentIdsFromUuid(
				const QString& parentIdsStr,
				DocumentTablePtr addressTable) const;
	/**
		\brief Convert comma-separated serial IDs to their UUID representation.
		\param parentIdsStr - Comma-separated string of serial IDs.
		\param addressTable - \sa imtmdbx::IDocumentTable shared ptr to table for address elements.
		\return Comma-separated string of UUIDs. Empty string if any serial ID not found.
	*/
	QString ConvertParentIdsToUuid(
				const QString& parentIdsStr,
				DocumentTablePtr addressTable) const;
	/**
		\brief Process input SDL object to extract filter and view parameters.
				Reimplemented logic of \sa imtservergql::CObjectCollectionControllerCompBase::PrepareFilters().
		\param inputObj - SDL Input object containing filter criteria.
		\return \struct FilterViewParams with processed parameters.
	*/
	template<class SdlClass>
	FilterViewParams processInputObject(const SdlClass& inputObj) const;

private:
	iprm::CParamsSet m_selectionParams;
	imtbase::CComplexCollectionFilter m_complexCollectionFilter;
	imtserverapp::CParamSetRepresentationController m_paramSetRepresentationController;
	imtserverapp::CComplexCollectionFilterRepresentationController m_complexCollectionFilterRepresentationController;
};


// protected template methods

template<class SdlClass>
CMdbxCollectionControllerBase::FilterViewParams CMdbxCollectionControllerBase::processInputObject(const SdlClass& inputObj) const
{
	if(inputObj.viewParams){
		return processViewParams(inputObj.viewParams);
	}

	FilterViewParams retVal;

	retVal.count = inputObj.count.value_or(-1);
	retVal.offset = inputObj.offset.value_or(0);

	if(!inputObj.selectionParams || (inputObj.selectionParams && !inputObj.selectionParams->parameters)){
		return retVal;
	}

	iprm::CParamsSet filterParams;
	istd::TNullableValue<imtsdl::TElementList<Parameter>> parameters = inputObj.selectionParams->parameters;

	const QList<Parameter> parameterList = parameters->ToList();
	for (const sdl::V1_0::imtbase::CParameter& parameter: parameterList){
		if (!parameter.id){
			continue;
		}

		const QByteArray& parameterId = *parameter.id;
		const iser::ISerializable* parameterPtr = m_selectionParams.GetParameter(parameterId);
		if (parameterPtr == nullptr){
			continue;
		}

		istd::TDelPtr<iser::ISerializable> filterParameterPtr;
		filterParameterPtr.SetCastedOrRemove(parameterPtr->CloneMe().PopPtr());
		if (!filterParameterPtr.IsValid()){
			continue;
		}

		if (parameterId == m_complexCollectionFilterRepresentationController.GetTypeId()){
			imtbase::CComplexCollectionFilter* complexFilterPtr = dynamic_cast<imtbase::CComplexCollectionFilter*>(filterParameterPtr.GetPtr());
			if (complexFilterPtr != nullptr){
				filterParams.SetEditableParameter(parameterId, filterParameterPtr.PopPtr(), true);
			}
		}
	}

	const sdl::V1_0::imtbase::CParamsSet& selectionParams = *inputObj.selectionParams;

	processComplexFilterFromParamSet(retVal, &filterParams, selectionParams);

	return retVal;
}


} // namespace imtmdbx



