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
			revisionsPermissionPath: "/Administration/Users/Revisions";
			collectionView: userCollectionViewContainer;
			documentManager: userCollectionViewContainer.documentManager;

			Component.onCompleted: {
				registerDocumentType("User", qsTr("User"))
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

	Component {
		id: systemInfoDelegateFilterComp
		FieldFilterDelegate {
			id: systemInfoDelegateFilter
			name: qsTr("Source")

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
		table.setColumnContentById(UserItemDataTypeMetaInfo.s_lastConnection, lastConnectionContentComp);
	}

	Component {
		id: lastConnectionContentComp;

		TableCellDelegateBase {
			id: lastConnectionCell;

			onReused: {
				let lastConnection = String(lastConnectionCell.getValue());
				lastConnectionText.text = lastConnection !== "" ? lastConnection : qsTr("Never");
				lastConnectionText.color = lastConnection !== "" ? Style.textColor : Style.inactiveTextColor;
			}

			Text {
				id: lastConnectionText;

				anchors.left: parent.left;
				anchors.leftMargin: Style.marginM;
				anchors.right: parent.right;
				anchors.verticalCenter: parent.verticalCenter;

				font.pixelSize: Style.fontSizeM;
				font.family: Style.fontFamily;
				elide: Text.ElideRight;
			}
		}
	}
}
