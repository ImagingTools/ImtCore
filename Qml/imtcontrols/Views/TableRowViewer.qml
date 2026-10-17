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

	// Fills every cell of the row in one pass, so the row is laid out once rather than once per cell.
	// A table that has spent its time slice gets the row on a later tick, which lets the first rows paint.
	function fillCells(){
		if (!dataList.rowDelegate || !dataList.rowDelegate.tableItem){
			return true;
		}

		let tableItem = dataList.rowDelegate.tableItem;
		if (typeof tableItem.takeRowFillSlice === 'function' && !tableItem.takeRowFillSlice()){
			fillTimer.start();
			return false;
		}

		// Inline rather than a function per cell, and contentItem set last: every QML function call is
		// its own update in the web runtime, so a cell resized inside one re-lays the row and the list out.
		for (let i = 0; i < repeater.count; i++){
			let cell = repeater.itemAt(i);
			if (!cell || !cell.complCompl || cell.contentItem){
				continue;
			}

			let contentComp = dataList.cellComponent(i);
			if (!contentComp){
				continue;
			}

			let obj = contentComp.createObject(cell);
			obj.columnIndex = i;
			obj.rowDelegate = dataList.rowDelegate;
			if (typeof obj.setCellWidth === 'function'){
				obj.setCellWidth();
			}

			cell.contentItem = obj;
		}

		return true;
	}

	// For a cell the Repeater does not list yet: added after the row was built, when the columns arrive late.
	function fillCell(cell, index){
		let contentComp = dataList.cellComponent(index);
		if (!contentComp){
			return;
		}

		let obj = contentComp.createObject(cell);
		obj.columnIndex = index;
		obj.rowDelegate = dataList.rowDelegate;
		if (typeof obj.setCellWidth === 'function'){
			obj.setCellWidth();
		}

		cell.contentItem = obj;
	}

	// Repeats until the row is filled: the web runtime stops a timer after its handler, which drops a restart() made inside it.
	// Not 0: Qt never fires a repeating Timer with a zero interval.
	Timer {
		id: fillTimer
		interval: 1
		repeat: true
		onTriggered: {
			if (dataList.fillCells()){
				fillTimer.stop();
			}
		}
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

				if (dataList.fillCells() && !cell.contentItem){
					dataList.fillCell(cell, model.index);
				}
			}
		}
	}
}


