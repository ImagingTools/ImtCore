import QtQuick 2.12
import imtcontrols 1.0

BaseClass {
	id: canvasTableHeaderSdl;

	property string m_id: "";
	property string m_name: "";

	function getJSONKeyForProperty(propertyId){
		switch (propertyId){
			case "m_id": return "id";
			case "m_name": return "name";
		}
	}
}
