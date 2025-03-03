import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtbaseComplexCollectionFilterSdl 1.0

ComplexCollectionFilter {
	id: root
	
	property Component fieldSortingInfoFact: Component {FieldSortingInfo {}}
	property Component groupFilterFact: Component {GroupFilter {}}
	property Component fieldFilterFact: Component {FieldFilter {}}
	property Component baseModelFact: Component {BaseModel {}}
	
	property var textFilteringInfoIds: []
	
	property var logicalOperation: ({
		AND: "And",
		OR: "Or"
	})
	
	property var filterOperation: ({
		NOT: "Not",
		EQUAL: "Equal",
		LESS: "Less",
		GREATER: "Greater",
		CONTAINS: "Contains"
	})
	
	property var valueType: ({
		INTEGER: "Integer",
		NUMBER: "Number",
		STRING: "String",
		BOOL: "Bool"
	})
	
	signal filterChanged()
	
	Component.onCompleted: {
		m_fieldsFilter.m_logicalOperation = logicalOperation.AND
	}
	
	function createBaseModel(){
		return baseModelFact.createObject(root)
	}
	
	function getTextFilter(){
		if (filterModel.containsKey("TextFilter")){
			return filterModel.getData("TextFilter")
		}
		
		return ""
	}

	function setTextFilter(filter){
		console.log("setTextFilter", filter, valueType.STRING, filterOperation.CONTAINS)
		console.log("textFilteringInfoIds", textFilteringInfoIds)
		
		m_fieldsFilter.m_groupFilters.clear();
		
		let fieldsModel = createBaseModel();
		for (let i = 0; i < textFilteringInfoIds.length; i++){
			let infoId = textFilteringInfoIds[i];
			let fieldObj = createFieldFilter(infoId, filter, valueType.STRING, [filterOperation.CONTAINS]);
			
			fieldsModel.addElement(fieldObj);
		}
		
		let textGroupFilter = createGroupFilter(logicalOperation.OR, fieldsModel);
		
		addGroupFilter(textGroupFilter);
	}
	
	function setSortingInfo(infoId, sortOrder){
		console.log("setSortingInfo", infoId, sortOrder)
		clearSortFilter()

		let obj = fieldSortingInfoFact.createObject(m_sortingInfo)
		
		obj.m_fieldId = infoId
		obj.m_sortingOrder = sortOrder
		
		m_sortingInfo.addElement(obj)
	}
	
	function getSortingInfo(){
		if (m_sortingInfo.length > 0){
			return m_sortingInfo.get(0).item
		}

		return null
	}
	
	// filteringInfoIds - array of strings
	function setFilteringInfoIds(filteringInfoIds){
		root.textFilteringInfoIds = filteringInfoIds
	}
	
	function createGroupFilter(operation, fields, groups){
		let groupObj = groupFilterFact.createObject(m_fieldsFilter)
		
		if (fields){
			groupObj.m_fieldFilters = fields
		}
		
		if (groups){
			groupObj.m_groupFilters = groups
		}
		
		if (operation){
			groupObj.m_logicalOperation = operation
		}
		else{
			groupObj.m_logicalOperation = logicalOperation.AND
		}

		return groupObj;
	}
	
	function addGroupFilter(group){
		m_fieldsFilter.m_groupFilters.addElement(group)
	}
	
	function createFieldFilter(fieldId, value, valueType, operations){
		let fieldObj = fieldFilterFact.createObject(m_fieldsFilter)
		fieldObj.m_fieldId = fieldId
		fieldObj.m_filterValue = value
		fieldObj.m_filterValueType = valueType
		fieldObj.m_filterOperations = operations
		
		return fieldObj;
	}
	
	function addFieldFilter(field){
		m_fieldsFilter.m_fieldFilters.addElement(field)
	}
	
	function removeFieldFilter(field){
		for (let i = 0; i < m_fieldsFilter.m_fieldFilters.count; i++){
			let item = m_fieldsFilter.m_fieldFilters.get(i).item
			if (item == field){
				m_fieldsFilter.m_fieldFilters.remove(i)
				return true;
			}
		}
		
		return false;
	}
	
	function removeGroupFilter(group){
		for (let i = 0; i < m_fieldsFilter.m_groupFilters.count; i++){
			let item = m_fieldsFilter.m_groupFilters.get(i).item
			if (item == group){
				m_fieldsFilter.m_groupFilters.remove(i)
				return true;
			}
		}
		
		return false;
	}
	
	function setTimeFilter(timeFilter){
		clearTimeFilter()
		
		if (timeFilter){
			m_timeFilter.copyFrom(timeFilter)
		}
	}
	
	function clearAllFilters(){
		clearSortFilter()
		clearFieldsFilter()
		clearTimeFilter()
		clearDistinctFilter()
	}
	
	function clearSortFilter(){
		m_sortingInfo.clear()
	}
	
	function clearFieldsFilter(){
		m_fieldsFilter.m_fieldFilters.clear()
		m_fieldsFilter.m_groupFilters.clear()
	}
	
	function clearTimeFilter(){
		m_timeFilter.m_timeRange.m_begin = ""
		m_timeFilter.m_timeRange.m_end = ""
		m_timeFilter.m_timeUnit = ""
		m_timeFilter.m_interpretationMode = ""
		m_timeFilter.m_unitMultiplier = 0
	}
	
	function clearDistinctFilter(){
		m_distinctFields = []
	}
}
