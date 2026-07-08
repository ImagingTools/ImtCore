import QtQuick
import imtcontrols 1.0

BaseClass {
	id: tableData
	property string m_text: ''
	property color m_textColor: 'black'
	property string m_iconSource: ''
	property string m_fontFamily: ''
	property real m_fontSize: 14

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case 'm_text': return 'text'
			case 'm_textColor': return 'textColor'
			case 'm_iconSource': return 'iconSource'
			case 'm_fontFamily': return 'fontFamily'
			case 'm_fontSize': return 'fontSize'
		}
	}

	function getPropertyType(propertyId){
		switch (propertyId){
			case 'm_text': return 'string'
			case 'm_textColor': return 'color'
			case 'm_iconSource': return 'string'
			case 'm_fontFamily': return 'string'
			case 'm_fontSize': return 'real'
		}
	}
}

