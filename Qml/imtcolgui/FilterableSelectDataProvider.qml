import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
	id: root

	property string collectionId

	signal listObjectsReceived(var listObjects)
	signal listObjectsReceiveFailed(string message)

	function getSelectableItems(count, offset, filter){
		console.warn("getSelectableItems() should be implemented in a subclass")
		listObjectsReceived(null)
	}
}
