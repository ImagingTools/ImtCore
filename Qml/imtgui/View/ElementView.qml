import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: rootElement
	
	width: Style.sizeHintXXL
	height: contentHeight + 2 * Style.marginXL
	
	border.width: 1
	border.color: Style.borderColor
	
	radius: Style.radiusM
	
	color: Style.baseColor
	
	property alias name: textName.text
	property alias description: textDescription.text
	
	property alias controlComp: controlLoader.sourceComponent
	property alias controlItem: controlLoader.item
	
	property alias topComp: topLoader.sourceComponent
	property alias topItem: topLoader.item
	
	property alias bottomComp: bottomLoader.sourceComponent
	property alias bottomItem: bottomLoader.item
	
	property bool topRoundedCorners: true
	property bool bottomRoundedCorners: true
	
	property alias contentWidth: content.width
	
	property alias nameWidth: textName.width
	
	property int controlWidth: Style.sizeHintM
	property int contentMargin: Style.marginXL
	property int contentHeight: content.height
	property int controlHeight: Style.controlHeightM
	
	onFocusChanged: {
		if (controlItem){
			controlItem.focus = focus
		}
	}
	
	Rectangle {
		id: topRect
		anchors.top: parent.top
		width: parent.width
		height: rootElement.radius
		visible: !rootElement.topRoundedCorners
		border.width: rootElement.border.width
		border.color: rootElement.border.color
		color: rootElement.color
		objectName: "ElementView"
	}
	
	Rectangle {
		anchors.left: parent.left
		anchors.leftMargin: rootElement.border.width
		anchors.right: parent.right
		anchors.rightMargin: rootElement.border.width
		anchors.top: topRect.bottom
		anchors.topMargin: -height
		height: rootElement.border.width
		color: rootElement.color
		objectName: "ElementView"
	}
	
	Rectangle {
		id: bottomRect
		anchors.bottom: parent.bottom
		width: parent.width
		height: rootElement.radius
		visible: !rootElement.bottomRoundedCorners
		border.width: rootElement.border.width
		border.color: rootElement.border.color
		color: rootElement.color
		objectName: "ElementView"
	}
	
	Rectangle {
		anchors.left: parent.left
		anchors.leftMargin: rootElement.border.width
		anchors.right: parent.right
		anchors.rightMargin: rootElement.border.width
		anchors.top: bottomRect.top
		height: rootElement.border.width
		color: rootElement.color
		objectName: "ElementView"
	}
	
	Column {
		id: content
		anchors.verticalCenter: parent.verticalCenter
		anchors.left: parent.left
		anchors.leftMargin: rootElement.contentMargin
		anchors.right: parent.right
		anchors.rightMargin: rootElement.contentMargin
		spacing: rootElement.contentMargin
		objectName: "ElementView"
		
		Loader {
			id: topLoader
			width: parent.width
			visible: rootElement.topComp != null && rootElement.topComp != undefined
		}
		
		Item {
			id: mainPart
			width: parent.width
			height: Math.max(textName.height, controlLoader.height)
			visible: textName.text !== "" || controlLoader.item
			
			Text {
				id: textName
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.right: controlLoader.left
				anchors.rightMargin: Style.marginXL
				color: Style.textColor
				font.family: Style.fontFamilyBold
				font.pixelSize: Style.fontSizeXL
				elide: Text.ElideRight
				wrapMode: Text.NoWrap
			}
			
			Loader {
				id: controlLoader
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
			}
		}
		
		Item {
			id: descriptionPart
			width: parent.width
			height: textDescription.text !== "" ? textDescription.height : 0
			visible: height > 0
			
			Text {
				id: textDescription
				anchors.verticalCenter: parent.verticalCenter
				width: parent.width
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
				elide: Text.ElideRight
			}
		}
		
		Item {
			id: bottomItem
			width: parent.width
			visible: bottomLoader.item !== undefined
			clip: true
			Loader {
				id: bottomLoader
				width: parent.width
				onItemChanged: {
					bottomItem.height = item ? item.height : 0
				}
			}
			Connections {
				target: bottomLoader.item
				function onHeightChanged(){
					bottomItem.height = bottomLoader.item.height
				}
			}
		}
	}
}


