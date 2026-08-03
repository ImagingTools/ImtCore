import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0

DecoratorBase {
	id: topCenterPanelDecorator;
	
	property bool canSearch: false;
	visible: canSearch;
	
	Component.onCompleted: {
		Events.subscribeEvent("SearchVisible", topCenterPanelDecorator.setVisible);
	}
	
	Component.onDestruction: {
		Events.unSubscribeEvent("SearchVisible", topCenterPanelDecorator.setVisible);
	}
	
	property int contentWidth: Style.sizeHintS;

	Connections {
		target: AuthorizationController;
		
		function onLoggedIn(){
			topCenterPanelDecorator.canSearch = true;
			topCenterPanelDecorator.visible = topCenterPanelDecorator.canSearch;
		}
		
		function onLoggedOut(){
			topCenterPanelDecorator.canSearch = false;
			topCenterPanelDecorator.visible = topCenterPanelDecorator.canSearch;
			tfc.text = "";
		}
	}
	
	function setVisible(visible){
		if (visible && !canSearch){
			topCenterPanelDecorator.visible = false;
			return;
		}
		
		topCenterPanelDecorator.visible = visible;
	}
	
	Row {
		id: contentRow;

		anchors.verticalCenter: topCenterPanelDecorator.verticalCenter;
		anchors.horizontalCenter: topCenterPanelDecorator.horizontalCenter;

		height: Style.controlHeightM;

		spacing: Style.marginM;

		// Below this there is no search box worth showing, only a sliver of one
		// clipped by the panel. Better to stand down and leave the room to the
		// title and the account controls.
		visible: topCenterPanelDecorator.width >= buttonItem.width + Style.sizeHintXXXS + 2 * Style.marginM;
		
		Connections {
			target: NavigationController;

			function onCurrentIndexChanged(index){
				leftButton.enabled = NavigationController.hasPrev();
				rightButton.enabled = NavigationController.hasNext();
			}
		}

		// Both arrows start out dead until the first navigation reports in; asked
		// once on creation they tell the truth from the moment they are drawn.
		Component.onCompleted: {
			leftButton.enabled = NavigationController.hasPrev();
			rightButton.enabled = NavigationController.hasNext();
		}

		Item {
			id: buttonItem
			height: parent.height;
			width: leftButton.width + rightButton.width;
			
			ToolButton {
				id: leftButton;
				anchors.left: parent.left;
				width: Style.buttonWidthM;
				height: parent.height;
				iconSource: enabled ? "../../../" +  Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
									: "../../../" +  Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled);
				enabled: false;
				// Round, like the collapse control on the rail: the square hover
				// blocks were the last flat-cornered thing left up here.
				decorator: Component {
					ToolButtonDecorator {
						icon.width: Style.iconSizeS;
						radius: height / 2;
					}
				}
				onClicked: {
					NavigationController.prev();
				}
			}
			
			ToolButton {
				id: rightButton;
				anchors.left: leftButton.right;
				width: Style.buttonWidthM;
				height: parent.height;
				iconSource: enabled ? "../../../" +  Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
									: "../../../" +  Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled);
				enabled: false;
				// Round, like the collapse control on the rail: the square hover
				// blocks were the last flat-cornered thing left up here.
				decorator: Component {
					ToolButtonDecorator {
						icon.width: Style.iconSizeS;
						radius: height / 2;
					}
				}
				onClicked: {
					NavigationController.next();
				}
			}
		}
		
		// Bound rather than assigned from onWidthChanged: the field kept whatever
		// width it was given the last time the panel resized, so a change to
		// contentWidth alone never reached it.
		SearchTextInput {
			id: tfc;
			height: parent.height;
			width: Math.max(Style.sizeHintXXXS,
							Math.min(topCenterPanelDecorator.contentWidth, topCenterPanelDecorator.width)
								- buttonItem.width - Style.marginM);
			radius: Style.radiusM;

			onSearchChanged: {
				Events.sendEvent("GlobalSearchActivated", tfc.text)
			}
		}
	}
}

