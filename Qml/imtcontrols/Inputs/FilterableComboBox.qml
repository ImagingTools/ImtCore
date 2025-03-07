import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ComboBox {
    id: comboBoxContainer;

    property string filter;
    property var filteringFields: [comboBoxContainer.nameId];
    property string descriptionFieldId;

    itemHeight: 35;

	property var sourceModel;

    delegate: Component {
		FilterableComboBoxDelegate {
            width: comboBoxContainer.width;
			comboBoxRef: comboBoxContainer;
        }
    }

	onSourceModelChanged: {
		updateFilterModel()
	}

	function updateFilterModel(){
		comboBoxContainer.model = 0;
		if (filter === ""){
			comboBoxContainer.model = sourceModel;
		}
	}

	onFilterChanged: {
		updateFilterModel();
	}

	onFinished: {
		if (index >= 0){
			if (itemId in proxyModel.sourceIndexes){
				comboBoxContainer.currentIndex = proxyModel.sourceIndexes[itemId]
			}
			else{
				comboBoxContainer.currentIndex = index;
			}
		}
	}

	SortFilterProxyModel {
		id: proxyModel;
		textFilter: comboBoxContainer.filter;
		sourceModel: comboBoxContainer.sourceModel;
		filterableFields: comboBoxContainer.filteringFields;
		onAccepted: {
			comboBoxContainer.model = proxyModel.filteredModel;
		}
	}

    property CustomTextField textInput;

    popupMenuComp: Component {
        id: popupMenu;

        PopupMenuDialog {
            id: popup;

            width: comboBoxContainer.width;
            itemHeight: comboBoxContainer.itemHeight;

            delegate: comboBoxContainer.delegate;
            hiddenBackground: comboBoxContainer.hiddenBackground;
            shownItemsCount: comboBoxContainer.shownItemsCount;
            moveToEnd: comboBoxContainer.moveToEnd;
            moveToIndex: comboBoxContainer.moveToIndex;
            visibleScrollBar: comboBoxContainer.visibleScrollBar;
            selectedIndex: comboBoxContainer.currentIndex;

            Component.onCompleted: {
                if(comboBoxContainer.decorator_){
                    comboBoxContainer.decorator_.textVisible = false;
                }
            }

            Component.onDestruction: {
                if(comboBoxContainer.decorator_){
                    comboBoxContainer.decorator_.textVisible = true;
                }

				comboBoxContainer.filter = "";
            }

			Connections {
				target: comboBoxContainer;

				function onModelChanged(){
					popup.model = comboBoxContainer.model;
				}
			}

            decorator: Component {PopupDecorator {
                    topContentLoaderSourceComp: Component {
                        Item {
                            id: contentItem;

							width: popup.width;
                            height: 25;

                            Component.onCompleted: {
                                popup.finished.connect(function(){ textField.text = "";});
                            }

                            CustomTextField {
                                id: textField;

                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.left: parent.left;
                                anchors.right: parent.right;
                                anchors.rightMargin: Style.sizeMainMargin;

                                height: parent.height;

                                color: "transparent";
                                borderWidth: 0;

                                text: comboBoxContainer.currentText;

								onEditingFinished: {
                                    if (text === "" || text != comboBoxContainer.currentText){
                                        comboBoxContainer.filter = text;
                                    }
                                }

                                Component.onCompleted: {
                                    comboBoxContainer.textInput = textField;
                                }
                            }
                        }
                    }
                }
            }

            onFinished: {
				comboBoxContainer.isOpen = false;
                comboBoxContainer.finished(commandId, index)
            }

            onStarted: {
                comboBoxContainer.popup = popup;
            }
        }
    }

    function openPopupMenu(){
        comboBoxContainer.isOpen = true;

        var point = comboBoxContainer.mapToItem(null, 0, comboBoxContainer.height);
        ModalDialogManager.openDialog(popupMenuComp, {
                                          "x":     point.x,
                                          "y":     point.y - (comboBoxContainer.height - 2),
                                          "model": comboBoxContainer.model,
                                          "width": comboBoxContainer.width});

        if (textInput){
            textInput.focus = true;
            textInput.forceActiveFocus();
        }
    }
}
