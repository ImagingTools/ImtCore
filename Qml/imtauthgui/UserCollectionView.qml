import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtauthUsersSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: userCollectionViewContainer;
	
	collectionId: "Users";
	visibleMetaInfo: true;
	documentCollectionFilter: null

	additionalFieldIds: [UserItemDataTypeMetaInfo.s_systemId]
	
	property string productId;
	property var documentManager: null;
	
	commandsDelegateComp: Component {DocCollectionViewDelegate {
			collectionView: userCollectionViewContainer;
			documentManager: userCollectionViewContainer.documentManager;

			Component.onCompleted: registerDocumentType("User", qsTr("User"))

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

	Component {
		id: systemInfoDelegateFilterComp
		FieldFilterDelegate {
			id: systemInfoDelegateFilter
			name: qsTr("System Info")

			Component.onCompleted: {
				createAndAddOption("Internal", qsTr("Internal"), "", true)
				createAndAddOption("Ldap", qsTr("LDAP"), "", true)
				
				setFieldFilterForOption("Internal", internalSystemFilter)
				setFieldFilterForOption("Ldap", ldapSystemFilter)
			}
			
			FieldFilter {
				id: internalSystemFilter
				m_fieldId: "SystemId"
				m_filterValue: ""
				m_filterValueType: "String"
				m_filterOperations: ["Equal"]
			}
			
			FieldFilter {
				id: ldapSystemFilter
				m_fieldId: "SystemId"
				m_filterValue: ""
				m_filterValueType: "String"
				m_filterOperations: ["Not","Equal"]
			}
		}
	}
	
	Component.onCompleted: {
		table.setSortingInfo(UserItemDataTypeMetaInfo.s_name, "ASC");
		table.nonSortableColumns = [UserItemDataTypeMetaInfo.s_roles, UserItemDataTypeMetaInfo.s_groups]
		registerFieldFilterDelegate("SystemInfo", systemInfoDelegateFilterComp)
		table.rowDelegate = tableRowDelegateBaseComp;
	}
	
	Component {
		id: tableRowDelegateBaseComp;
		
		TableRowDelegate {
			tableItem: userCollectionViewContainer.table;
			width: userCollectionViewContainer.table.width;
			minHeight: userCollectionViewContainer.table.itemHeight;
			readOnly: userCollectionViewContainer.table.readOnly;
		}
	}
	
	onHeadersChanged: {
		table.setColumnContentById(UserItemDataTypeMetaInfo.s_roles, dataComp);
		table.setColumnContentById(UserItemDataTypeMetaInfo.s_groups, groupsContentComp);
	}
	
	Component {
		id: dataComp;
		
		TableCellDelegateBase {
			id: cellDelegate
			
			height: Style.controlHeightS;
			
			clip: true;
			
			onReused: {
				if (!rowDelegate){
					return
				}
				
				let username = userCollectionViewContainer.table.elements.getData(UserItemDataTypeMetaInfo.s_name, rowIndex);
				let roles = userCollectionViewContainer.table.elements.getData(UserItemDataTypeMetaInfo.s_roles, rowIndex);
				arrowButton.visible = roles !== "";
				if (roles !== ""){
					let roleList = roles.split(';');
					name.text = qsTr("View roles") + "(" + roleList.length +  ")";
					
					let result = "<b><u>" + username + "</u></b> " + qsTr("has been assigned") + " <b>" + roleList.length + "</b> " + qsTr("roles:");
					for (let i = 0; i < roleList.length; i++){
						result += "<p>" + roleList[i] + "</p>";
					}
					
					arrowButton.tooltipText = result;
				}
				else{
					name.text = qsTr("No roles");
				}
			}
			
			Text {
				id: name;
				
				anchors.verticalCenter: parent.verticalCenter;
				
				font.pixelSize: Style.fontSizeM;
				font.family: Style.fontFamily;
				color: Style.textColor;
				
				elide: Text.ElideRight;
			}
			
			ToolButton {
				id: arrowButton;
				
				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: name.right;
				anchors.leftMargin: Style.marginXS;
				
				width: Style.buttonWidthXS;
				height: width;
				
				iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
				
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent";
						icon.width: arrowButton.width;
					}
				}
			}
		}
	}
	
	Component {
		id: groupsContentComp;
		
		TableCellDelegateBase {
			id: cellDelegate
			
			onReused: {
				if (!rowDelegate){
					return
				}
				
				let username = userCollectionViewContainer.table.elements.getData(UserItemDataTypeMetaInfo.s_name, rowIndex);
				let groups = userCollectionViewContainer.table.elements.getData(UserItemDataTypeMetaInfo.s_groups, rowIndex);
				arrowButton.visible = groups !== "";
				if (groups !== ""){
					let groupList = groups.split(';');
					name2.text =  qsTr("View groups") + "(" + groupList.length +  ")";
					
					let result = "<b><u>" + username + "</u></b> " + qsTr("is included into") + " <b>" + groupList.length + "</b> " + qsTr("groups") + ":";
					for (let i = 0; i < groupList.length; i++){
						result += "<p>" + groupList[i] + "</p>";
					}
					arrowButton.tooltipText = result;
				}
				else{
					name2.text = qsTr("No groups");
				}
			}
			
			Item {
				anchors.fill: parent;
				
				clip: true;
				
				Text {
					id: name2;
					
					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: parent.left
					anchors.leftMargin: Style.marginM;
					
					font.pixelSize: Style.fontSizeM;
					font.family: Style.fontFamily;
					color: Style.textColor;
					
					elide: Text.ElideRight;
				}
				
				ToolButton {
					id: arrowButton;
					
					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: name2.right;
					anchors.leftMargin: Style.marginXS;
					
					width: 15;
					height: width;
					
					visible: name2.text !== "";
					
					iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
					
					decorator: Component {
						ToolButtonDecorator {
							color: "transparent";
							icon.width: 16;
						}
					}
				}
			}
		}
	}
	
}


