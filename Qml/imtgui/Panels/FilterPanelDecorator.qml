import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

DecoratorBase {
	id: filterPanelDecorator;
	
	width: content.width
	height: content.height;
	
	property alias contentWidth: content.width;
	
	property CollectionFilter complexFilter: baseElement ? baseElement.complexFilter : null;
	property DocCollectionFilter documentFilter: baseElement ? baseElement.documentFilter : null;
	
	Component.onCompleted: {
		updateText();
	}
	
	LocalizationEvent {
		onLocalizationChanged: {
			filterPanelDecorator.updateText();
		}
	}
	
	function updateText(){
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
		
		function clear(){
			m_timeRange.m_begin = ""
			m_timeRange.m_end = ""
			m_timeUnit = ""
			m_interpretationMode = ""
			m_unitMultiplier = 0
		}
	}
	
	Row {
		id: content;
		anchors.right: filterPanelDecorator.right;
		height: tfc.height;
		spacing: Style.marginM;
		
		ComboBox {
			id: stateCb
			height: parent.height;
			width: 250;
			currentIndex: 0;
			radius: 3;
			visible: filterPanelDecorator.documentFilter != null
			
			TreeItemModel {
				id: stateModel
				Component.onCompleted: {
					let index = insertNewItem();
					setData("id", "Active", index);
					setData("name", qsTr("Active Documents"), index);
					
					index = insertNewItem();
					setData("id", "Disabled", index);
					setData("name", qsTr("Disabled Documents"), index);
					
					stateCb.model = stateModel
				}
			}
			
			onCurrentIndexChanged: {
				filterPanelDecorator.documentFilter.clear()
				if (currentIndex == 0){
					filterPanelDecorator.documentFilter.addDocumentState(filterPanelDecorator.documentFilter.s_activeState)
				}
				else if (currentIndex == 1){
					filterPanelDecorator.documentFilter.addDocumentState(filterPanelDecorator.documentFilter.s_disabledState)
				}
				
				filterPanelDecorator.documentFilter.filterChanged()
			}
			
			FieldFilter {
				id: stateFilter
				m_fieldId: "State"
				m_filterValueType: "String"
				m_filterOperations: ["Equal"]
			}
		}
		
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
				width: row.width + 2 * Style.marginM;
				height: row.height;
				
				Row {
					id: row;
					anchors.horizontalCenter: parent.horizontalCenter;
					spacing: Style.marginM;
					
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
							timeFilter.clear();
							
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
		
		SearchTextInput {
			id: tfc;
			anchors.verticalCenter: content.verticalCenter;
			onSearchChanged: {
				if (filterPanelDecorator.complexFilter){
					filterPanelDecorator.complexFilter.setTextFilter(tfc.text);
					filterPanelDecorator.complexFilter.filterChanged()
				}
			}
		}
		
		Item {
			width: Style.buttonWidthM;
			height: width;
		}
	}
}

