import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0

RemoteCollectionView {
	id: log;
	
	width: parent.width
	height: Style.sizeHintXXS;
	
	commandsControllerComp: null
	table.enableAlternating: false

	onHeadersChanged: {
		if (log.table.headers.getItemsCount() > 0){
			log.table.tableDecorator = logTableDecoratorModel
			log.table.setColumnContentComponent(0, messageColumnContentComp);
		}
	}
	
	Component.onCompleted: {
		table.setSortingInfo("timeStamp", "DESC")
		filterMenu.decorator = messageCollectionFilterComp;
	}

	function getHeaders(){
		return {};
	}
	
	dataControllerComp: Component { CollectionRepresentation {
			collectionId: log.collectionId
			gqlGetListCommandId: log.gqlGetListCommandId
			function getHeaders(){
				return log.getHeaders()
			}
		} }
	
	
	TreeItemModel {
		id: logTableDecoratorModel;
		
		Component.onCompleted: {
			var cellWidthModel = logTableDecoratorModel.addTreeModel("CellWidth");
			
			let index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", 200, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", 300, index);
		}
	}
	
	Component {
		id: messageCollectionFilterComp;
		
		MessageCollectionFilterDecorator {
			complexFilter: log.collectionFilter
		}
	}
	
	Component {
		id: messageColumnContentComp;
		TableCellIconTextDelegate {
			id: cellDelegate
			onReused: {
				if (rowIndex >= 0){
					let category = rowDelegate.tableItem.elements.getData("category", rowIndex);
					if (category === 0){
						icon.source = "../../../../" + Style.getIconPath("Icons/Diagnostics", Icon.State.On, Icon.Mode.Normal);
					}
					else if (category === 1){
						icon.source = "../../../../" + Style.getIconPath("Icons/Info", Icon.State.On, Icon.Mode.Normal);
					}
					else if (category === 2){
						icon.source = "../../../../" + Style.getIconPath("Icons/Warning", Icon.State.On, Icon.Mode.Normal);
					}
					else if (category === 3){
						icon.source = "../../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal);
					}
					else if (category === 4){
						icon.source = "../../../../" + Style.getIconPath("Icons/Critical", Icon.State.On, Icon.Mode.Normal);
					}
				}
			}
			
			MouseArea {
				id: ma;
				
				anchors.fill: parent
				propagateComposedEvents: true;
				onDoubleClicked: {
					var parameters = {"centered": true, "message": tooltipArea.text};
					ModalDialogManager.openDialog(messageDialogComp, parameters);
				}
			}
			
			TooltipArea {
				id: tooltipArea;
				
				mouseArea: ma;
				
				text: cellDelegate.getValue()
			}
		}
	}
	
	Component {
		id: messageDialogComp;
		MessageDialog {
			title: "Message";
			Component.onCompleted: {
				clearButtons()
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}
		}
	}
}
