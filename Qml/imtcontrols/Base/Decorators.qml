// pragma Singleton
import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0

StyleComponents {
	property string styleId: "StyleAcf"
	menuPanelButtonDecorator: menuPanelButtonDecoratorComp
	subMenuPanelButtonDecorator: subMenuPanelButtonDecoratorComp
	filterPanelDecorator: filterPanelDecoratorComp
	// bottomPanelDecorator: bottomPanelDecoratorComp
	topButtonDecorator: topButtonDecoratorComp
	topPanelDecorator: topPanelDecoratorComp
	topCenterPanelDecorator: topCenterPanelDecoratorComp
	topLeftPanelDecorator: topLeftPanelDecoratorComp
	topRightPanelDecorator: topRightPanelDecoratorComp

	tabPanelDecorator: tabPanelDecoratorComp

	buttonDecorator: buttonDecoratorComp
	firstSegmentDecorator: firstSegmentDecoratorComp
	middleSegmentDecorator: middleSegmentDecoratorComp
	lastSegmentDecorator: lastSegmentDecoratorComp

	toolButtonDecorator: toolButtonDecoratorComp;

	roundButtonDecorator: roundButtonDecoratorComp

	textFieldDecorator: textFieldDecoratorComp
	textEditDecorator: textEditDecoratorComp

	comboBoxDecorator: comboBoxDecoratorComp
	checkBoxDecorator: checkBoxDecoratorComp
	switchDecorator: switchDecoratorComp
	sliderDecorator: sliderDecoratorComp
	rangeSliderDecorator: rangeSliderDecoratorComp
	radioButtonDecorator: radioButtonDecoratorComp

	textAreaDecorator: textAreaDecoratorComp
	spinBoxDecorator: spinBoxDecoratorComp

	dialogDecorator: dialogDecoratorComp

	progressBarDecorator: progressBarDecoratorComp
	busyIndicatorDecorator: busyIndicatorDecoratorComp
	tooltipDecorator: tooltipDecoratorComp
	scrollBarDecorator: null;
	topPanelDialogDecorator: Component{TopPanelDialog{}}

	itemDelegateDecorator: itemDelegateDecoratorComp;

	popupDecorator: popupDecoratorComp;
	popupDecoratorGql: popupDecoratorGqlComp;
	popupItemDelegateDecorator: popupItemDelegateDecoratorComp;
	detailedPopupItemDelegateDecorator: detailedPopupItemDelegateDecoratorComp;

	collectionTableDecorator: Component{TreeItemModel{}}
	drawingContainerDecorator: null;

	Component {
		id: popupItemDelegateDecoratorComp;

		PopupItemDelegateDecorator {
		}
	}

	Component {
		id: detailedPopupItemDelegateDecoratorComp;

		PopupDetailedDelegateDecorator {
		}
	}

	Component {
		id: popupDecoratorComp;

		PopupDecorator {
		}
	}

	Component {
		id: popupDecoratorGqlComp;

		PopupDecoratorGql {
		}
	}

	Component {
		id: itemDelegateDecoratorComp;

		ItemDelegateDecorator {

		}
	}

	Component {
		id: menuPanelButtonDecoratorComp

		MenuPanelButtonDecorator{

		}
	}

	Component {
		id: subMenuPanelButtonDecoratorComp

		SubMenuPanelButtonDecorator{

		}
	}

	Component {
		id: filterPanelDecoratorComp

		FilterPanelDecorator{

		}
	}

	// Component {
	//     id: bottomPanelDecoratorComp

	//     BottomPanelDecorator{

	//     }
	// }

	Component {
		id: topButtonDecoratorComp

		TopButtonDecorator{

		}
	}

	Component{
		id: topPanelDecoratorComp;

		TopPanelDecorator{

		}

	}

	Component{
		id: topCenterPanelDecoratorComp;

		TopCenterPanelDecorator{

		}

	}

	Component{
		id: topLeftPanelDecoratorComp;

		TopLeftPanelDecorator{

		}

	}


	Component{
		id: topRightPanelDecoratorComp;

		TopRightPanelDecorator{

		}

	}

	Component{
		id: tabPanelDecoratorComp;

		TabPanelDecorator{

		}

	}

	Component{
		id: buttonDecoratorComp;

		ButtonDecorator{
		}
	}

	Component{
		id: firstSegmentDecoratorComp
		ExternalSegmentDecorator{
		}
	}

	Component{
		id: middleSegmentDecoratorComp
		ButtonDecorator{
			radius: 0;
		}
	}

	Component{
		id: lastSegmentDecoratorComp

		ExternalSegmentDecorator{
			isLast: true
		}
	}

	Component{
		id: toolButtonDecoratorComp;

		ToolButtonDecorator{
		}
	}

	Component{
		id: roundButtonDecoratorComp;

		ButtonDecorator{
			width: Style.controlHeightM;
			height: width;
			radius: width;
		}
	}

	Component{
		id: textFieldDecoratorComp;

		Item{
			id: textFieldItem;

			width: Style.sizeHintXXS;
			height: Style.controlHeightM;
			z: -1;

			property var baseElement;

			property string text: "";


			Rectangle {
				id: mainRect;

				anchors.fill: textFieldItem;

				color: !textFieldItem.baseElement ? "transparent" : textFieldItem.baseElement.readOnly ? Style.alternateBaseColor : Style.baseColor;

				radius: !textFieldItem.baseElement ? 0 : textFieldItem.baseElement.radius;

				border.color: !textFieldItem.baseElement ?
								  "transparent" : textFieldItem.baseElement.borderColorConst !== "" ?
									  textFieldItem.baseElement.borderColorConst: textFieldItem.baseElement.activeFocus ?
										  textFieldItem.baseElement.borderColor : Style.hover;

				border.width: 1;
			}

			Text {
				id: placeHolder;

				anchors.left: textFieldItem.left;
				anchors.leftMargin: !textFieldItem.baseElement ? 0 : textFieldItem.baseElement.margin;
				anchors.verticalCenter: textFieldItem.verticalCenter;


				font.pixelSize: Style.fontSizeM;
				font.family: Style.fontFamily;

				//TODO -> Style
				color: !textFieldItem.baseElement ? "transparent" : textFieldItem.baseElement.placeHolderFontColor;
				opacity: !textFieldItem.baseElement ? 0 : textFieldItem.baseElement.placeHolderOpacity;

				visible: !textFieldItem.baseElement ? false : textFieldItem.baseElement.text == "";

				text: !textFieldItem.baseElement ? "" : textFieldItem.baseElement.placeHolderText;
			}

			MouseArea {
				id: mouseArea;

				anchors.fill: textFieldItem;
				cursorShape: !textFieldItem.baseElement ? Qt.ArrowCursor : textFieldItem.baseElement.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;

				visible: !textFieldItem.baseElement ? false : !textFieldItem.baseElement.readOnly;

				acceptedButtons: Qt.RightButton;

				onClicked: {
					let point = mapToItem(null, mouse.x, mouse.y);
					if(textFieldItem.baseElement){
						textFieldItem.baseElement.openInputAction(point.x, point.y)
					}
				}
			}
		}
	}

	Component{
		id: textEditDecoratorComp;

		Item{
			id: textAreaItem;

			width: Style.sizeHintXXS;
			height: Style.controlHeightM;
			z: -1;

			property var baseElement;
			property string text: "";

			function setHeight(){
				//console.log("setHeight");
			}

			Rectangle {
				id: mainRect;

				anchors.fill: textAreaItem;

				color: !textAreaItem.baseElement ? "transparent" : textAreaItem.baseElement.readOnly ? Style.alternateBaseColor : Style.baseColor;

				radius: !textAreaItem.baseElement ? 0 : textAreaItem.baseElement.radius;

				border.color: !textAreaItem.baseElement ? "transparent" : textAreaItem.baseElement.borderColorConst !== "" ? textAreaItem.baseElement.borderColorConst: textAreaItem.baseElement.activeFocus ? textAreaItem.baseElement.borderColor : Style.hover;

				border.width: 1;
			}

			Text {
				id: placeHolder;

				anchors.left: textAreaItem.left;
				anchors.leftMargin: !textAreaItem.baseElement ? 0 : textAreaItem.baseElement.margin;
				anchors.verticalCenter: textAreaItem.verticalCenter;


				font.pixelSize: Style.fontSizeM;
				font.family: Style.fontFamily;

				//TODO -> Style
				color: !textAreaItem.baseElement ? "transparent" : textAreaItem.baseElement.placeHolderFontColor;
				opacity: !textAreaItem.baseElement ? 0 : textAreaItem.baseElement.placeHolderOpacity;

				visible: !textAreaItem.baseElement ? false : textAreaItem.baseElement.text == "";

				text: !textAreaItem.baseElement ? "" : textAreaItem.baseElement.placeHolderText;
			}

			MouseArea {
				id: mouseArea;

				anchors.fill: textAreaItem;
				cursorShape: !textAreaItem.baseElement ? Qt.ArrowCursor : textAreaItem.baseElement.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;

				visible: !textAreaItem.baseElement ? false : !textAreaItem.baseElement.readOnly;

				acceptedButtons: Qt.RightButton;

				onClicked: {
					let point = mapToItem(null, mouse.x, mouse.y);
					if(textAreaItem.baseElement){
						textAreaItem.baseElement.openInputAction(point.x, point.y)
					}
				}
			}
		}
	}

	Component{
		id: comboBoxDecoratorComp;

		Rectangle {
			id: cbMainRect;

			width: Style.sizeHintXXS;
			height: Style.controlHeightM;

			property var baseElement;
			property var model;
			property int currentIndex: -1;

			property alias textVisible: cbTitleTxt.visible;
			property alias iconVisible: cbArrowIcon.visible;
			property alias icon: cbArrowIcon;

			property bool isCropped: textHelper.width > cbTitleTxt.width;

			border.color: !baseElement ? "transparent" : baseElement.borderColor;
			border.width: 1;

			radius: !baseElement ? 0 : baseElement.radius;
			color: !baseElement ? "transparent" : baseElement.backgroundColor;

			Text {
				id: cbTitleTxt;

				anchors.verticalCenter: cbMainRect.verticalCenter;
				anchors.left: cbMainRect.left;
				anchors.right: cbArrowIcon.left;
				anchors.rightMargin: Style.marginM;

				horizontalAlignment: !cbMainRect.baseElement ? Text.AlignLeft : cbMainRect.baseElement.textCentered ? Text.AlignHCenter : Text.AlignLeft;

				color: !cbMainRect.baseElement ? "transparent" :
						cbMainRect.baseElement.currentText !== "" ? cbMainRect.baseElement.fontColorTitle : Style.placeHolderTextColor;
				text: !cbMainRect.baseElement ? "" : cbMainRect.baseElement.currentText !== "" ? cbMainRect.baseElement.currentText : cbMainRect.baseElement.placeHolderText;
				font.family: Style.fontFamily;
				font.pixelSize: !cbMainRect.baseElement ? Style.fontSizeM : cbMainRect.baseElement.textSize;

				elide: Text.ElideRight;
			}

			Binding {
				target: cbTitleTxt.anchors
				property: "leftMargin"
				value:  !cbMainRect.baseElement ? 0 :
						cbMainRect.baseElement.textCentered ? cbMainRect.width/2 - cbTitleTxt.width/2 :
						Style.marginM
				delayed: true
			}

			Text {
				id: textHelper;

				text: cbTitleTxt.text;
				font.family: cbTitleTxt.font.family;
				font.pixelSize: cbTitleTxt.font.pixelSize;

				visible: false;
			}

			Image {
				id: cbArrowIcon;

				anchors.right: cbMainRect.right;
				anchors.verticalCenter: cbMainRect.verticalCenter;
				anchors.rightMargin: Style.marginXS;

				width: Style.iconSizeXS;
				height: Style.iconSizeXXS;

				rotation: !cbMainRect.baseElement ? 0 : cbMainRect.baseElement.isOpen ? 180 : 0
				source: cbMainRect.baseElement.changeable ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
														  : "../../../" + Style.getIconPath("Icons/Down", Icon.State.Off, Icon.Mode.Disabled);

				sourceSize.width: width;
				sourceSize.height: height;

				visible: !cbMainRect.baseElement ? false : cbMainRect.baseElement.visibleIcon;
			}

			TooltipArea {
				id: tooltipArea;

				mouseArea: !cbMainRect.baseElement ? null : cbMainRect.baseElement.mouseArea;
				text: (!cbMainRect.baseElement || !cbMainRect.isCropped) ? "" : cbTitleTxt.text;
			}
		}
	}

	Component {
		id: checkBoxDecoratorComp;

		Item {
			id: checkBoxItem;

			width: checkBoxText.text == "" ? checkRect.width : checkRect.width + checkBoxText.width + checkBoxItem.mainMargin;
			height: !baseElement ? 0 : baseElement.height

			property var baseElement: null;

			property int mainMargin: !checkBoxItem.baseElement ? 0 : 8;
			property string text: !baseElement ? "" : baseElement.text == undefined ? "" : baseElement.text;
			property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;
			property int fontPixelSize: !baseElement ? 10 : baseElement.fontPixelSize == undefined ? 10 : baseElement.fontPixelSize;
			property bool fontBold: !baseElement ? false : baseElement.fontBold == undefined ? false : baseElement.fontBold;

			Rectangle {
				id: checkRect;

				anchors.left: checkBoxItem ? checkBoxItem.left : undefined;
				anchors.leftMargin: !checkBoxItem.baseElement ? 0 : !checkBoxItem.baseElement.isLeftText ? 0 : checkBoxText.width + checkBoxItem.baseElement.mainMargin;
				anchors.verticalCenter: checkBoxItem ? checkBoxItem.verticalCenter : undefined;

				width: height;
				height: checkBoxItem.height;

				color: Style.baseColor;

				radius: !checkBoxItem.baseElement ? 0 : checkBoxItem.baseElement.radius;

				border.width: 1;
				border.color: !checkBoxItem.baseElement ? "transparent" : checkBoxItem.borderColor !=="" ? checkBoxItem.borderColor  : checkBoxItem.baseElement.isActive ? Style.borderColor : Style.inactiveTextColor;

				Image {
					id: image;

					anchors.centerIn: checkRect;

					height: checkRect.height - 1;
					width: height;

					sourceSize.width: width;
					sourceSize.height: height;

					visible: !checkBoxItem.baseElement ? false : checkBoxItem.baseElement.checkIndicator == "rectangle" ? false : checkBoxItem.baseElement.checkState != Qt.PartiallyChecked;
					source: !checkBoxItem.baseElement ? "" : checkBoxItem.baseElement.isActive ? checkBoxItem.baseElement.checkState == Qt.Checked ? checkBoxItem.baseElement.imageSourceActive : "" :
					checkBoxItem.baseElement.checkState == Qt.Checked ? checkBoxItem.baseElement.imageSourceNotActive : "";
				}
			}

			Rectangle {
				id: rect;

				anchors.centerIn: checkRect;

				height: checkRect.height - 4;
				width: height;

				radius: checkRect.radius;

				color: Style.textColor;

				visible: !checkBoxItem.baseElement ? false : checkBoxItem.baseElement.checkIndicator == "rectangle" ? checkBoxItem.baseElement.checkState == Qt.Checked : checkBoxItem.baseElement.checkState == Qt.PartiallyChecked;
			}

			Text {
				id: checkBoxText;

				anchors.left: checkBoxItem ? checkBoxItem.left : undefined;
				anchors.leftMargin: !checkBoxItem.baseElement ? 0 : checkBoxItem.baseElement.isLeftText ? 0 : checkRect.width + checkBoxItem.baseElement.mainMargin;
				anchors.verticalCenter: checkBoxItem ? checkBoxItem.verticalCenter : undefined;

				color: !checkBoxItem.baseElement ? "transparent" :  checkBoxItem.baseElement.enabled ? Style.buttonTextColor : Style.buttonInactiveTextColor;

				font.pixelSize: checkBoxItem.fontPixelSize;
				font.family: Style.fontFamily;
				font.bold: checkBoxItem.fontBold;

				text: checkBoxItem.text;
			}

		}

	}

	Component{
		id: switchDecoratorComp;

		Item{
			id: switchItem;

			width: switchText.text == "" ? backgroundRec.width : backgroundRec.width + switchText.width + switchItem.mainMargin;
			height: Style.controlHeightS;

			property var baseElement: null;
			property bool checked: !baseElement ? false : baseElement.checked;
			property bool readOnly: !baseElement ? false : baseElement.readOnly;

			property int mainMargin: !baseElement ? 0 : Style.marginS;
			property string text: !baseElement ? "" : baseElement.text == undefined ? "" : baseElement.text;
			property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;
			property int fontPixelSize: !baseElement ? Style.fontSizeXS : baseElement.fontPixelSize == undefined ? 10 : baseElement.fontPixelSize;
			property bool fontBold: !baseElement ? false : baseElement.fontBold == undefined ? false : baseElement.fontBold;

			property real backgroundWidth: !baseElement ? 0 : baseElement.backgroundWidth;

			Rectangle{
				id: backgroundRec;

				anchors.left: switchItem.left;
				anchors.leftMargin: !switchItem.baseElement ? 0 : !switchItem.baseElement.isLeftText ? 0 : switchText.width  + switchItem.baseElement.mainMargin;
				anchors.verticalCenter: switchItem.verticalCenter;

				width: switchItem.backgroundWidth;
				height: !switchItem.baseElement ? 0 : switchItem.baseElement.backgroundHeight;
				color: !switchItem.baseElement ? "transparent" : switchItem.checked ? switchItem.baseElement.backgroundColorChecked :
																					  switchItem.baseElement.backgroundColor;
				radius: height;

				opacity: !switchItem.baseElement ? 0 : switchItem.baseElement.backgroundOpacity;

			}

			Rectangle{
				id: controlRec;

				anchors.verticalCenter: switchItem.verticalCenter;

				x: !switchItem.baseElement ? 0 : backgroundRec.x + switchItem.baseElement.controlRecX;
				width: switchItem.height;
				height: width;
				radius: width;
				color: switchItem.readOnly ? switchItem.baseElement.controlColor : !switchItem.baseElement ? "transparent" : switchItem.checked ? switchItem.baseElement.controlColorChecked :
																					  switchItem.baseElement.controlColor;
			}

			Text {
				id: switchText;

				anchors.left: switchItem.left;
				anchors.leftMargin: !switchItem.baseElement ? 0 : switchItem.baseElement.isLeftText ? 0 : backgroundRec.width + switchItem.baseElement.mainMargin;
				anchors.verticalCenter: switchItem.verticalCenter;

				color: !switchItem.baseElement ? "transparent" :  switchItem.baseElement.enabled ? Style.buttonTextColor : Style.buttonInactiveTextColor;

				font.pixelSize: switchItem.fontPixelSize;
				font.family: Style.fontFamily;
				font.bold: switchItem.fontBold;

				text: switchItem.text;
			}
		}

	}

	Component{
		id: sliderDecoratorComp;

		Item{
			id: sliderItem;

			width: !baseElement ?  0 :baseElement.width;
			height: !baseElement ?  0 :baseElement.height;

			property var baseElement: null;
			property real value: 0;

			property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;
			property int controlWidth: !sliderItem.baseElement ? 0 : sliderItem.baseElement.controlWidth;

			property int majorTickInterval: !sliderItem.baseElement ? 0 : sliderItem.baseElement.majorTickInterval
			property int minorTickInterval: !sliderItem.baseElement ? 0 : sliderItem.baseElement.minorTickInterval

			Item{
				id: rotationContainer;

				anchors.centerIn: parent

				transformOrigin: Item.Center
				rotation: !sliderItem.baseElement ? 0 : sliderItem.baseElement.orientation == Qt.Vertical ? -90 : 0;

				width: !sliderItem.baseElement ? parent.width : sliderItem.baseElement.orientation == Qt.Vertical ? parent.height : parent.width;
				height:  !sliderItem.baseElement ? parent.height : sliderItem.baseElement.orientation == Qt.Vertical ? parent.width : parent.height;

				Item{
					id: sliderContainer;

					anchors.horizontalCenter: parent.horizontalCenter;
					anchors.verticalCenter: parent.verticalCenter;

					width: parent.width - sliderItem.controlWidth;
					height: parent.height;

					Row {
						id: majorTickRow;

						y: !sliderItem.baseElement ? 0 : sliderItem.baseElement.controlCenterY + correction;
						property real correction: !sliderItem.baseElement ? 0 : sliderItem.baseElement.ticksPosition == RelativePosition.verticalCenter ? - height/2 :
																																			   sliderItem.baseElement.ticksPosition == RelativePosition.top ? - height :
																																																		  sliderItem.baseElement.ticksPosition == RelativePosition.bottom ? 0: 0

						height: !sliderItem.baseElement ? 0 : sliderItem.baseElement.majorTickHeight;

						spacing: 0;

						visible: !sliderItem.baseElement ? false : sliderItem.baseElement.hasTicks

						property real delegateWidth: 2;

						Repeater{
							id: majorTickRepeater;

							model: !sliderItem.baseElement || sliderItem.majorTickInterval <=0 ? 0 : Math.trunc((sliderItem.baseElement.to - sliderItem.baseElement.from)/sliderItem.majorTickInterval)

							delegate:
								Item{
								width: majorTickRepeater.model == 0 ? 0 : sliderContainer.width / majorTickRepeater.model;
								height: majorTickRow.height;

								Rectangle{
									anchors.left: parent.left;
									anchors.leftMargin: -width/2
									anchors.bottom: parent.bottom;
									height: majorTickRow.height //- tickTextLeft.height;
									width: majorTickRow.delegateWidth;
									color: Style.borderColor2;

									BaseText{
										id: tickTextLeft;

										anchors.top: !sliderItem.baseElement ? undefined : sliderItem.baseElement.ticksPosition == RelativePosition.bottom ? parent.bottom : parent.top;

										anchors.topMargin: !sliderItem.baseElement ? 0 : (sliderItem.baseElement.ticksPosition == RelativePosition.top || sliderItem.baseElement.ticksPosition == RelativePosition.verticalCenter)? -height - 2: 0

										anchors.horizontalCenter: parent.horizontalCenter;

										color: parent.color;
										font.pixelSize: !sliderItem.baseElement ? Style.fontSizeM : sliderItem.baseElement.fontSize

										text: Math.trunc(sliderItem.baseElement.from + model.index * (sliderItem.baseElement.to - sliderItem.baseElement.from)/majorTickRepeater.count);
									}
								}
								Rectangle{
									anchors.right: parent.right;
									anchors.rightMargin: -width/2
									anchors.bottom: parent.bottom;
									height: majorTickRow.height //- tickTextRight.height;
									width: majorTickRow.delegateWidth;
									color: Style.borderColor2;
									visible: model.index == majorTickRepeater.count -1;

									BaseText{
										id: tickTextRight;

										anchors.top: !sliderItem.baseElement ? undefined : sliderItem.baseElement.ticksPosition == RelativePosition.bottom ? parent.bottom : parent.top;

										anchors.topMargin: !sliderItem.baseElement ? 0 : (sliderItem.baseElement.ticksPosition == RelativePosition.top || sliderItem.baseElement.ticksPosition == RelativePosition.verticalCenter)? -height  - 2 : 0
										anchors.horizontalCenter: parent.horizontalCenter;

										font.pixelSize: !sliderItem.baseElement ? Style.fontSizeM : sliderItem.baseElement.fontSize

										color: parent.color;
										text: Math.trunc(sliderItem.baseElement.to)
									}

								}
							}
						}
					}
					Row {
						id: minorTickRow;

						anchors.bottom: !sliderItem.baseElement ? undefined : sliderItem.baseElement.ticksPosition == RelativePosition.top ? majorTickRow.bottom : undefined
						anchors.top: !sliderItem.baseElement ? undefined : sliderItem.baseElement.ticksPosition == RelativePosition.bottom ? majorTickRow.top : undefined
						anchors.verticalCenter: !sliderItem.baseElement ? undefined : sliderItem.baseElement.ticksPosition == RelativePosition.verticalCenter ? majorTickRow.verticalCenter : undefined

						height: !sliderItem.baseElement ? 0 : sliderItem.baseElement.minorTickHeight;

						spacing: 0;

						visible: !sliderItem.baseElement ? false : sliderItem.baseElement.hasTicks

						property real delegateWidth: 1;
						Repeater{
							id: minorTickRepeater;

							model: !sliderItem.baseElement || sliderItem.minorTickInterval <=0 ? 0 : Math.trunc((sliderItem.baseElement.to - sliderItem.baseElement.from)/sliderItem.minorTickInterval)

							delegate:
								Item{
								width: minorTickRepeater.model == 0 ? 0 : sliderContainer.width / minorTickRepeater.model;
								height: minorTickRow.height;

								Rectangle{
									anchors.left: parent.left;
									anchors.leftMargin: -width/2
									height: minorTickRow.height;
									width: minorTickRow.delegateWidth;
									color: Style.borderColor2;

								}
								Rectangle{
									anchors.right: parent.right;
									anchors.rightMargin: -width/2
									height: minorTickRow.height;
									width: minorTickRow.delegateWidth;
									color: Style.borderColor2;
									visible: model.index == minorTickRepeater.count -1;
								}
							}
						}
					}

					Rectangle{
						id: backgroundRec;

						anchors.left: sliderContainer.left;
						y: !sliderItem.baseElement ? 0 :sliderItem.baseElement.controlCenterY - height/2
						//anchors.verticalCenter: sliderContainer.verticalCenter;

						width: sliderContainer.width;
						height: !sliderItem.baseElement ? 0 : sliderItem.baseElement.backgroundHeight;
						color: !sliderItem.baseElement ? "transparent" : sliderItem.baseElement.backgroundColor;

						opacity: !sliderItem.baseElement ? 0 : sliderItem.baseElement.backgroundOpacity;

						Rectangle{
							width: !sliderItem.baseElement ? 0 : sliderItem.baseElement.controlRecX;
							height: backgroundRec.height;
							radius: backgroundRec.radius;
							color: !sliderItem.baseElement ? "transparent" : sliderItem.baseElement.controlColor;
						}

					}


					Rectangle{
						id: indicator;

						anchors.horizontalCenter: controlRec.horizontalCenter;

						anchors.bottom: controlRec.bottom;
						anchors.bottomMargin: !sliderItem.baseElement ? 0 : sliderItem.baseElement.indicatorPosition == RelativePosition.bottom ? -height - 2 + controlRec.height : 0;

						height: !sliderItem.baseElement ? 0 : sliderItem.baseElement.indicatorHeight;
						width: 2;
						color: Style.textColor//Style.borderColor;
						visible: !sliderItem.baseElement ? false : sliderItem.baseElement.hasIndicator;
						Rectangle{
							anchors.left: parent.right;
							anchors.leftMargin:  isLeft ? - width : 0;

							anchors.top:  parent.top;
							anchors.topMargin: !sliderItem.baseElement ? 0 : sliderItem.baseElement.indicatorPosition == RelativePosition.bottom ? parent.height - height : 0;

							height: parent.width;
							width: indicatorText.width//Style.controlHeightM;
							color: Style.textColor//Style.borderColor;
							property bool isLeft: !sliderItem.baseElement ? false : sliderItem.baseElement.value > sliderItem.baseElement.to/2;

							BaseText{
								id: indicatorText

								anchors.horizontalCenter: parent.horizontalCenter;
								anchors.bottom: parent.bottom;
								anchors.bottomMargin: !sliderItem.baseElement ? 0 : sliderItem.baseElement.indicatorPosition == RelativePosition.bottom ? - height - 2 : 0;

								font.pixelSize: !sliderItem.baseElement ? Style.fontSizeM : sliderItem.baseElement.fontSize

								text: !sliderItem.baseElement ? "" : Math.trunc(sliderItem.baseElement.value)
							}
						}
					}

					Rectangle{
						id: controlRec;

						anchors.verticalCenter: backgroundRec.verticalCenter;

						x: !sliderItem.baseElement ? 0 : sliderItem.baseElement.controlRecX - sliderItem.baseElement.controlWidth/2;
						width: !sliderItem.baseElement ? 0 : sliderItem.baseElement.controlWidth;
						height: width;
						radius: width;
						color: !sliderItem.baseElement ? "transparent" : sliderItem.baseElement.controlColor;

					}
				}
			}

		}
	}

	Component{
		id: rangeSliderDecoratorComp;

		Item{
			id: rangeSliderItem;

			width: !baseElement ?  0 :baseElement.width;
			height: !baseElement ?  0 :baseElement.height;

			property var baseElement: null;
			property real value: 0;

			property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;

			property int controlWidth: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlWidth;

			property int majorTickInterval: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.majorTickInterval
			property int minorTickInterval: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.minorTickInterval

			Item{
				id: rotationContainer;

				anchors.centerIn: parent

				transformOrigin: Item.Center
				rotation: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.orientation == Qt.Vertical ? -90 : 0;

				width: !rangeSliderItem.baseElement ? parent.width : rangeSliderItem.baseElement.orientation == Qt.Vertical ? parent.height : parent.width;
				height:  !rangeSliderItem.baseElement ? parent.height : rangeSliderItem.baseElement.orientation == Qt.Vertical ? parent.width : parent.height;

				Item{
					id: rangeSliderContainer;

					anchors.horizontalCenter: parent.horizontalCenter;

					width: parent.width - rangeSliderItem.controlWidth;
					height: parent.height;

					Row {
						id: majorTickRow;

						y: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlCenterY + correction;
						property real correction: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.ticksPosition == RelativePosition.verticalCenter ? - height/2 :
																																			   rangeSliderItem.baseElement.ticksPosition == RelativePosition.top ? - height :
																																																		  rangeSliderItem.baseElement.ticksPosition == RelativePosition.bottom ? 0: 0

						height: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.majorTickHeight;

						spacing: 0;

						visible: !rangeSliderItem.baseElement ? false : rangeSliderItem.baseElement.hasTicks

						property real delegateWidth: 2;

						Repeater{
							id: majorTickRepeater;

							model: !rangeSliderItem.baseElement || rangeSliderItem.majorTickInterval <=0 ? 0 : Math.trunc((rangeSliderItem.baseElement.to - rangeSliderItem.baseElement.from)/rangeSliderItem.majorTickInterval)

							delegate:
								Item{
								width: majorTickRepeater.model == 0 ? 0 : rangeSliderContainer.width / majorTickRepeater.model;
								height: majorTickRow.height;
								Rectangle{
									anchors.left: parent.left;
									anchors.leftMargin: -width/2

									height: majorTickRow.height;
									width: majorTickRow.delegateWidth;
									color: Style.borderColor2;
									BaseText{
										anchors.top: !rangeSliderItem.baseElement ? undefined : rangeSliderItem.baseElement.ticksPosition == RelativePosition.bottom ? parent.bottom : parent.top;

										anchors.topMargin: !rangeSliderItem.baseElement ? 0 : (rangeSliderItem.baseElement.ticksPosition == RelativePosition.top || rangeSliderItem.baseElement.ticksPosition == RelativePosition.verticalCenter)? -height - 2 : 0

										anchors.horizontalCenter: parent.horizontalCenter;

										font.pixelSize: !rangeSliderItem.baseElement ? Style.fontSizeM : rangeSliderItem.baseElement.fontSize

										color: parent.color;
										text: Math.trunc(rangeSliderItem.baseElement.from +  model.index * (rangeSliderItem.baseElement.to - rangeSliderItem.baseElement.from)/majorTickRepeater.count);
									}
								}
								Rectangle{
									anchors.right: parent.right;
									anchors.rightMargin: -width/2

									height: majorTickRow.height;
									width: majorTickRow.delegateWidth;

									color: Style.borderColor2;
									visible: model.index == majorTickRepeater.count -1;
									BaseText{
										anchors.top: !rangeSliderItem.baseElement ? undefined : rangeSliderItem.baseElement.ticksPosition == RelativePosition.bottom ? parent.bottom : parent.top;

										anchors.topMargin: !rangeSliderItem.baseElement ? 0 : (rangeSliderItem.baseElement.ticksPosition == RelativePosition.top || rangeSliderItem.baseElement.ticksPosition == RelativePosition.verticalCenter)? -height - 2 : 0

										anchors.horizontalCenter: parent.horizontalCenter;

										font.pixelSize: !rangeSliderItem.baseElement ? Style.fontSizeM : rangeSliderItem.baseElement.fontSize

										color: parent.color;
										text: Math.trunc(rangeSliderItem.baseElement.to)
									}
								}
							}
						}

					}

					Row {
						id: minorTickRow;


						anchors.bottom: !rangeSliderItem.baseElement ? undefined : rangeSliderItem.baseElement.ticksPosition == RelativePosition.top ? majorTickRow.bottom : undefined
						anchors.top: !rangeSliderItem.baseElement ? undefined : rangeSliderItem.baseElement.ticksPosition == RelativePosition.bottom ? majorTickRow.top : undefined
						anchors.verticalCenter: !rangeSliderItem.baseElement ? undefined : rangeSliderItem.baseElement.ticksPosition == RelativePosition.verticalCenter ? majorTickRow.verticalCenter : undefined

						height: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.minorTickHeight;

						spacing: 0;

						visible: !rangeSliderItem.baseElement ? false : rangeSliderItem.baseElement.hasTicks

						property real delegateWidth: 1;
						Repeater{
							id: minorTickRepeater;

							model: !rangeSliderItem.baseElement || rangeSliderItem.minorTickInterval <=0 ? 0 : Math.trunc((rangeSliderItem.baseElement.to - rangeSliderItem.baseElement.from)/rangeSliderItem.minorTickInterval)

							delegate:
								Item{
								width: minorTickRepeater.model == 0 ? 0 : rangeSliderContainer.width / minorTickRepeater.model;
								height: minorTickRow.height;

								Rectangle{
									anchors.left: parent.left;
									anchors.leftMargin: -width/2
									height: minorTickRow.height;
									width: minorTickRow.delegateWidth;
									color: Style.borderColor2;

								}
								Rectangle{
									anchors.right: parent.right;
									anchors.rightMargin: -width/2
									height: minorTickRow.height;
									width: minorTickRow.delegateWidth;
									color: Style.borderColor2;
									visible: model.index == minorTickRepeater.count -1;
								}
							}
						}
					}

					Rectangle{
						id: backgroundRec;

						anchors.left: rangeSliderContainer.left;
						y: !rangeSliderItem.baseElement ? 0 :rangeSliderItem.baseElement.controlCenterY - height/2
						//anchors.verticalCenter: rangeSliderContainer.verticalCenter;

						width: rangeSliderContainer.width;
						height: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.backgroundHeight;
						color: !rangeSliderItem.baseElement ? "transparent" : rangeSliderItem.baseElement.backgroundColor;

						radius: 0//height;

						opacity: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.backgroundOpacity;

						Rectangle{
							x: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlRecXFirst;
							width: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlRecXSecond - rangeSliderItem.baseElement.controlRecXFirst;
							height: backgroundRec.height;
							radius: backgroundRec.radius;
							color: !rangeSliderItem.baseElement ? "transparent" : rangeSliderItem.baseElement.controlColor;
						}

					}


					Rectangle{
						id: indicatorFirst;

						anchors.horizontalCenter: controlRecFirst.horizontalCenter;
						anchors.bottom: controlRecFirst.bottom;
						anchors.bottomMargin: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.indicatorPosition == RelativePosition.bottom ? -height - 2 + controlRecFirst.height : 0;

						height: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.indicatorHeight;
						width: 2;
						color: Style.textColor//Style.borderColor;
						visible: !rangeSliderItem.baseElement ? false : rangeSliderItem.baseElement.hasIndicator;

						Rectangle{
							anchors.right: parent.left;
							anchors.top:  parent.top;
							anchors.topMargin: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.indicatorPosition == RelativePosition.bottom ? parent.height - height : 0;

							height: parent.width;
							width: indicatorFirstText.width;
							color: Style.textColor//Style.borderColor;
							BaseText{
								id: indicatorFirstText

								anchors.horizontalCenter: parent.horizontalCenter;
								anchors.bottom: parent.bottom;
								anchors.bottomMargin: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.indicatorPosition == RelativePosition.bottom ? - height : 0;

								font.pixelSize: !rangeSliderItem.baseElement ? Style.fontSizeM : rangeSliderItem.baseElement.fontSize

								text: !rangeSliderItem.baseElement ? "" :  Math.trunc(rangeSliderItem.baseElement.valueFirst)
							}
						}
					}

					Rectangle{
						id: controlRecFirst;

						anchors.verticalCenter: backgroundRec.verticalCenter;

						x: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlRecXFirst - rangeSliderItem.baseElement.controlWidth/2;
						width: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlWidth;
						height: width;
						radius: width;
						color: !rangeSliderItem.baseElement ? "transparent" : rangeSliderItem.baseElement.controlColor;
					}

					Rectangle{
						id: indicatorSecond;

						anchors.horizontalCenter: controlRecSecond.horizontalCenter;
						anchors.bottom: controlRecSecond.bottom;
						anchors.bottomMargin: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.indicatorPosition == RelativePosition.bottom ? -height - 2 + controlRecSecond.height : 0;

						height: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.indicatorHeight;
						width: 2;
						color: Style.textColor//Style.borderColor;
						visible: !rangeSliderItem.baseElement ? false : rangeSliderItem.baseElement.hasIndicator;

						Rectangle{
							anchors.left: parent.right;
							anchors.top:  parent.top;
							anchors.topMargin: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.indicatorPosition == RelativePosition.bottom ? parent.height - height : 0;

							height: parent.width;
							width: indicatorSecondText.width;
							color: Style.textColor//Style.borderColor;
							BaseText{
								id: indicatorSecondText

								anchors.horizontalCenter: parent.horizontalCenter;
								anchors.bottom: parent.bottom;
								anchors.bottomMargin: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.indicatorPosition == RelativePosition.bottom ? - height : 0;

								font.pixelSize: !rangeSliderItem.baseElement ? Style.fontSizeM : rangeSliderItem.baseElement.fontSize

								text: !rangeSliderItem.baseElement ? "" : Math.trunc(rangeSliderItem.baseElement.valueSecond)
							}
						}
					}

					Rectangle{
						id: controlRecSecond;

						anchors.verticalCenter: backgroundRec.verticalCenter;

						x: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlRecXSecond  - rangeSliderItem.baseElement.controlWidth/2;
						width: !rangeSliderItem.baseElement ? 0 : rangeSliderItem.baseElement.controlWidth;
						height: width;
						radius: width;
						color: !rangeSliderItem.baseElement ? "transparent" : rangeSliderItem.baseElement.controlColor;

					}

				}
			}


		}

	}

	Component{
		id: radioButtonDecoratorComp;

		Item{
			id: radioButtonItem;

			width: radioButtonItem.text == "" ? radioRect.width :
												radioRect.width + radioButtonText.width + radioButtonText.anchors.leftMargin;
			height: Math.max(checkSize, radioButtonText.height);

			property var baseElement: null;
			property int checkSize: !baseElement ? 0 : baseElement.checkSize;
			property string text: !baseElement ? "" : baseElement.text;

			Rectangle {
				id: radioRect;

				anchors.verticalCenter: radioButtonItem.verticalCenter;

				height: radioButtonItem.checkSize;
				width: height;
				radius: height;

				color: "transparent";

				border.width: 2;
				border.color: !radioButtonItem.baseElement ? "transparent" : radioButtonItem.baseElement.selectedColor;


				Rectangle{
					id: circle

					anchors.centerIn: radioRect;

					height: radioRect.height - 2*radioRect.border.width;
					width: height;
					radius: width;
					border.color: Style.borderColor;
					border.width: 1;
					color: Style.grayColor;

					Rectangle{
						id: selectCircle

						anchors.centerIn: circle;

						height: circle.height - 8;
						width: height;
						radius: width;
						color: !radioButtonItem.baseElement ? "transparent" : radioButtonItem.baseElement.selectedColor;
						visible: !radioButtonItem.baseElement ? false : radioButtonItem.baseElement.checkState == Qt.Checked;
					}
				}

			}

			Text {
				id: radioButtonText;

				anchors.left: radioRect.right;
				anchors.leftMargin: !radioButtonItem.baseElement ? 0 : radioButtonItem.baseElement.mainMargin;
				anchors.verticalCenter: radioButtonItem.verticalCenter;

				color: !radioButtonItem.baseElement ? "transparent" : radioButtonItem.baseElement.enabled ? Style.buttonTextColor : Style.buttonInactiveTextColor;

				font.pixelSize: !radioButtonItem.baseElement ? Style.fontSizeXXS : radioButtonItem.baseElement.fontPixelSize;
				font.family: Style.fontFamily;
				font.bold: !radioButtonItem.baseElement ? false : radioButtonItem.baseElement.fontBold;

				text: radioButtonItem.text;
			}
		}
	}

	Component {
		id: textAreaDecoratorComp;
		Item{
			id: textAreaItem;

			width: Style.sizeHintXXS;
			height: Style.controlHeightM;
			z: -1;

			property var baseElement;
			property string text: "";

			function setHeight(){
			}

			Rectangle {
				id: mainRect;

				anchors.fill: textAreaItem;

				color: !textAreaItem.baseElement ? "transparent" : textAreaItem.baseElement.readOnly ? Style.alternateBaseColor : Style.baseColor;

				radius: !textAreaItem.baseElement ? 0 : textAreaItem.baseElement.radius;

				border.color: !textAreaItem.baseElement ? "transparent" : textAreaItem.baseElement.borderColorConst !== "" ? textAreaItem.baseElement.borderColorConst: textAreaItem.baseElement.activeFocus ? textAreaItem.baseElement.borderColor : Style.hover;

				border.width: 1;
			}

			Text {
				id: placeHolder;

				anchors.left: textAreaItem.left;
				anchors.leftMargin: !textAreaItem.baseElement ? 0 : textAreaItem.baseElement.margin;
				anchors.verticalCenter: textAreaItem.verticalCenter;


				font.pixelSize: Style.fontSizeM;
				font.family: Style.fontFamily;

				//TODO -> Style
				color: !textAreaItem.baseElement ? "transparent" : textAreaItem.baseElement.placeHolderFontColor;
				opacity: !textAreaItem.baseElement ? 0 : textAreaItem.baseElement.placeHolderOpacity;

				visible: !textAreaItem.baseElement ? false : textAreaItem.baseElement.text == "";

				text: !textAreaItem.baseElement ? "" : textAreaItem.baseElement.placeHolderText;
			}

			MouseArea {
				id: mouseArea;

				anchors.fill: textAreaItem;
				cursorShape: !textAreaItem.baseElement ? Qt.ArrowCursor : textAreaItem.baseElement.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;

				visible: !textAreaItem.baseElement ? false : !textAreaItem.baseElement.readOnly;

				acceptedButtons: Qt.RightButton;

				onClicked: {
					let point = mapToItem(null, mouse.x, mouse.y);
					console.log("inputActions");
					if(textAreaItem.baseElement){
						textAreaItem.baseElement.openInputAction(point.x, point.y)
					}
				}
			}
		}
	}


	Component {
		id: spinBoxDecoratorComp;
		SpinBoxComp{}
	}


	Component {
		id: progressBarDecoratorComp;
		Item {
			id: progressBar;

			width: !baseElement ? Style.sizeHintXS : baseElement.width;
			height: progressContainer.y + progressContainer.height;

			clip: true;

			property int barHeight: !baseElement ? Style.controlHeightM : baseElement.barHeight;

			property bool hasText: !baseElement ? true : baseElement.hasText;
			property bool hasTitle: !baseElement ? true : baseElement.hasTitle;

			property string title: !baseElement ? qsTr("Progress:") : baseElement.title;
			property string text: !baseElement ? "" : baseElement.text;

			property int percent: !baseElement ? 0 : baseElement.percent;
			property real from: !baseElement ? 0.0 : baseElement.from;
			property real to: !baseElement ? 1.0 : baseElement.to;
			property real value: !baseElement ? 0.0 : baseElement.value;
			property real position: !baseElement ? 0.0 : baseElement.position;
			property bool indeterminate: !baseElement ? false : baseElement.indeterminate;

			property int barRadius: !baseElement ? Style.radiusM : baseElement.barRadius;
			property string barColor: "lightgreen";
			property bool barIsGradient: !baseElement ? false : baseElement.barIsGradient;
			property Gradient gradient: !baseElement ? null : baseElement.gradient;
			property string borderColor: !baseElement ? Style.backgroundColor : baseElement.borderColor;
			property string backgroundColor: !baseElement ? Style.baseColor : baseElement.backgroundColor;
			property int fontSize: !baseElement ? Style.fontSizeL : baseElement.fontSize;

			property var baseElement: null;

			Component.onCompleted: {
				if(indeterminate){
					animX.start();
				}
				if(barHeight > height){
					barHeight = height;
				}
			}

			onIndeterminateChanged: {
				if(indeterminate){
					animX.start();
				}
				else {
					animX.stop();
					animWidthTo.stop();
					animWidthPause.stop();
					animWidthFrom.stop();
					animX2.stop();
					animWidthTo2.stop();
					animWidthPause2.stop();
					animWidthFrom2.stop();
					barIndeterminate1.x = 0;
					barIndeterminate2.x = 0;
					barIndeterminate1.width = 0;
					barIndeterminate2.width = 0;
				}
			}

			onHeightChanged: {
				if(barHeight > height){
					barHeight = height;
				}
			}

			Text {
				id: mainTitle;

				anchors.top: progressBar.top;

				color: Style.textColor;
				font.family: Style.fontFamily;
				font.pixelSize: progressBar.fontSize;
				visible: !progressBar.hasTitle ? false : !progressBar.title ? false : true;

				text: qsTr(progressBar.title);

			}

			Text {
				id: progressText;

				anchors.top: progressBar.top;
				anchors.topMargin: mainTitle.visible * (mainTitle.height + 10);

				color: Style.textColor;
				font.family: Style.fontFamily;
				font.pixelSize: progressBar.fontSize;
				visible: !progressBar.hasText ? false : !progressBar.text ? false : true;

				text: !progressBar.text ? "" : !progressBar.indeterminate ? qsTr("Completed") + ": " + progressBar.text :
																			qsTr("Completed") + "...";

			}

			Rectangle{
				id: progressContainer;

				anchors.top: progressBar.top;
				anchors.topMargin: mainTitle.visible * mainTitle.height +
								   progressText.visible * (progressText.height + mainTitle.visible * 10)
								   + !onlyBar * 10;
				anchors.left: progressBar.left;
				anchors.right: progressBar.right;

				height: progressBar.barHeight;
				radius: progressBar.barRadius;
				color: progressBar.backgroundColor;
				border.width: 1;
				border.color: progressBar.borderColor;
				property bool onlyBar: !mainTitle.visible && !progressText.visible;

				Rectangle{
					id: bar;

					anchors.left: progressContainer.left;
					anchors.leftMargin: progressContainer.border.width;
					anchors.verticalCenter: progressContainer.verticalCenter;

					height: progressContainer.height - 2* anchors.leftMargin;
					width: progressBar.percent < 0 ? 0 : progressBar.percent > 100 ?
														 (progressContainer.width - 2* anchors.leftMargin) :
														 (progressContainer.width - 2* anchors.leftMargin) * progressBar.percent/100;

					radius: progressContainer.radius;
					color: progressBar.barColor;
					gradient: progressBar.barIsGradient ? progressBar.gradient : null;
					visible: !progressBar.indeterminate;

				}

				Rectangle{
					id: barIndeterminate1;

					anchors.verticalCenter: progressContainer.verticalCenter;

					height: progressContainer.height - 2* progressContainer.border.width;
					width: 0;

					radius: progressContainer.radius;
					color: progressBar.barColor;
					visible: progressBar.indeterminate;
					property real maxWidth: progressContainer.width/5;

				}

				Rectangle{
					id: barIndeterminate2;

					anchors.verticalCenter: progressContainer.verticalCenter;

					height: progressContainer.height - 2* progressContainer.border.width;
					width: 0;

					radius: progressContainer.radius;
					color: progressBar.barColor;
					visible: progressBar.indeterminate;
					property real maxWidth: progressContainer.width/5;

				}

			}

			//animations
			NumberAnimation {
				id: animX;

				target: barIndeterminate1;
				property: "x";
				duration: 1200;
				from: 0;
				to: barIndeterminate1.parent.width;
				onStarted: {
					if(progressBar.indeterminate){
						animWidthTo.start();
					}
				}
			}

			NumberAnimation {
				id: animWidthTo;

				target: barIndeterminate1;
				property: "width";
				duration: animX.duration/4;
				from: barIndeterminate1.maxWidth/2;
				to: barIndeterminate1.maxWidth;
				onFinished: {
					if(progressBar.indeterminate){
						animWidthPause.start();
					}
				}

			}

			PauseAnimation {
				id: animWidthPause;

				duration: animX.duration/2;
				onFinished: {
					if(progressBar.indeterminate){
						animWidthFrom.start();
						animX2.start();
					}
				}
			}

			NumberAnimation {
				id: animWidthFrom;

				target: barIndeterminate1;
				property: "width";
				duration: animX.duration/4;
				from: barIndeterminate1.maxWidth;
				to: 0;

			}

			//for second rectangle
			NumberAnimation {
				id: animX2;

				target: barIndeterminate2;
				property: "x";
				duration: animX.duration/2;
				from: 0;
				to: barIndeterminate2.parent.width;
				onStarted: {
					if(progressBar.indeterminate){
						animWidthTo2.start();
					}
				}
			}

			NumberAnimation {
				id: animWidthTo2;

				target: barIndeterminate2;
				property: "width";
				duration: animX2.duration/3;
				from: barIndeterminate2.maxWidth/2;
				to: barIndeterminate2.maxWidth;
				onFinished: {
					if(progressBar.indeterminate){
						animWidthPause2.start();
					}
				}

			}

			PauseAnimation {
				id: animWidthPause2;

				duration: animX2.duration/3;
				onFinished: {
					if(progressBar.indeterminate){
						animWidthFrom2.start();
					}
				}
			}

			NumberAnimation {
				id: animWidthFrom2;

				target: barIndeterminate2;
				property: "width";
				duration: animX2.duration/3;
				from: barIndeterminate2.maxWidth;
				to: 0;
				onFinished: {
					if(progressBar.indeterminate){
						animX.start();
					}
				}

			}

		}
	}

	Component {
		id: busyIndicatorDecoratorComp;

		Rectangle {
			id: busyIndicatorItem;

			anchors.fill: parent;

			color: Style.backgroundColor;
			visible: !busyIndicatorItem.baseElement ? false : busyIndicatorItem.baseElement.visible;

			property var baseElement: null;

			Item { // square
				id: square;

				anchors.centerIn: busyIndicatorItem
				property double minimum: Math.min(busyIndicatorItem.width, busyIndicatorItem.height);
				width: 50;
				height: width;

				Repeater {
					id: repeater;

					model: 8;

					delegate: Rectangle{
						color: Style.textColor;

						property double b: 0.1;
						property double a: 0.25;

						width: repeater.count === 0 ? square.height : ((b - a) / repeater.count * index + a) * square.height;
						height: width;
						radius: 0.5 * height;

						x: 0.5 * square.width  + 0.5 * (square.width  - width )  * Math.cos(2 * Math.PI / repeater.count * model.index) - 0.5 * width;
						y: 0.5 * square.height - 0.5 * (square.height - height)  * Math.sin(2 * Math.PI / repeater.count * model.index) - 0.5 * height;
					}
				}
			}

			MouseArea {
				anchors.fill: busyIndicatorItem;
			}

			Timer {
				interval: 10;
				running: busyIndicatorItem.visible;
				repeat:  true;

				onTriggered: {
					square.rotation += 2; // degrees
				}
			}
		}

	}

	Component {
		id: tooltipDecoratorComp;
		Item {
			id: tooltipItem;

			property var baseElement: null;
		}
	}

	Component {
		id: dialogDecoratorComp;

		DialogDecorator{}

	}

}
