import QtQuick 2.12
import Acf 1.0


Item {
	id: delegateContainer;

	property Item pTableDelegateContainer: Item{}

	/// \workaround to calc cells count
	property Item pDataList: parent.parent

	property bool compl: false;
	property bool complCompl: delegateContainer.compl && pDataList.compl;

	property alias textLeftIndent: name.anchors.leftMargin
	property alias textRightIndent: name.anchors.rightMargin

	height: pTableDelegateContainer.height;
	width: pTableDelegateContainer.width/pTableDelegateContainer.count;

	Component.onCompleted: {
		delegateContainer.compl = true;
	}

	onComplComplChanged: {
		if(delegateContainer.complCompl){
			pTableDelegateContainer.widthRecalc.connect(delegateContainer.setCellWidth)
			delegateContainer.setCellWidth();
		}
	}


	function setCellWidth(){
		if(!delegateContainer.complCompl){
			return;
		}

		var defaultWidth = delegateContainer.pDataList.count == 0 ? 0 : delegateContainer.pTableDelegateContainer.width/pDataList.count;
		var widthFromModel = delegateContainer.pTableDelegateContainer.widthDecoratorDynamic.IsValidData("Width", model.index) ? delegateContainer.pTableDelegateContainer.widthDecoratorDynamic.GetData("Width", model.index) : -1;

		if(!delegateContainer.pTableDelegateContainer.widthDecoratorDynamic.GetItemsCount()){
			delegateContainer.width = defaultWidth;
		}
		else if(widthFromModel >= 0){
			delegateContainer.width = widthFromModel;
		}
		else{
			delegateContainer.width = defaultWidth;
		}
	}

	//borders

	Rectangle{
		id: topBorder;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: delegateContainer.pTableDelegateContainer.visibleTopBorderFirst  ? delegateContainer.pTableDelegateContainer.horizontalBorderSize : 0;
		color: delegateContainer.pTableDelegateContainer.borderColorHorizontal;
	}

	Rectangle{
		id: bottomBorder;
		anchors.bottom: parent.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: delegateContainer.pTableDelegateContainer.visibleBottomBorderLast ? delegateContainer.pTableDelegateContainer.horizontalBorderSize : 0;
		color:  delegateContainer.pTableDelegateContainer.borderColorHorizontal;
	}

	Rectangle{
		id: leftBorder;
		anchors.left: parent.left;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		width: delegateContainer.pTableDelegateContainer.isRightBorder ? delegateContainer.pTableDelegateContainer.verticalBorderSize * delegateContainer.pTableDelegateContainer.visibleLeftBorderFirst * (model.index == 0)
													: delegateContainer.pTableDelegateContainer.visibleLeftBorderFirst ? delegateContainer.pTableDelegateContainer.verticalBorderSize : model.index > 0 ? delegateContainer.pTableDelegateContainer.verticalBorderSize : 0;
		color:  delegateContainer.pTableDelegateContainer.borderColorVertical;
	}

	Rectangle{
		id: rightBorder;
		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		width: !delegateContainer.pTableDelegateContainer.isRightBorder ?
				   delegateContainer.pTableDelegateContainer.verticalBorderSize * delegateContainer.pTableDelegateContainer.visibleRightBorderLast  * (model.index == (delegateContainer.pTableDelegateContainer.count -1)) * (delegateContainer.pTableDelegateContainer.count > 0) :
				   delegateContainer.pTableDelegateContainer.visibleRightBorderLast ? delegateContainer.pTableDelegateContainer.verticalBorderSize  :
																   delegateContainer.pTableDelegateContainer.verticalBorderSize * (model.index < (delegateContainer.pTableDelegateContainer.count -1));

		color: delegateContainer.pTableDelegateContainer.borderColorVertical;
	}

	//borders
	Rectangle{
		id: mainRec;
		anchors.top: topBorder.bottom;
		anchors.left: leftBorder.right;
		anchors.right: rightBorder.left;
		anchors.bottom: bottomBorder.top;
		color: delegateContainer.pTableDelegateContainer.emptyDecorCell ? "transparent" :
													   delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("Color", model.index) ?
														   delegateContainer.pTableDelegateContainer.cellDecorator.GetData("Color", model.index) :
														   "transparent";

		opacity: delegateContainer.pTableDelegateContainer.emptyDecorCell ? 1 :
														 delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("Opacity", model.index) ?
															 delegateContainer.pTableDelegateContainer.cellDecorator.GetData("Opacity", model.index) :
															 1;

		radius: delegateContainer.pTableDelegateContainer.emptyDecorCell ? 0 :
														delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("CellRadius", model.index) ?
															delegateContainer.pTableDelegateContainer.cellDecorator.GetData("CellRadius", model.index) :0;

		//cornerPatches
		Rectangle{
			id: leftTopCornerPatch;
			anchors.left: parent.left;
			anchors.top: parent.top;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
															 delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("LeftTopRound", model.index) ?
																 !delegateContainer.pTableDelegateContainer.cellDecorator.GetData("LeftTopRound", model.index) :true;
		}

		Rectangle{
			id: rightTopCornerPatch;
			anchors.right: parent.right;
			anchors.top: parent.top;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
															 delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("RightTopRound", model.index) ?
																 !delegateContainer.pTableDelegateContainer.cellDecorator.GetData("RightTopRound", model.index) :true;
		}

		Rectangle{
			id: leftBottomCornerPatch;
			anchors.left: parent.left;
			anchors.bottom: parent.bottom;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
															 delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("LeftBottomRound", model.index) ?
																 !delegateContainer.pTableDelegateContainer.cellDecorator.GetData("LeftBottomRound", model.index) :true;
		}

		Rectangle{
			id: rightBottomCornerPatch;
			anchors.right:  parent.right;
			anchors.bottom: parent.bottom;
			width: parent.width/2;
			height: parent.height/2;
			color: parent.color;
			visible: delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
															 delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("RightBottomRound", model.index) ?
																 !delegateContainer.pTableDelegateContainer.cellDecorator.GetData("RightBottomRound", model.index) :true;
		}
		//cornerPatches
	}//mainRec

	Rectangle {
		id: selectionRec;

		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		anchors.topMargin: topBorder.height;
		anchors.bottomMargin: bottomBorder.height;

		color: delegateContainer.pTableDelegateContainer.selectedColor;
		opacity: delegateContainer.pTableDelegateContainer.selectedOpacity;

		radius: mainRec.radius;
		visible: delegateContainer.pTableDelegateContainer.selected && mainRec.color !== "transparent";
	}

	Text {
		id: name;

		anchors.verticalCenter: mainRec.verticalCenter;
		anchors.left: mainRec.left;
		anchors.right: mainRec.right;
		anchors.leftMargin: delegateContainer.pTableDelegateContainer.textLeftMargin;
		anchors.rightMargin: delegateContainer.pTableDelegateContainer.textRightMargin;

		verticalAlignment: Text.AlignVCenter;
		horizontalAlignment: delegateContainer.pTableDelegateContainer.emptyDecorCell ? Text.AlignLeft :
																	 delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("TextPosition", model.index) ?
																		 delegateContainer.pTableDelegateContainer.cellDecorator.GetData("TextPosition", model.index) :
																		 Text.AlignLeft;

		font.pixelSize: delegateContainer.pTableDelegateContainer.emptyDecorCell ? Style.fontSize_common:
																delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("FontSize", model.index) ?
																	delegateContainer.pTableDelegateContainer.cellDecorator.GetData("FontSize", model.index) :
																	Style.fontSize_common;

		font.family: Style.fontFamily;

		font.bold: delegateContainer.pTableDelegateContainer.emptyDecorCell ? true :
														   delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("FontBold", model.index) ?
															   delegateContainer.pTableDelegateContainer.cellDecorator.GetData("FontBold", model.index) :
															   true;

		color: delegateContainer.pTableDelegateContainer.emptyDecorCell ? Style.textColor :
													   delegateContainer.pTableDelegateContainer.cellDecorator.IsValidData("FontColor", model.index) ?
														   delegateContainer.pTableDelegateContainer.cellDecorator.GetData("FontColor", model.index) :
														   Style.textColor;

		elide: delegateContainer.pTableDelegateContainer.elideMode;
		wrapMode: delegateContainer.pTableDelegateContainer.wrapMode;
		text: delegateContainer.pTableDelegateContainer.dataModel[delegateContainer.pTableDelegateContainer.headers.GetData("Id", model.index)]

		onLinkActivated: {
			Qt.openUrlExternally(link);
		}

		onHeightChanged: {
			if(delegateContainer.pTableDelegateContainer.wrapMode !== Text.NoWrap){
				if(model.index < heightModel.count){
					var height_ = name.height +
							2*delegateContainer.pTableDelegateContainer.textMarginVer +
							topBorder.height + bottomBorder.height;

					heightModel.setProperty(model.index, "cellHeight", height_);
				}
			}
		}
	}
}//delegate

