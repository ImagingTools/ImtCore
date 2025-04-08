import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: container;
	
	color: Style.backgroundColor2;
	
	clip: true;
	
	property TreeItemModel metaInfoModel: TreeItemModel {}
	property int elementHeight: 20;
	property bool contentVisible: true;
	
	function startLoading(){
		loading.start();
	}
	
	function stopLoading(){
		loading.stop();
	}
	
	Flickable {
		id: collectionMetaInfo;
		
		anchors.fill: parent;
		
		contentWidth: width;
		contentHeight: column.height;
		
		boundsBehavior: Flickable.StopAtBounds;
		
		Column {
			id: column;
			
			anchors.top: parent.top;
			anchors.topMargin: Style.sizeMainMargin;
			anchors.left: parent.left;
			anchors.leftMargin: Style.sizeMainMargin;
			anchors.right: parent.right;
			anchors.rightMargin: Style.sizeMainMargin;
			
			visible: container.contentVisible;
			
			Repeater {
				id: repeaterColumn;
				
				model: container.metaInfoModel;
				
				delegate: Item {
					id: repeaterTitle;
					
					width: column.width;
					height: childColumn.height + nameTitle.height + container.elementHeight;
					
					Component.onCompleted: {
						repeaterChilds.model = repeaterColumn.model.getData("children", model.index);
					}
					
					Text {
						id: nameTitle;
						
						width: column.width;
						
						font.pixelSize: Style.fontSizeNormal;
						font.family: Style.fontFamilyBold;
						font.bold: true;
						
						color: Style.lightBlueColor;
						elide: Text.ElideRight;
						wrapMode: Text.WrapAnywhere ;
						
						text: model.name;
					}
					
					Column {
						id: childColumn;
						
						anchors.top: nameTitle.bottom;
						anchors.topMargin: Style.sizeSmallMargin;
						
						width: column.width;
						
						Repeater {
							id: repeaterChilds;
							
							width: column.width;
							
							delegate: Rectangle {
								height: container.elementHeight > valueText.contentHeight ? container.elementHeight : valueText.contentHeight;
								width: column.width;
								
								color: "transparent";
								
								clip: true;
								
								Image {
									id: icon
									
									anchors.verticalCenter: parent.verticalCenter
									anchors.left: parent.left;
									anchors.leftMargin: width > 0 ? Style.sizeMainMargin : 0;
									
									width: model.icon === undefined ? 0 : Style.iconSizeSmall
									height: width
									
									sourceSize.width: width
									sourceSize.height: height
									source: model.icon === undefined ? "" : "../../../" + Style.getIconPath(model.icon, Icon.State.On, Icon.Mode.Normal)
								}
								
								Text {
									id: valueText;
									
									anchors.verticalCenter: parent.verticalCenter
									anchors.left: icon.right;
									anchors.leftMargin: icon.width == 0 ? 0 : Style.sizeMainMargin;
									anchors.right: parent.right;

									font.family: Style.fontFamily;
									font.pixelSize: Style.fontSizeSmall;
									wrapMode: Text.WordWrap
									
									color: Style.textColor;
									
									elide: Text.ElideRight;
									
									text: model.value;
								}
							}
						}
					}
				}
			}
		}//Column main
	}
	
	Rectangle {
		anchors.fill: parent;
		color: parent.color;
		visible: loading.visible;
	}
	
	Loading {
		id: loading;
		
		anchors.top: parent.top;
		
		width: parent.width;
		height: 200;
		
		indicatorSize: 20;
		
		color: container.color;
		
		visible: false;
	}
}

