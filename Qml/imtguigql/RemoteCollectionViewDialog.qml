import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcolgui 1.0

Dialog {
	id: root
	canMove: false
	
	width: ModalDialogManager.activeView.width - 100
	height: ModalDialogManager.activeView.height - 100

	property string collectionId
	property RemoteCollectionView collectionView: null

	contentComp: Component {
		Item {
			width: root.width
			height: root.height - 100
			
			RemoteCollectionView {
				anchors.fill: parent
				commandsControllerComp: null
				visibleMetaInfo: false
				commandsDelegateComp: null
				collectionId: root.collectionId
				documentCollectionFilter: null
				loadingDataAfterHeadersReceived: false
				Component.onCompleted: {
					root.collectionView = this
				}
			}
		}
	}
}
