import QtQuick 2.0
import Acf 1.0
import imtguigql 1.0
import imtbaseImtCollectionSdl 1.0

QtObject {
	id: root
	
	signal visualStatusReceived(string objectId, string icon, string text, string description)
	signal visualStatusReceiveFailed(string objectId, string errorMessage)

	function getVisualStatus(id, typeId){
		visualStatusReceived(id, "", "", "")
	}
}

