import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

DecoratorBase {
	id: filterPanelDecorator;
	
	height: content.height;
	
	property alias contentWidth: content.width;
	
	property CollectionFilter complexFilter: baseElement ? baseElement.complexFilter : null;
	
	Component.onCompleted: {
		updateText();
		checkWidth()
	}

	onWidthChanged: {
		checkWidth();
	}
	
	LocalizationEvent {
		onLocalizationChanged: {
			filterPanelDecorator.updateText();
		}
	}
	
	function checkWidth(){
		//        rect.visible = width - rect.width <= contentWidth + Style.sizeLargeMargin;
		//        tfc.visible = width - tfc.width <= contentWidth + Style.sizeLargeMargin;
	}
	
	function updateText(){
		tfc.placeHolderText = qsTr("Enter some text to filter the item list");
	}
	
	Component {
		id: timeFilterParamComp;
		
		TimeFilterParamView {
			onAccepted: {
				if (filterPanelDecorator.complexFilter){
					filterPanelDecorator.complexFilter.setTimeFilter(this.timeFilter);
					filterPanelDecorator.complexFilter.filterChanged()
				}
				
				// filterPanelDecorator.baseElement.filterChanged("TimeFilter", this.model);
				ModalDialogManager.closeByComp(timeFilterParamComp);
				rect.filterEnabled = true;
				
				buttonText.text = name;
			}
			
			onCancelled: {
				clearButton.clicked();
			}
		}
	}
	
	TimeFilter {
		id: timeFilter;
	}
	
	Row {
		id: content;
		anchors.right: filterPanelDecorator.right;
		height: tfc.height;
		spacing: Style.sizeMainMargin;
		
		Rectangle {
			id: rect;
			width: item.width;
			height: parent.height;
			color: filterEnabled ? Style.iconColorOnSelected : "transparent";
			border.width: 1;
			border.color: Style.borderColor;
			radius: 3;
			property bool filterEnabled: false;
			
			MouseArea {
				id: ma;
				anchors.fill: parent;
				cursorShape: Qt.PointingHandCursor;
				hoverEnabled: true;
				onClicked: {
					var point = mapToItem(null, x - width, y + height);
					ModalDialogManager.openDialog(timeFilterParamComp, {"x": point.x, "y": point.y, "timeFilter": timeFilter});
				}
			}
			
			Item {
				id: item;
				anchors.centerIn: parent;
				width: row.width + 2 * Style.sizeMainMargin;
				height: row.height;
				
				Row {
					id: row;
					anchors.horizontalCenter: parent.horizontalCenter;
					spacing: Style.sizeMainMargin;
					
					BaseText {
						id: buttonText;
						anchors.verticalCenter: parent.verticalCenter;
						text: !rect.filterEnabled ? qsTr("Date") : "";
						color: rect.filterEnabled ? Style.baseColor : Style.textColor;
					}
					
					ToolButton {
						id: clearButton;
						anchors.verticalCenter: parent.verticalCenter;
						width: 13;
						height: width;
						iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.Off, Icon.Mode.Disabled)
						visible: rect.filterEnabled;
						onClicked: {
							rect.filterEnabled = false;
							buttonText.text = qsTr("Date");
							
							if (filterPanelDecorator.complexFilter){
								filterPanelDecorator.complexFilter.clearTimeFilter();
								filterPanelDecorator.complexFilter.filterChanged()
							}
						}
						decorator: Component {
							ToolButtonDecorator {
								color: "transparent";
								icon.width: clearButton.width;
								textColor: "white";
							}
						}
					}
					
					ToolButton {
						id: downButton;
						anchors.verticalCenter: parent.verticalCenter;
						width: 13;
						height: width;
						iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
						visible: !rect.filterEnabled;
						onClicked: {
							ma.clicked();
						}
						decorator: Component {
							ToolButtonDecorator {
								color: "transparent";
								icon.width: downButton.width;
							}
						}
					}
				}
			}
		}
		
		CustomTextField {
			id: tfc;
			anchors.verticalCenter: content.verticalCenter;
			textFieldRightMargin: iconClear.width + 2 * margin;
			width: 270;
			height: 30;
			
			onTextChanged: {
				timer.restart();
			}
			
			Timer {
				id: timer;
				
				interval: 500;
				
				onTriggered: {
					if (filterPanelDecorator.complexFilter){
						filterPanelDecorator.complexFilter.setTextFilter(tfc.text);
						filterPanelDecorator.complexFilter.filterChanged()
					}
				}
			}
			
			ToolButton {
				id: iconClear;
				
				z: 999;
				
				anchors.verticalCenter: tfc.verticalCenter;
				anchors.right: tfc.right;
				anchors.rightMargin: Style.sizeMainMargin;
				
				width: Style.buttonWidthSmall;
				height: width;
				
				visible: tfc.text != "";
				
				iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent";
						icon.width: 16;
					}
				}
				
				onClicked: {
					tfc.text = "";
				}
			}
		}
		
		Item {
			width: Style.buttonWidthMedium;
			height: width;
		}
	}
}

