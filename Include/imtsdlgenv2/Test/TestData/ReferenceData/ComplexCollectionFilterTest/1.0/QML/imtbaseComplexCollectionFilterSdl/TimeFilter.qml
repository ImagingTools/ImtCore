import QtQuick
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

BaseClass {
	id: timeFilter
	readonly property string __typename: 'TimeFilter'
	property var m_timeRange: null
	property var m_timeUnit: null
	property var m_interpretationMode: null
	property var m_unitMultiplier: null

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_timeRange': return 'timeRange'
			case 'm_timeUnit': return 'timeUnit'
			case 'm_interpretationMode': return 'interpretationMode'
			case 'm_unitMultiplier': return 'unitMultiplier'
		}
	}

	function createComponent(propertyId, typename){
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

