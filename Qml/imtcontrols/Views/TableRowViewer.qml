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
			width: loader.item ? loader.item.width :20
			height: dataList.height;

			clip: true;

			Component.onCompleted: {
				cell.compl = true;
			}

			onComplComplChanged: {
				if(cell.complCompl && dataList && dataList.rowDelegate && dataList.rowDelegate.tableItem && dataList.rowDelegate.tableItem.headers){
					let headerId = dataList.rowDelegate.tableItem.getHeaderId(model.index)
					let contents = dataList.rowDelegate.tableItem.columnContentComps;
					let contentComp = undefined;

					if (dataList.loadDefaultCellDelegate){
						contentComp = dataList.rowDelegate.tableItem.cellDelegate;
					}

					if (headerId.toLowerCase().endsWith("link")){
						contentComp = objectLinkDelegateComp;
					}

					if (Object.keys(contents).includes(headerId)){
						if (contents[headerId]){
							contentComp = contents[headerId];
						}
					}

					loader.sourceComponent = contentComp;
				}
			}

			Loader {
				id: loader;
				anchors.fill: parent
				property bool ready: item && dataList.rowDelegate;
				onReadyChanged: {
					if(ready){
						item.columnIndex = model.index
						item.rowDelegate = dataList.rowDelegate
						if(typeof item.setCellWidth === 'function'){
							item.setCellWidth()
						}
					}
				}

			}
		}
	}
}


