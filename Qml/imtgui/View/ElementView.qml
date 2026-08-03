import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0

Rectangle {
	id: rootElement
	objectName: name.replace(/\s/g, '') + "ElementView"
	
	width: Style.sizeHintXXL
	height: contentHeight + 2 * Style.marginXL
	
	border.width: 1
	border.color: Style.borderColor
	
	radius: Style.radiusM
	
	color: Style.baseColor
	property alias contentSpacing: content.spacing

	property int titleFontSize:  Style.fontSizeXL;
	
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
	property int nameMargin: Style.marginXL
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
			height: textName.text === "" && !controlLoader.item ? 0
					: mainPart.stacked ? textName.height + Style.marginS + controlHolder.height
					: Math.max(Style.controlHeightM, textName.height)
			visible: textName.text !== "" || controlLoader.item

			// Squeezed far enough that the name would have no room left beside its
			// control, the two stop sharing a line and stack instead - the only
			// way neither ends up drawn over the other. Guarded on a real width,
			// or a row would stack while it was still being built and had none.
			property bool stacked: mainPart.width > 0 && controlHolder.width > 0 && textName.text !== ""
					&& (mainPart.width - controlHolder.width - rootElement.nameMargin) < Style.sizeHintXXXS

			// Placed by y rather than by swapping anchors between verticalCenter
			// and top: bound through a ternary each of those is set independently,
			// and the moment both hold a value Qt drops the pair and leaves the
			// item wherever the conflict left it - which is what sent the control
			// down the row and kept it there.
			Text {
				id: textName
				y: mainPart.stacked ? 0 : Math.round((mainPart.height - height) / 2)
				anchors.left: parent.left
				anchors.right: mainPart.stacked ? parent.right : controlHolder.left
				anchors.rightMargin: mainPart.stacked ? 0 : rootElement.nameMargin
				color: Style.textColor
				font.family: Style.fontFamilyBold
				font.pixelSize: rootElement.titleFontSize
				elide: Text.ElideRight
				wrapMode: Text.NoWrap
			}

			// Measures the control instead of being measured by it. The Loader
			// carried no size, so until whatever it holds reported an implicit
			// one the name was told the control started at the right-hand edge -
			// and controls that set a plain width never report one at all. That
			// is why a row only came out right after the page had been resized.
			Item {
				id: controlHolder

				y: mainPart.stacked ? textName.height + Style.marginS
					: Math.round((mainPart.height - height) / 2)
				anchors.right: parent.right

				width: controlLoader.item ? controlLoader.item.width : 0
				height: controlLoader.item ? controlLoader.item.height : 0

				Loader {
					id: controlLoader
				}
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
			id: bottomItemPart
			width: parent.width
			// Left standing even with nothing in it: every element in the code
			// base is laid out around the row of column spacing this reserves.
			visible: bottomLoader.item !== undefined
			// Bound rather than assigned from a pair of handlers, which left the
			// height at whatever the last signal happened to report - and never
			// followed a bottom item that changed size afterwards.
			height: bottomLoader.item ? bottomLoader.item.height : 0
			clip: true

			Loader {
				id: bottomLoader
				width: parent.width
			}
		}

	}
}


