import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

Item {
	id: comboBoxContainerGql;

	width: 90;
	height: Style.controlHeightM;

	property var model;
	property TreeItemModel properties : TreeItemModel{};
	property TreeItemModel gettedParams : TreeItemModel{};
	property TreeItemModel filterIdsModel : TreeItemModel{};

	property color borderColor: comboBoxContainerGql.focus ? Style.iconColorOnSelected : Style.alternateBaseColor;

	property color backgroundColor: Style.baseColor;

	property string currentText;

	property bool textCentered: false;
	property bool menuVisible: false;
	property bool isColor: false;
	property bool backVisible: true;
	property bool hiddenBackground: true;
	property bool canClose: true;
	property bool closeEmpty: false;
	property bool complexModel: false;
	property bool openST: false;
	property bool preventFirstLoading: false;

	property int radius: Style.radiusM;
	property int currentIndex: -1;
	property int offset: 0;
	property int count: 15;
	property int countVisibleItem: 5;
	property int delegateRadius: 0;
	property int itemHeight: 26;
	property int filterHeiht: Style.controlHeightM;
	property int textSize: Style.fontSizeM;
	property int textDelegateSize: Style.fontSizeM;
	property int dialogsCountPrev: 1000;
	property int dialogsCount: ModalDialogManager.count;

	property string commandId: "";
	property string filterName: nameId;
	property string filterText: "";
	property string fontColor: Style.textColor;
	property string fontColorTitle: fontColor;
	property bool keepFilterText: false;
	property bool inFocus: false;
	property int pauseDuration: 500;

	property string excludeFilterPart: "";
	property bool canUpdateModel: true;
	property string additionalFilter: "";

	// ID for display in combo box delegates
	property string nameId: "name";

	property int selectedIndex: -1;
	property bool hoverBlocked: true;
	property bool doNotCorrectPosition : false;
	property bool readOnly: false;
	property bool isUpwards: false;

	property Component delegate: PopupMenuDelegate{
		width: comboBoxContainerGql.width;
		height: comboBoxContainerGql.itemHeight;

		text: model[comboBoxContainerGql.nameId];

		selected: comboBoxContainerGql.selectedIndex === model.index;

		onClicked: {
			let resultId = model.Id || model.id || model.item.m_id || "";
			comboBoxContainerGql.finished(resultId, model.index)
		}

		onEntered: {
			comboBoxContainerGql.selectedIndex = model.index;
		}
	};

	property alias popupComp: popupMenu;
	property alias gradient: cbMainRect.gradient;

	property alias image: cbArrowIcon;
	property alias imageSource: cbArrowIcon.source;
	property alias imageRotation: cbArrowIcon.rotation;

	property alias tooltipText: tooltip.text;
	property alias tooltipItem: tooltip;

	signal setCurrentText(var modelll, int index);
	signal clicked();
	signal finished(string commandId, int index);
	signal editSignal();
	signal closeSignal(var model_);
	signal clearSignal();
	signal closeEmptySignal();

	onSetCurrentText: {
		if(!comboBoxContainerGql.complexModel){
			comboBoxContainerGql.currentText = modelll.getData(comboBoxContainerGql.nameId, index);
		}
	}

	onOpenSTChanged: {
		selectedIndex = -1;
	}

	onModelChanged: {
		if (comboBoxContainerGql.currentIndex > -1){
			comboBoxContainerGql.currentText = popup.model.getData(nameId);
		}
	}

	onDialogsCountChanged: {

		comboBoxContainerGql.openST = comboBoxContainerGql.dialogsCount > comboBoxContainerGql.dialogsCountPrev;
		if(!comboBoxContainerGql.openST && comboBoxContainerGql.dialogsCountPrev < 1000){
			comboBoxContainerGql.dialogsCountPrev = 1000;
		}
	}

	onPropertiesChanged: {
		comboBoxContainerGql.currentIndex = -1;
		comboBoxContainerGql.currentText = "";
	}

	function popupDestruction(){
		console.log("popupDestruction");
	}

	Component {
		id: popupMenu;
		PopupMenuDialogGqlLegacy {
			id: popup;

			opacity: 0;
			rootItem: comboBoxContainerGql;
			offset: comboBoxContainerGql.offset;
			count: comboBoxContainerGql.count;
			commandId: comboBoxContainerGql.commandId;
			filterName: comboBoxContainerGql.filterName;
			delegate: comboBoxContainerGql.delegate;
			properties: comboBoxContainerGql.properties;
			gettedParams: comboBoxContainerGql.gettedParams;
			filterIdsModel: comboBoxContainerGql.filterIdsModel;
			delegateRadius: comboBoxContainerGql.delegateRadius;
			hiddenBackground: comboBoxContainerGql.hiddenBackground;
			itemHeight: comboBoxContainerGql.itemHeight;
			filterHeight: comboBoxContainerGql.filterHeiht;
			textSize: comboBoxContainerGql.textSize;
			fontColor: comboBoxContainerGql.fontColor;
			canClose: comboBoxContainerGql.canClose;
			preventFirstLoading: comboBoxContainerGql.preventFirstLoading;
			pauseDuration: comboBoxContainerGql.pauseDuration;
			excludeFilterPart: comboBoxContainerGql.excludeFilterPart;
			canUpdateModel: comboBoxContainerGql.canUpdateModel;
			additionalFilter: comboBoxContainerGql.additionalFilter;
			isUpwards: comboBoxContainerGql.isUpwards;
			// doNotCorrectPosition: comboBoxContainerGql.doNotCorrectPosition; !!!!!!!!!!!!

			// function getCurrentText(index){
			// 	return popup.model.getData("name",index);
			// }
			// Connections{
			// 	target: comboBoxContainerGql;
			// 	onFinished: popup.finished(commandId, index)
			// }

			Component.onCompleted: {
				comboBoxContainerGql.finished.connect(popup.finished);
				popup.clearSignal.connect(comboBoxContainerGql.clearSignal);
			}

			onHeightChanged: {
				popup.setY();
			}

			function setY(){
				if(popup.height === 0){
					return
				}
				if(comboBoxContainerGql.isUpwards){
					let point = comboBoxContainerGql.mapToItem(null, 0, 0)
					popup.y = point.y;
				}
				popup.opacity = 1;
			}

			onFilterTextChanged: {
				// comboBoxContainerGql.filterText = popup.filterText;
				// comboBoxContainerGql.currentIndex = -1;
				// comboBoxContainerGql.currentText = popup.filterText;
			}
			onFinished: {
				if (index > -1){
					for (var item = 0; item < comboBoxContainerGql.gettedParams.getItemsCount(); item++){
						let param = comboBoxContainerGql.gettedParams.getData(comboBoxContainerGql.nameId, item);
						let value = popup.model.getData(param, index);
						comboBoxContainerGql.gettedParams.setData("Value", value, item);
					}
				}
				// comboBoxContainerGql.currentText = popup.model.getData(comboBoxContainerGql.nameId, index);
				comboBoxContainerGql.setCurrentText(popup.model, index)
				if (comboBoxContainerGql.currentText == ""){
					comboBoxContainerGql.currentText = popup.filterText;
				}

				comboBoxContainerGql.currentIndex = index;

				if(popup.canClose){
					popup.root.closeDialog();
				}
			}
			onPropertiesChangedSignal: {
				comboBoxContainerGql.currentIndex = -1;
				comboBoxContainerGql.currentText = "";
			}
			onDestructionSignal: {
				comboBoxContainerGql.popupDestruction();
			}
		}
	}

	Component.onCompleted: {
		if (comboBoxContainerGql.textCentered){
			cbTitleTxt.anchors.horizontalCenter = cbMainRect.horizontalCenter;
		}
		else {
			cbTitleTxt.anchors.left = cbMainRect.left;
			cbTitleTxt.anchors.leftMargin = 10;
		}
	}

	function openPopupMenu(){
		comboBoxContainerGql.dialogsCountPrev = ModalDialogManager.count;
		let filterText_ = comboBoxContainerGql.keepFilterText ? comboBoxContainerGql.currentText : "";
		let point = comboBoxContainerGql.mapToItem(null, 0, 0);
		let y_ = point.y;
		if(y_ + comboBoxContainerGql.height + comboBoxContainerGql.countVisibleItem * comboBoxContainerGql.itemHeight > ModalDialogManager.activeView.height){
			comboBoxContainerGql.isUpwards = true;
		}
		else {
			comboBoxContainerGql.isUpwards = false;
		}

		let params = {};
		if(!comboBoxContainerGql.isUpwards){
			params = {
				"x": point.x,
				"y": point.y,
				"filterText": filterText_,
				"model": comboBoxContainerGql.model,
				"width": comboBoxContainerGql.width,
				"countVisibleItem": comboBoxContainerGql.countVisibleItem}
		}
		else {
			params = {
				"x": point.x,
				"filterText": filterText_,
				"model": comboBoxContainerGql.model,
				"width": comboBoxContainerGql.width,
				"countVisibleItem": comboBoxContainerGql.countVisibleItem}
		}

		ModalDialogManager.openDialog(popupMenu, params);
	}

	function closeFunc(){
		if(comboBoxContainerGql.closeEmpty){
			ModalDialogManager.closeDialog();
			closeEmptySignal();
		}
	}

	Rectangle {
		id: cbMainRect;

		anchors.fill: parent;

		border.color: comboBoxContainerGql.borderColor;
		border.width: !comboBoxContainerGql.backVisible ? 0 :1;

		radius: comboBoxContainerGql.radius;
		color: comboBoxContainerGql.backgroundColor;


		gradient: Gradient {
			GradientStop { position: 0.0; color: comboBoxContainerGql.isColor ? cbMainRect.color : Style.imagingToolsGradient1; }
			GradientStop { position: 0.97; color: comboBoxContainerGql.isColor ? cbMainRect.color : Style.imagingToolsGradient2; }
			GradientStop { position: 0.98; color: comboBoxContainerGql.isColor ? cbMainRect.color : Style.imagingToolsGradient3; }
			GradientStop { position: 1.0; color: comboBoxContainerGql.isColor ? cbMainRect.color : Style.imagingToolsGradient4; }
		}

		Text {
			id: cbTitleTxt;

			anchors.verticalCenter: parent.verticalCenter;
			anchors.left: parent.left;
			anchors.leftMargin: 10;
			anchors.right: parent.right;
			anchors.rightMargin: 20;

			clip: true;

			color: comboBoxContainerGql.fontColorTitle;
			font.family: Style.fontFamily;
			font.pixelSize: comboBoxContainerGql.textSize;
			visible: comboBoxContainerGql.backVisible;

			text: comboBoxContainerGql.currentText;
		}

		Image {
			id: cbArrowIcon;

			anchors.right: cbMainRect.right;
			anchors.verticalCenter: cbMainRect.verticalCenter;
			anchors.rightMargin: Style.marginXS;

			width: Style.iconSizeXS;
			height: Style.iconSizeXXS;

			source: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

			sourceSize.width: width;
			sourceSize.height: height;
		}

		MouseArea {
			id: cbMouseArea;

			anchors.fill: parent;
			hoverEnabled: !comboBoxContainerGql.readOnly;
			enabled: comboBoxContainerGql.enabled;

			cursorShape: hoverEnabled ? Qt.PointingHandCursor : Qt.ArrowCursor;

			onClicked: {
				if(!comboBoxContainerGql.readOnly){
					comboBoxContainerGql.openPopupMenu();
					comboBoxContainerGql.clicked();
				}
			}

			onPressed: {
				if(tooltip.text !== ""){
					tooltip.closeTooltip();
				}
			}

			onPositionChanged: {
				if(tooltip.text !== ""){
					tooltip.show(mouseX, mouseY);
				}
			}

			onExited: {
				if(tooltip.text !== ""){
					tooltip.hide();
				}
			}
		}

		CustomTooltip{
			id: tooltip;

			fitToTextWidth: true;
		}
	}
}
