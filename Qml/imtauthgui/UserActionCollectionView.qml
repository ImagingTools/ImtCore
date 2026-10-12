import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthUsersSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

RemoteCollectionView {
	id: root
	gqlGetListCommandId: "GetUserActions"
	documentCollectionFilter: null
	commandsControllerComp: null
	collectionId: "UserActions"
	Component.onCompleted: {
		registerFieldFilterDelegate("actionTypeId", actionDelegateFilterComp)

		if (PermissionsController.checkPermission("ViewUsers")){
			registerFieldFilterDelegate("userId", userDelegateFilterComp)
		}
	}
	
	onHeadersChanged: {
		table.setSortingInfo("timeStamp", "DESC")
		table.setColumnContentById("actionType", actionCellDelegateComp);
		table.setColumnContentById("userLink", userCellDelegateComp);
	}

	Component {
		id: actionDelegateFilterComp
		FieldFilterDelegate {
			id: actionDelegateFilter
			name: qsTr("Action")
			defaultFieldFilter.m_fieldId: "actionTypeId"

			Component.onCompleted: {
				createAndAddOption("Create", qsTr("Create"), "", true)
				createAndAddOption("Update", qsTr("Update"), "", true)
				createAndAddOption("Delete", qsTr("Delete"), "", true)
			}
		}
	}

	Component {
		id: userDelegateFilterComp
		
		CollectionFieldFilterDelegate {
			name: qsTr("Users")
			defaultFieldFilter.m_fieldId: "userId"
			commandId: ImtauthUsersSdlCommandIds.s_usersList
			fields: [UserItemDataTypeMetaInfo.s_id, UserItemDataTypeMetaInfo.s_name]
			titleField: UserItemDataTypeMetaInfo.s_name
			textFilterFieldIds: [UserItemDataTypeMetaInfo.s_name]
			sortByField: UserItemDataTypeMetaInfo.s_name
			filterPlaceholder: qsTr("Search by user name")
		}
	}
	
	Component {
		id: userCellDelegateComp
		TextLinkCellDelegate {
			onLinkActivated: {
				let targetLink = getValue()
				if (targetLink.containsKey("url")){
					let targetUrl = targetLink.getData("url")
					if (targetUrl && targetUrl.containsKey("path")){
						let path = targetUrl.getData("path")
						NavigationController.navigate(path)
					}
				}
			}
			
			onReused: {
				imageSource = "../../../" + Style.getIconPath("Icons/Account", Icon.State.On, Icon.Mode.Normal)
				let targetLink = getValue()
				text = targetLink.getData("name")
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
						image.source = "../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal) 
					}
					else if (actionType === "Create"){
						image.source = "../../../" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal) 
					}
					else if (actionType === "Delete"){
						image.source = "../../../" + Style.getIconPath("Icons/Garbage", Icon.State.On, Icon.Mode.Normal) 
					}
					else{
						image.source = "../../../" + Style.getIconPath("Icons/Restore", Icon.State.On, Icon.Mode.Normal) 
					}

					statusLable.text = cellDelegate.getValue();
				}
			}
		}
	}
}


