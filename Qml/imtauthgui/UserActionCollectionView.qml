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
import imtbaseImtCollectionSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

RemoteCollectionView {
	id: root
	gqlGetListCommandId: "GetUserActions"
	documentCollectionFilter: null
	commandsDelegateComp: null

	Component.onCompleted: {
		registerFieldFilterDelegate("userId", userDelegateFilterComp)
	}
	
	onHeadersChanged: {
		table.setColumnContentById("actionType", actionCellDelegateComp);
		table.setColumnContentById("targetLink", targetLinkDelegateComp);
		table.setColumnContentById("userLink", targetLinkDelegateComp);
	}

	Component {
		id: userDelegateFilterComp
		
		FieldFilterDelegate {
			id: userDelegateFilter
			name: qsTr("Users")
			visibleItemCount: 15
			defaultFieldFilter.m_fieldId: "userId"

			Component.onCompleted: {
				userCollectionDataProvider.updateModel()
			}

			CollectionDataProvider {
				id: userCollectionDataProvider
				commandId: "UsersList"
				fields: ["id", "typeId", "name", "description", "userId"]
				onCollectionModelChanged: {
					optionsListAdapter.collectionModel = collectionModel
				}
			}

			OptionsListAdapter {
				id: optionsListAdapter
				idKey: "userId"
				onCollectionModelChanged: {
					userDelegateFilter.setOptionsList(m_options)
				}
			}
		}
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

	Component {
		id: targetLinkDelegateComp
		TableCellDelegateBase {
			id: cellDelegate

			Text {
				id: linkText
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: Style.fontSizeM
				font.family: Style.fontFamily
				elide: Text.ElideRight
				color: "#0b5ed7"
				font.underline: true
			}
			
			MouseArea {
				id: mouseArea
				anchors.fill: linkText
				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor
				onClicked: {
					let targetLink = cellDelegate.getValue()
					let targetId = targetLink.getData("id")
					let targetTypeId = targetLink.getData("typeId")
					let targetSource = targetLink.getData("source")
					
					MainDocumentManager.openDocument(targetSource, targetId, targetTypeId)
				}
			}

			onReused: {
				let targetLink = cellDelegate.getValue()
				linkText.text = targetLink.getData("name")
			}
		}
	}
}


