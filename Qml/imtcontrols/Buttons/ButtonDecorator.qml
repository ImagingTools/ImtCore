import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
	id: commonButtonDecorator

	width: Math.max(contentWidth + Style.sizeMainMargin * 2, widthDefault)
	height: Style.buttonHeight;

	clip: true;

	property int maxTextWidth: 200;
	property int minTextWidth: 20;
	property int iconSize: Style.sizeMainMargin;
	property int contentSpacing: Style.sizeMainMargin;
	property int contentLeftMargin: Style.sizeMainMargin;
	property alias contentWidth: content.width;

	property int widthDefault: 0;
	property alias icon: iconObj
	property alias font: textObj.font
	property string fontFamily: (baseElement && baseElement.font && baseElement.fontFamily) ? baseElement.font.fontFamily : Style.fontFamily
	property int fontSize:  (baseElement && baseElement.font) ? baseElement.font.pixelSize : Style.fontSizeXSmall
	property alias textColor: textObj.color
	property alias color: background.color
	property alias radius: background.radius
	property alias border: background.border

	property alias backgroundItem: background

	property bool contentCentered: true;
	property bool textIsCropped: textObj.text !== "" && helperText.width > textObj.width;
	property real maxContentWidth: helperText.width + 2*Style.sizeMainMargin;

	property string tooltipText: baseElement && baseElement.tooltipText !== "" ? baseElement.tooltipText : (textIsCropped ? textObj.text : "");
	property bool enabled: baseElement ? baseElement.enabled : false;

	signal mouseEntered(real mouseX, real mouseY);
	signal mouseExited(real mouseX, real mouseY);
	signal mousePositionChanged(real mouseX, real mouseY);

	onContentCenteredChanged: {
		setAnchors();
	}

	onContentLeftMarginChanged: {
		setAnchors();
	}

	function setAnchors(){
		if (contentCentered){
			content.anchors.left = undefined;
			content.anchors.horizontalCenter = commonButtonDecorator.horizontalCenter;
		}
		else{
			content.anchors.horizontalCenter = undefined;
			content.anchors.left = commonButtonDecorator.left;
			content.anchors.leftMargin = commonButtonDecorator.contentLeftMargin;
		}
	}

	onBaseElementChanged: {
		if(baseElement){
			baseElement.exited.connect(commonButtonDecorator.mouseExited);
			baseElement.entered.connect(commonButtonDecorator.mouseEntered);
			baseElement.positionChanged.connect(commonButtonDecorator.mousePositionChanged);
			baseElement.closeTooltip.connect(commonButtonDecorator.closeTooltip);
		}
	}

	onMousePositionChanged: {
		if(tooltip.text !== "" && enabled){
			if(mouseX >= 0 && mouseX <= baseElement.width && mouseY >= 0 && mouseY <= baseElement.height){
				tooltip.show(mouseX, mouseY);
			}
			else{
				closeTooltip();
			}
		}
	}

	onMouseExited: {
		closeTooltip();
	}

	onEnabledChanged: {
		if (!enabled){
			closeTooltip()
		}
	}

	Rectangle {
		id: background

		anchors.fill: parent
		radius: Style.buttonRadius
		color: !commonButtonDecorator.baseElement ? "transparent" :
													commonButtonDecorator.baseElement.down || commonButtonDecorator.baseElement.checked ?
														Style.baseColor : commonButtonDecorator.baseElement.hovered ?
															Style.baseColor : Style.backgroundColor2
		border.width: Style.buttonBorderWidth
		border.color: !commonButtonDecorator.baseElement ? "transparent" :
														   commonButtonDecorator.baseElement.focus ? Style.buttonBorderFocusColor :
																									 Style.borderColor
	}

	Item {
		id: content;

		anchors.verticalCenter: commonButtonDecorator.verticalCenter;
		anchors.horizontalCenter: commonButtonDecorator.horizontalCenter;

		width: iconObj.width + textItem.width + Style.sizeMainMargin;
		height: Math.max(iconObj.height, textItem.height)


		Image {
			id: iconObj

			anchors.verticalCenter: content.verticalCenter;
			anchors.left: parent.left;

			width: (!visible || !commonButtonDecorator.baseElement) ? 0 :
																	  (commonButtonDecorator.baseElement.icon && commonButtonDecorator.baseElement.icon.width !== 0) ?
																	  commonButtonDecorator.baseElement.icon.width : Style.iconSizeSmall

			height: !commonButtonDecorator.baseElement ? 0 :
														 (commonButtonDecorator.baseElement.icon && commonButtonDecorator.baseElement.icon.height !== 0) ?
														 commonButtonDecorator.baseElement.icon.height : width

			sourceSize.width: width
			sourceSize.height: height
			source: !commonButtonDecorator.baseElement ? "" :
														(commonButtonDecorator.baseElement.icon && commonButtonDecorator.baseElement.icon.source !== "") ?
															 commonButtonDecorator.baseElement.icon.source :
															 commonButtonDecorator.baseElement.iconSource ? commonButtonDecorator.baseElement.iconSource: "";
			visible: source != "" && source != undefined;
		}



		Item {
			id: textItem;

			anchors.centerIn: iconObj && iconObj.visible ? undefined : content;
			anchors.verticalCenter: content.verticalCenter;
			anchors.left: iconObj && iconObj.visible ? iconObj.right : undefined;
			anchors.leftMargin: iconObj && iconObj.visible ? commonButtonDecorator.contentSpacing : 0;

			width: visible ? helperText.width > commonButtonDecorator.maxTextWidth ? commonButtonDecorator.maxTextWidth : helperText.width : -Style.sizeMainMargin;
			height: textObj.height;

			visible: textObj.text !== "";



			Text {
				id: textObj

				width: parent.width;

				color: !commonButtonDecorator.baseElement ? "transparent" : (commonButtonDecorator.baseElement.font && commonButtonDecorator.baseElement.font.color !== "") ? commonButtonDecorator.baseElement.font.color : commonButtonDecorator.baseElement.enabled ? Style.textColor : Style.inactive_textColor

				font.pixelSize: commonButtonDecorator.fontSize;
				font.family: commonButtonDecorator.fontFamily;

				text: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.text
				elide: Text.ElideRight;
			}

			Text {
				id: helperText;

				font.pixelSize: commonButtonDecorator.fontSize;
				font.family: commonButtonDecorator.fontFamily;

				text: textObj.text;

				visible: false;
			}
		}
	}

	function closeTooltip(){
		if(tooltip && tooltip.openST){
			tooltip.closeTooltip();
		}
	}

	CustomTooltip {
		id: tooltip;

		text: commonButtonDecorator.tooltipText;
		fitToTextWidth: false;
		componentMinHeight: 30;

		fitToHCenter: true;

		function hide(){
			closeTooltip();
		}

		function show(xX, yY){
			if (text !== ""){
				openTooltip(xX, yY);
			}
		}
	}


}


