import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0


ComboBox {
	id: comboBoxContainerGql;

	shownItemsCount: 6

	// Data provider aliases
	property alias dataProvider: dataProvider
	property alias filter: dataProvider.filter
	property alias commandId: dataProvider.commandId;

	property string excludeFilterPart: "";
	property string filterText: "";

	property bool endListStatus: false;
	property bool closeOnFinished: true;
	property bool keepFilterText: false;

	property int modelUpdateDelay: 200

	property Component popupDecorator: Style.popupDecoratorGql

	onFilterTextChanged: {
		if(comboBoxContainerGql.popup){
			comboBoxContainerGql.popup.filterText = filterText
		}
	}

	// signals
	signal editSignal();
	signal clearSignal();
	signal closeAction()

	popupMenuComp: Component {
		id: popupMenu;
		PopupMenuDialogGql {
			id: popup;

			comboDecorator: comboBoxContainerGql.decorator_;
			decorator: comboBoxContainerGql.popupDecorator
			delegate: comboBoxContainerGql.delegate;
			hiddenBackground: comboBoxContainerGql.hiddenBackground;
			itemHeight: comboBoxContainerGql.itemHeight;
			radius: comboBoxContainerGql.radius;
			textSize: comboBoxContainerGql.textSize;
			fontColor: comboBoxContainerGql.fontColor;

			model: comboBoxContainerGql.model;
			shownItemsCount: comboBoxContainerGql.shownItemsCount
			commandId: comboBoxContainerGql.commandId;
			dataProviderState: dataProvider.state
			filterText: comboBoxContainerGql.filterText;
			selectedIndex: comboBoxContainerGql.currentIndex;

			moveToEnd: comboBoxContainerGql.moveToEnd;
			moveToIndex: comboBoxContainerGql.moveToIndex;
			visibleScrollBar: comboBoxContainerGql.visibleScrollBar;

			onFilterTextChanged: { comboBoxContainerGql.filterText = filterText }

			Component.onCompleted: {
				popup.clearSignal.connect(comboBoxContainerGql.clearSignal);
			}

			onStarted: {
				comboBoxContainerGql.popup = popup;
			}

			onFinished: {
				if (comboBoxContainerGql.currentText == ""){
					comboBoxContainerGql.currentText = popup.filterText;
				}

				if (index < 0){
					comboBoxContainerGql.isOpen = false;
					closeCombo()
				}
				else{
					comboBoxContainerGql.currentIndex = index;
					if(comboBoxContainerGql.closeOnFinished){
						comboBoxContainerGql.isOpen = false
						closeCombo()
					}
				}

				comboBoxContainerGql.finished(commandId, index)
			}

			onDecorator_Changed: {
				if(!popup.decorator_){
					return
				}

				comboBoxContainerGql.updateModel(0)
			}

			onEditSignal: {
				comboBoxContainerGql.filterText = filterText
				comboBoxContainerGql.currentIndex = -1;

				let str = filterText.replace(comboBoxContainerGql.excludeFilterPart, "");
				comboBoxContainerGql.setTextFilter(str)

				pause.restart();
			}

			function close(){}

			function closeCombo(){
				if(root){
					root.closeDialog();
					comboBoxContainerGql.closeAction()
				}
			}

			function setY(){
				if(popup.height == 0){
					return
				}

				let point = comboBoxContainerGql.mapToItem(null, 0, 0)
				let y_ = point.y;

				popup.isUpwards = (y_ + popup.height) > ModalDialogManager.activeView.height
				popup.y = popup.isUpwards ? point.y - popup.height + popup.comboDecorator.height : point.y;
				popup.opacity = 1;
			}

			function requestNextBatch(){
				let elementCount = popup.model.getItemsCount();
				let elemCountOk = elementCount >= 0 ? elementCount > dataProvider.offset : true;
				let doRequest = !comboBoxContainerGql.endListStatus
									&& dataProviderState.toLowerCase() == "ready"
									&& elemCountOk;
				if(doRequest){
					comboBoxContainerGql.updateModel(dataProvider.offset + dataProvider.count)
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
										"filterText": filterText_,
									});

		comboBoxContainerGql.isOpen = true;
	}

	function updateModel(offset){
		if(offset !== undefined && offset !== null && offset >= 0){
			dataProvider.offset = offset
		}

		pause.restart()
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

	function setTextFilter(text){
		if(dataProvider.filter.hasTextFilter()){
			dataProvider.filter.m_textFilter.m_text = text
		}
		else{
			dataProvider.filter.setTextFilter(text)
		}
	}

	function setCustomFilters(inputParams){
	}

	PauseAnimation {
		id: pause;

		duration: comboBoxContainerGql.modelUpdateDelay;
		onFinished:{
			dataProvider.updateModel();
		}
	}

	CollectionDataProvider {
		id: dataProvider;

		offset: 0
		count: comboBoxContainerGql.shownItemsCount + 10
		commandId: comboBoxContainerGql.commandId

		function setCustomInputParams(inputParams){
			comboBoxContainerGql.setCustomFilters(inputParams)
		}

		onFailed: {
			ModalDialogManager.showErrorDialog(message)
		}

		onModelUpdated: {
			let closeImmediatly = false;
			if (data.containsKey("close") && data.getData("close")){
				comboBoxContainerGql.closeOnFinished = closeImmediatly = data.getData("close");
			}

			if (dataProvider.offset == 0){
				comboBoxContainerGql.model = dataProvider.collectionModel
				comboBoxContainerGql.endListStatus = false;
				comboBoxContainerGql.currentIndex = -1;

				if(comboBoxContainerGql.popup && closeImmediatly){
					comboBoxContainerGql.popup.finished('', -1)
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
