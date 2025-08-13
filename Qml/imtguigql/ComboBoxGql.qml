import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0


ComboBox {
	id: comboBoxContainerGql;

	shownItemsCount: 6

	// Data provider aliases
	property alias filter: dataProvider.filter
	property alias commandId: dataProvider.commandId;
	property alias subscriptionCommandId: dataProvider.subscriptionCommandId;
	property alias fields: dataProvider.fields;
	property alias textFilteringInfoIds: dataProvider.textFilteringInfoIds

	property int totalCount: -1;

	property string excludeFilterPart: "";
	property string filterText: "";

	property bool endListStatus: false;

	property Component popupDecorator: Component{ PopupDecoratorGql{} }

	// signals
	signal setCurrentText(var modelll, int index);
	signal editSignal();
	signal closeSignal(var model_);
	signal clearSignal();
	signal closeEmptySignal();

	popupMenuComp: Component {
		id: popupMenu;
		PopupMenuDialogGql {
			id: popup;

			decorator: comboBoxContainerGql.popupDecorator
			delegate: comboBoxContainerGql.delegate;
			hiddenBackground: comboBoxContainerGql.hiddenBackground;
			itemHeight: comboBoxContainerGql.itemHeight;
			textSize: comboBoxContainerGql.textSize;
			fontColor: comboBoxContainerGql.fontColor;

			model: comboBoxContainerGql.model;
			shownItemsCount: comboBoxContainerGql.shownItemsCount
			commandId: comboBoxContainerGql.commandId;
			dataProverState: dataProvider.state
			filterText: comboBoxContainerGql.filterText;
			selectedIndex: comboBoxContainerGql.currentIndex;

			moveToEnd: comboBoxContainerGql.moveToEnd;
			moveToIndex: comboBoxContainerGql.moveToIndex;
			visibleScrollBar: comboBoxContainerGql.visibleScrollBar;

			Component.onCompleted: {
				comboBoxContainerGql.finished.connect(popup.finished);
				popup.closeSignal.connect(comboBoxContainerGql.closeSignal);
				popup.clearSignal.connect(comboBoxContainerGql.clearSignal);
			}

			onStarted: {
				comboBoxContainerGql.popup = popup;
			}

			onFinished: {
				if (index >= 0){
					comboBoxContainerGql.currentIndex = index;
				}

				comboBoxContainerGql.isOpen = false;

				comboBoxContainerGql.setCurrentText(popup.model, index)
				if (comboBoxContainerGql.currentText == ""){
					comboBoxContainerGql.currentText = popup.filterText;
				}

				comboBoxContainerGql.currentIndex = index;
			}

			onDecorator_Changed: {
				if(!popup.decorator_){
					return
				}

				dataProvider.updateModel(0)
			}

			onEditSignal: {
				comboBoxContainerGql.filterText = filterText
				comboBoxContainerGql.currentIndex = -1;

				let str = filterText.replace(comboBoxContainerGql.excludeFilterPart, "");
				if(dataProvider.filter.hasTextFilter()){
					dataProvider.filter.m_textFilter.m_text = str
				}
				else{
					dataProvider.filter.setTextFilter(str)
				}

				pause.restart();
			}

			function setY(){
				if(popup.height == 0){
					return
				}

				let point = comboBoxContainerGql.mapToItem(null, 0, 0)
				let y_ = point.y;

				popup.isUpwards = (y_ + popup.height) > ModalDialogManager.activeView.height
				popup.y = popup.isUpwards ? point.y - popup.height : point.y;
				popup.opacity = 1;
			}

			function requestNextBatch(){
				let elementCount = popup.model.getItemsCount();
				let elemCountOk = elementCount >= 0 ? elementCount > dataProvider.offset : true;
				let doRequest = !comboBoxContainerGql.endListStatus
									&& dataProverState.toLowerCase() == "ready"
									&& elemCountOk;
				if(doRequest){
					dataProvider.updateModel(dataProvider.offset + dataProvider.count)
				}
			}
		}
	}

	function openPopupMenu(){
		let point = comboBoxContainerGql.mapToItem(null, 0, comboBoxContainerGql.height);
		let filterText_ = comboBoxContainerGql.keepFilterText ? comboBoxContainerGql.currentText : "";

		ModalDialogManager.openDialog(comboBoxContainerGql.popupMenuComp, {
										"x":	 point.x,
										"width": comboBoxContainerGql.width,
									});

		comboBoxContainerGql.isOpen = true;
	}

	function onMouseAreaClicked(){
		if (!comboBoxContainerGql.changeable){
			return;
		}

		comboBoxContainerGql.focus = true;
		comboBoxContainerGql.forceActiveFocus();

		comboBoxContainerGql.openPopupMenu();
		comboBoxContainerGql.clicked();
	}

	function setCustomFilters(inputParams){
	}

	PauseAnimation {
		id: pause;

		duration: 400;
		onFinished:{
			dataProvider.updateModel(0);
		}
	}

	CollectionDataProvider {
		id: dataProvider;

		offset: 0
		count: 15
		commandId: comboBoxContainerGql.commandId

		function setCustomInputParams(inputParams){
			comboBoxContainerGql.setCustomFilters(inputParams)
		}

		onFailed: {
			ModalDialogManager.showErrorDialog(message)
		}

		onModelUpdated: {
			let canClose = data.containsKey("close") && data.getData("close");

			if (dataProvider.notificationModel.containsKey("totalCount")){
				comboBoxContainerGql.totalCount = dataProvider.notificationModel.getData("totalCount");
			}

			if (dataProvider.offset == 0){
				comboBoxContainerGql.model = dataProvider.collectionModel
				comboBoxContainerGql.endListStatus = false;
				comboBoxContainerGql.currentIndex = -1;
				if(canClose){
					comboBoxContainerGql.closePopupMenu();
				}
			}
			else{
				comboBoxContainerGql.endListStatus = dataProvider.collectionModel.getItemsCount() <= 0
				if(!comboBoxContainerGql.endListStatus){
					for (let i = 0; i < dataProvider.collectionModel.getItemsCount(); i++){
						let index_ = comboBoxContainerGql.model.insertNewItem();
						dataProvider.collectionModel.copyItemDataToModel(i, comboBoxContainerGql.model, index_);
					}
				}
			}

			dataProvider.collectionModel.refresh();
		}
	}
}
