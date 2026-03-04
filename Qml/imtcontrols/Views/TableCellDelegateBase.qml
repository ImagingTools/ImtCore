import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Item {
	id: delegateContainer;

	property TableRowDelegateBase rowDelegate: null;
	property var table: rowDelegate && rowDelegate.tableItem ? rowDelegate.tableItem : null

	height: rowDelegate ? rowDelegate.contentHeight : 0;
	width: !rowDelegate || !rowDelegate.columnCount  ? 0 : rowDelegate.width/rowDelegate.columnCount;

	property int columnCount: rowDelegate && rowDelegate.tableItem ? rowDelegate.tableItem.columnCount : 0;
	property int rowCount: rowDelegate && rowDelegate.tableItem ? rowDelegate.tableItem.elementsCount : 0;
	property real textLeftIndent: 0
	property real textRightIndent: 0

	property int columnIndex: -1
	property int rowIndex: rowDelegate ? rowDelegate.rowIndex : -1;
	property bool ready: rowDelegate;
	property bool emptyDecorCell: rowDelegate && rowDelegate.tableItem ? rowDelegate.tableItem.emptyDecorCell : true;

	property string cellHeaderId;
	property string sdlVarPrefix: "m_"

	property bool widthIsCalculated: false;
	property bool compl: false;

	property Item backgroundItem: null;
	property Item leftBorderItem: null;
	property Item rightBorderItem: null;
	property Item topBorderItem: null;
	property Item bottomBorderItem: null;

	signal reused();

	Component.onCompleted: {
		compl = true;
	}

	property bool ok: rowIndex >= 0 && rowDelegate != null
	onOkChanged: {
		if (ok){
			rowDelegate.reused.connect(reused)
			reused();
		}
	}

	Component.onDestruction: {
		if (rowDelegate && rowDelegate.tableItem){
			rowDelegate.reused.disconnect(delegateContainer.reused)
			rowDelegate.tableItem.widthRecalc.disconnect(delegateContainer.setCellWidth)
		}
	}

	onEmptyDecorCellChanged: {
		if (!delegateContainer.emptyDecorCell){
			if (delegateContainer.rowDelegate.cellColor !== "transparent"){
				if(!delegateContainer.backgroundItem){
					delegateContainer.backgroundItem = cellBackground.createObject(delegateContainer)
				}
			}

			if (delegateContainer.rowDelegate.verticalBorderSize){
				if(!delegateContainer.leftBorderItem){
					if(delegateContainer.columnIndex > 0){
						delegateContainer.leftBorderItem = leftBorder.createObject(delegateContainer)
					}
					else if(columnIndex == 0 && delegateContainer.rowDelegate.visibleLeftBorderFirst){
						delegateContainer.leftBorderItem = leftBorder.createObject(delegateContainer)
					}
				}
			}
			if (delegateContainer.rowDelegate.verticalBorderSize){
				if((delegateContainer.columnIndex == delegateContainer.columnCount -1) && delegateContainer.rowDelegate.visibleRightBorderLast){
					if(!delegateContainer.rightBorderItem){
						delegateContainer.rightBorderItem = rightBorder.createObject(delegateContainer)
					}
				}
			}

			if (delegateContainer.rowDelegate.horizontalBorderSize){
				if(!delegateContainer.topBorderItem){
					if(delegateContainer.rowCount && delegateContainer.rowIndex > 0 && delegateContainer.rowIndex < delegateContainer.rowCount){
						delegateContainer.topBorderItem = topBorder.createObject(delegateContainer)
					}
					else if(delegateContainer.rowDelegate.visibleTopBorderFirst && delegateContainer.rowIndex == 0 && delegateContainer.rowCount){
						delegateContainer.topBorderItem = topBorder.createObject(delegateContainer)
					}
				}
			}

			if (delegateContainer.rowDelegate.horizontalBorderSize){
				if(delegateContainer.rowDelegate.visibleBottomBorderLast && delegateContainer.rowIndex == delegateContainer.rowCount -1){
					if(!delegateContainer.bottomBorderItem){
						delegateContainer.bottomBorderItem = bottomBorder.createObject(delegateContainer)
					}
				}
			}
		}
	}

	onReadyChanged: {
		if(!ready){
			return;
		}

		delegateContainer.rowDelegate.tableItem.modelRefresh.connect(delegateContainer.refreshModelData)

		if (delegateContainer.rowDelegate.cellColor !== "transparent"){
			if(!delegateContainer.backgroundItem){
				delegateContainer.backgroundItem = cellBackground.createObject(delegateContainer)
			}
		}

		if (delegateContainer.rowDelegate.verticalBorderSize){
			if(!delegateContainer.leftBorderItem){
				if(delegateContainer.columnIndex > 0){
					delegateContainer.leftBorderItem = leftBorder.createObject(delegateContainer)
				}
				else if(columnIndex == 0 && delegateContainer.rowDelegate.visibleLeftBorderFirst){
					delegateContainer.leftBorderItem = leftBorder.createObject(delegateContainer)
				}
			}
		}
		if (delegateContainer.rowDelegate.verticalBorderSize){
			if((delegateContainer.columnIndex == delegateContainer.columnCount -1) && delegateContainer.rowDelegate.visibleRightBorderLast){
				if(!delegateContainer.rightBorderItem){
					delegateContainer.rightBorderItem = rightBorder.createObject(delegateContainer)
				}
			}
		}

		if (delegateContainer.rowDelegate.horizontalBorderSize){
			if(!delegateContainer.topBorderItem){
				if(delegateContainer.rowCount && delegateContainer.rowIndex > 0 && delegateContainer.rowIndex < delegateContainer.rowCount){
					delegateContainer.topBorderItem = topBorder.createObject(delegateContainer)
				}
				else if(delegateContainer.rowDelegate.visibleTopBorderFirst && delegateContainer.rowIndex == 0 && delegateContainer.rowCount){
					delegateContainer.topBorderItem = topBorder.createObject(delegateContainer)
				}
			}
		}

		if (delegateContainer.rowDelegate.horizontalBorderSize){
			if(delegateContainer.rowDelegate.visibleBottomBorderLast && delegateContainer.rowIndex == delegateContainer.rowCount -1){
				if(!delegateContainer.bottomBorderItem){
					delegateContainer.bottomBorderItem = bottomBorder.createObject(delegateContainer)
				}
			}
		}

		cellHeaderId = delegateContainer.rowDelegate.tableItem.getHeaderId(delegateContainer.columnIndex);

		delegateContainer.rowDelegate.tableItem.widthRecalc.connect(delegateContainer.setCellWidth)

		if (delegateContainer && delegateContainer.rowDelegate){
			if ("item" in delegateContainer.rowDelegate.dataModel){
				let funcChanged = delegateContainer.rowDelegate.dataModel.item[delegateContainer.sdlVarPrefix + cellHeaderId + 'Changed']
				if (funcChanged){
					funcChanged.connect(reused)
				}
			}
		}
	}

	function refreshModelData(rowIndex_, modelRole_){
		if(rowIndex_ === delegateContainer.rowIndex && modelRole_ === delegateContainer.cellHeaderId){
			reused();
		}
	}

	Component{
		id: cellBackground;
		Rectangle{
			anchors.fill: parent;
			z:-4;
			color: !delegateContainer.rowDelegate ? "transparent" : delegateContainer.rowDelegate.selected ? delegateContainer.rowDelegate.selectedColor: !delegateContainer.rowDelegate.tableItem ? "transparent" : (delegateContainer.rowDelegate.tableItem.enableAlternating && delegateContainer.rowDelegate.tableItem.alternatingCellColor !== "transparent" && delegateContainer.rowIndex % 2) ? delegateContainer.rowDelegate.tableItem.alternatingCellColor : delegateContainer.rowDelegate.cellColor;
		}
	}


	Component{
		id: topBorder;
		Rectangle{
			anchors.top: parent.top;
			anchors.left: parent.left;
			anchors.right: parent.right;
			height: !delegateContainer.rowDelegate ? 0 :
													 (delegateContainer.rowIndex > 0 && delegateContainer.rowIndex < delegateContainer.rowCount) ? delegateContainer.rowDelegate.horizontalBorderSize :
																																				   (delegateContainer.rowIndex == 0 && delegateContainer.rowDelegate.visibleTopBorderFirst) ? delegateContainer.rowDelegate.horizontalBorderSize : 0;
			color: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.borderColorHorizontal : "transparent";
		}
	}

	Component{
		id: bottomBorder;
		Rectangle{
			anchors.bottom: parent.bottom;
			anchors.left: parent.left;
			anchors.right: parent.right;

			height: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.visibleBottomBorderLast ? delegateContainer.rowDelegate.horizontalBorderSize : 0 : 0;
			color:  delegateContainer.rowDelegate ? delegateContainer.rowDelegate.borderColorHorizontal : "transparent";
		}
	}

	Component{
		id: leftBorder;
		Rectangle{
			anchors.left: parent.left;
			anchors.top: parent.top;
			anchors.bottom: parent.bottom;
			z: -1;

			width: !delegateContainer.rowDelegate ? 0 : delegateContainer.rowDelegate.verticalBorderSize;
			color: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.borderColorVertical : "transparent";
		}
	}

	Component{
		id: rightBorder;
		Rectangle{
			anchors.right: parent.right;
			anchors.top: parent.top;
			anchors.bottom: parent.bottom;
			z: -2;

			width: !delegateContainer.rowDelegate ? 0 : delegateContainer.rowDelegate.verticalBorderSize;
			color: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.borderColorVertical : "transparent";
		}
	}

	//cornerPatches
	Component{
		id: leftTopCornerPatch;
		Rectangle{
			anchors.left: parent.left;
			anchors.top: parent.top;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.tableItem.emptyDecorCell ? true :
																											  delegateContainer.rowDelegate.cellDecorator.isValidData("LeftTopRound", delegateContainer.columnIndex) ?
																												  !delegateContainer.rowDelegate.cellDecorator.getData("LeftTopRound", delegateContainer.columnIndex) :true : 0;
		}
	}

	Component{
		id: rightTopCornerPatch;
		Rectangle{
			anchors.right: parent.right;
			anchors.top: parent.top;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.tableItem.emptyDecorCell ? true :
																											  delegateContainer.rowDelegate.cellDecorator.isValidData("RightTopRound", delegateContainer.columnIndex) ?
																												  !delegateContainer.rowDelegate.cellDecorator.getData("RightTopRound", delegateContainer.columnIndex) :true : 0;
		}
	}

	Component{
		id: leftBottomCornerPatch;
		Rectangle{
			anchors.left: parent.left;
			anchors.bottom: parent.bottom;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: delegateContainer.rowDelegate ?
						 delegateContainer.rowDelegate.tableItem.emptyDecorCell ? true :
																				  delegateContainer.rowDelegate.cellDecorator.isValidData("LeftBottomRound", delegateContainer.columnIndex) ?
																					  !delegateContainer.rowDelegate.cellDecorator.getData("LeftBottomRound", delegateContainer.columnIndex) :true : 0;
		}
	}

	Component{
		id: rightBottomCornerPatch;
		Rectangle{
			anchors.right:  parent.right;
			anchors.bottom: parent.bottom;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: delegateContainer.rowDelegate ? delegateContainer.rowDelegate.tableItem.emptyDecorCell ? true :
																											  delegateContainer.rowDelegate.cellDecorator.isValidData("RightBottomRound", delegateContainer.columnIndex) ?
																												  !delegateContainer.rowDelegate.cellDecorator.getData("RightBottomRound", delegateContainer.columnIndex) :true : 0;
		}
	}

	function getValue(){
		if (delegateContainer && delegateContainer.columnIndex >= 0){
			if(delegateContainer.rowDelegate !== null && delegateContainer.rowDelegate.tableItem !==null && delegateContainer.rowDelegate.dataModel != null){
				let val
				let key = delegateContainer.rowDelegate.tableItem.getHeaderId(delegateContainer.columnIndex)
				if ("item" in delegateContainer.rowDelegate.dataModel){
					val = delegateContainer.rowDelegate.dataModel.item[delegateContainer.sdlVarPrefix + key]
				}
				else if(delegateContainer.rowDelegate.modelData){
					if(typeof delegateContainer.rowDelegate.modelData == 'object'){
						val = delegateContainer.rowDelegate.modelData[key];
					}
					else {
						val = delegateContainer.rowDelegate.modelData;
					}
				}
				else {
					val = delegateContainer.rowDelegate.dataModel[key];
				}

				return val !== undefined ? val : "";
			}
		}

		return "";
	}

	function setValue(value){
		if (delegateContainer && delegateContainer.columnIndex >= 0){
			if(rowDelegate !== null && rowDelegate.tableItem !==null){
				let tableItem = rowDelegate.tableItem;
				let elements = tableItem.elements;
				let headerId = delegateContainer.rowDelegate.tableItem.getHeaderId(delegateContainer.columnIndex);
				if ("item" in delegateContainer.rowDelegate.dataModel){
					delegateContainer.rowDelegate.dataModel.item[delegateContainer.sdlVarPrefix + headerId] = value;
				}
				else{
					elements.setData(headerId, value, delegateContainer.rowIndex);
				}
			}
		}
	}

	function setCellWidth(){
		if(!delegateContainer || !delegateContainer.rowDelegate){
			return;
		}

		let isFlickableTable = delegateContainer.rowDelegate.tableItem.isFlickable;
		let defaultWidth = isFlickableTable ? delegateContainer.rowDelegate.tableItem.defaultColumnWidth : delegateContainer.columnCount == 0 ? 0 : delegateContainer.rowDelegate.width/delegateContainer.columnCount;

		if(!delegateContainer.rowDelegate.tableItem.widthDecoratorDynamic.getItemsCount()){
			delegateContainer.width = defaultWidth;
		}
		else {
			let	widthFromModel = delegateContainer.rowDelegate.tableItem.widthDecoratorDynamic.isValidData("Width", delegateContainer.columnIndex) ?
						   delegateContainer.rowDelegate.tableItem.widthDecoratorDynamic.getData("Width", delegateContainer.columnIndex) : -1;

			if(widthFromModel >= 0){
				delegateContainer.width = widthFromModel;
			}
			else{
				delegateContainer.width = defaultWidth;
			}
		}
	}
}//delegate

