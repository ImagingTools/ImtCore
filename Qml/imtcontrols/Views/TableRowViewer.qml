import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Row {
	id: dataList;

	property TableRowDelegateBase rowDelegate: null;
	property bool compl: false;
	property alias model: repeater.model
	property bool loadDefaultCellDelegate: true;

	function cellComponent(columnIndex){
		if (!dataList.rowDelegate || !dataList.rowDelegate.tableItem){
			return null;
		}

		let tableItem = dataList.rowDelegate.tableItem;
		let headerId = tableItem.getHeaderId(columnIndex);
		let contentComp = null;

		if (dataList.loadDefaultCellDelegate){
			contentComp = tableItem.cellDelegate;
		}

		if (headerId && headerId.toLowerCase().endsWith("link")){
			contentComp = objectLinkDelegateComp;
		}

		let contents = tableItem.columnContentComps;
		if (contents && Object.keys(contents).includes(headerId) && contents[headerId]){
			contentComp = contents[headerId];
		}

		return contentComp;
	}

	Component {
		id: objectLinkDelegateComp
		TextLinkCellDelegate {
			id: objectLinkDelegate
			onLinkActivated: {
				let targetLink = getValue()
				if (targetLink && targetLink.containsKey("url")){
					let targetUrl = targetLink.getData("url")
					if (targetUrl && targetUrl.containsKey("path")){
						let path = targetUrl.getData("path")
						NavigationController.navigate(path)
					}
				}
			}

			onReused: {
				let targetLink = getValue()
				if (targetLink){
					text = targetLink.getData("name")
				}
			}
		}
	}

	Repeater {
		id: repeater

		Component.onCompleted: {
			dataList.compl = true;
		}

		delegate: Item {
			id: cell
			property bool compl: false;
			property bool complCompl: dataList.compl && dataList && dataList.rowDelegate && dataList.rowDelegate.tableItem && dataList.rowDelegate.tableItem.columnCount;
			property Item contentItem: null;
			width: cell.contentItem ? cell.contentItem.width : 20
			height: dataList.height;

			clip: true;

			Component.onCompleted: {
				cell.compl = true;
			}

			onComplComplChanged: {
				if (!cell.complCompl || cell.contentItem){
					return;
				}

				let contentComp = dataList.cellComponent(model.index);
				if (!contentComp){
					return;
				}

				let obj = contentComp.createObject(cell);
				cell.contentItem = obj;
				obj.columnIndex = model.index;
				obj.rowDelegate = dataList.rowDelegate;
				if (typeof obj.setCellWidth === 'function'){
					obj.setCellWidth();
				}
			}
		}
	}
}


