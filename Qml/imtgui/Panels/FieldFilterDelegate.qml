import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

OptionsFilterDelegate {
	id: filterDelegate
	property FieldFilter defaultFieldFilter: FieldFilter {
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}
	QtObject {
		id: internal
		property var prevSelectedIds: ([])
		property var prevSelectedIndexes: ([])
		property var fieldFiltersMap: ({})
	}

	onOptionSelectionChanged: {
		if (!collectionFilter){
			return
		}

		if (internal.prevSelectedIds.length > 0 || optionIndexes.length === 0){
			for (let i = 0; i < internal.prevSelectedIds.length; i++){
				let optionId = internal.prevSelectedIds[i]
				if (optionId in internal.fieldFiltersMap){
					collectionFilter.removeFilterByFieldId(internal.fieldFiltersMap[optionId].m_fieldId)
				}
				else{
					collectionFilter.removeFilterByFieldId(defaultFieldFilter.m_fieldId)
				}
			}
		}
		
		if (optionIndexes.length > 0){
			for (let i = 0; i < optionIndexes.length; i++){
				let optionIndex = optionIndexes[i]
				let optionId = getOptionId(optionIndex);
				if (optionId in internal.fieldFiltersMap){
					collectionFilter.addFieldFilter(internal.fieldFiltersMap[optionId])
				}
				else{
					let filter = defaultFieldFilter.copyMe()
					filter.m_filterValue = optionId
					collectionFilter.addFieldFilter(filter)
				}
			}
		}
		
		internal.prevSelectedIds = optionIds
		internal.prevSelectedIndexes = optionIndexes

		collectionFilter.filterChanged()
	}
	
	function setFieldFilterForOption(optionId, fieldFilter){
		internal.fieldFiltersMap[optionId] = fieldFilter
	}
}


