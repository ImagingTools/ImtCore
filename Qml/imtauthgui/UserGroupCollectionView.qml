import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtdocgui 1.0

RemoteCollectionView {
	id: userGroupCollectionViewContainer;
	
	collectionId: "Groups";
	visibleMetaInfo: true;
	documentCollectionFilter: null
	
	property string productId;
	property var documentManager: null;
	
	commandsDelegateComp: Component {DocCollectionViewDelegate {
			collectionView: userGroupCollectionViewContainer;
			documentManager: userGroupCollectionViewContainer.documentManager;

			Component.onCompleted: {
				registerDocumentType("Group", qsTr("Group"))
			}

			function updateStateBaseCommands(selection, commandsController, elementsModel){
				let isEnabled = selection.length > 0;
				if(commandsController){
					commandsController.setCommandIsEnabled("Remove", isEnabled);
					commandsController.setCommandIsEnabled("Edit", selection.length === 1);
					commandsController.setCommandIsEnabled("Export", selection.length === 1);
					commandsController.setCommandIsEnabled("Revision", selection.length === 1);
				}
			}
		}
	}

	function handleSubscription(dataModel){
		userGroupCollectionViewContainer.doUpdateGui();
	}
}
