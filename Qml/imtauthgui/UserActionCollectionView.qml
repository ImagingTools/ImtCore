import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthUsersSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: root
	gqlGetListCommandId: "GetUserActions"
	documentCollectionFilter: null
	commandsDelegateComp: Component {
		CollectionViewCommandsDelegateBase {
			collectionView: root
			function onEdit(){
				let selection = root.table.getSelectedIndexes()
				if (selection.length === 1){
					let documentId = root.table.elements.getData("targetId", selection[0])
					let documentTypeId = root.table.elements.getData("targetTypeId", selection[0])
					MainDocumentManager.openDocument(documentId, documentTypeId)
				}
			}
		}
	}
	
	onHeadersChanged: {
		table.setColumnContentById("actionType", actionCellDelegateComp);
	}

	Component {
		id: actionCellDelegateComp
		TableCellDelegateBase {
			id: cellDelegate
			
			Image {
				id: image;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: parent.left;
				anchors.leftMargin: 5;

				width: 20;
				height: width;

				sourceSize.width: width;
				sourceSize.height: height;
			}

			Text {
				id: statusLable;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: image.right
				anchors.leftMargin: Style.marginM;
				anchors.right: parent.right

				font.pixelSize: Style.fontSizeM;
				font.family: Style.fontFamily;
				color: Style.textColor;

				elide: Text.ElideRight;
			}

			onReused: {
				if (rowIndex >= 0){
					let actionType = cellDelegate.rowDelegate.tableItem.elements.getData("actionType", rowIndex)
					if (actionType === "Update"){
						image.source = "../../../" + Style.getIconPath("Icons/Restore", Icon.State.On, Icon.Mode.Normal) 
					}
					else if (actionType === "Create"){
						image.source = "../../../" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal) 
					}
					else if (actionType === "Delete"){
						image.source = "../../../" + Style.getIconPath("Icons/Delete", Icon.State.On, Icon.Mode.Normal) 
					}
					else{
						image.source = ""
					}

					statusLable.text = cellDelegate.getValue();
				}
			}
		}
	}
}


