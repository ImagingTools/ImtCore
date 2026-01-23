import QtQuick
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

BaseClass {
	id: timeFilter
	readonly property string __typename: 'TimeFilter'
	property TimeRange m_timeRange: null
	property string m_timeUnit: ''
	property string m_interpretationMode: ''
	property int m_unitMultiplier: 0

	Component.onCompleted: {
		this._internal.removed = ["m_timeRange","m_timeUnit","m_interpretationMode","m_unitMultiplier"]
	}

	function hasTimeRange(){
		return (m_timeRange !== undefined && m_timeRange !== null)
	}

	function hasTimeUnit(){
		return (m_timeUnit !== undefined && m_timeUnit !== null)
	}

	function hasInterpretationMode(){
		return (m_interpretationMode !== undefined && m_interpretationMode !== null)
	}

	function hasUnitMultiplier(){
		return (m_unitMultiplier !== undefined && m_unitMultiplier !== null)
	}

	function emplaceTimeRange(typename){
		m_timeRange = createComponent('m_timeRange', typename).createObject(this)
		m_timeRange.owner = this
		this._internal.removeAt('m_timeRange')
	}

	function removeTimeRange(){
		removeKey('m_timeRange')
	}

	function removeTimeUnit(){
		removeKey('m_timeUnit')
	}

	function removeInterpretationMode(){
		removeKey('m_interpretationMode')
	}

	function removeUnitMultiplier(){
		removeKey('m_unitMultiplier')
	}

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_timeRange': return 'timeRange'
			case 'm_timeUnit': return 'timeUnit'
			case 'm_interpretationMode': return 'interpretationMode'
			case 'm_unitMultiplier': return 'unitMultiplier'
			case '__typename': return '__typename'
		}
	}

	function createElement(propertyId, typename){
		switch (propertyId){
		}
	}

	function createComponent(propertyId, typename){
		let retVal;
		switch (propertyId){
			case 'm_timeRange': return Qt.createComponent('qrc:/qml/imtbaseImtBaseTypesSdl/TimeRange.qml')
		}
	}

	function createMe(){
		return Qt.createComponent('TimeFilter.qml').createObject()
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_timeRange': return 'TimeRange'
			case 'm_timeUnit': return 'string'
			case 'm_interpretationMode': return 'string'
			case 'm_unitMultiplier': return 'int'
		}
	}
}

