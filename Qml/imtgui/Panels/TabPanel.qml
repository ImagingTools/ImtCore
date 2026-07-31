import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtqmlutils 1.0

Rectangle {
	id: tabPanelContainer;
	objectName: "TabPanel"
	
	width: list.width + externButtons.width;
	height: visible ? Style.controlHeightL : 0;
	
	color: Style.baseColor;

	property int selectedIndex: 0;
	property int count: 0;
	property alias spacing: list.spacing;
	property bool isCloseEnable: false;
	property int maxWidth: width
	
	property alias model: list.model;
	property alias tabDelegate: list.delegate
	// Expose the horizontal tab ListView so custom tabDelegate instances can set
	// TabDelegate.listView (dividers, lastElement, scroll helpers).
	property alias tabsList: list
	property Component tabDelegateDecorator: Style.tabPanelDecorator;
	
	signal closeItem(int index);
	signal tabClicked(var mouse, var tabItem, int index);
	signal rightClicked();
	signal leftClicked();
	signal startTabContentLoading(string tabId);
	signal stopTabContentLoading(string tabId);
	
	Component.onCompleted: {
		Events.subscribeEvent("AppSizeChanged", appSizeChanged);
	}
	
	Component.onDestruction: {
		Events.unSubscribeEvent("AppSizeChanged", appSizeChanged);
	}
	
	function viewTabInListView(index){
		list.positionViewAtIndex(index, ListView.Contain);
	}
	
	function appSizeChanged(){
		viewTabInListView(selectedIndex);
	}
	
	onSelectedIndexChanged: {
		viewTabInListView(selectedIndex);
	}
	
	ListView {
		id: list;
		
		anchors.left: parent.left;
		width: contentWidth > (tabPanelContainer.maxWidth - externButtons.width) ? tabPanelContainer.maxWidth - externButtons.width : contentWidth
		
		height: parent.height;
		
		clip: true;
		
		orientation: ListView.Horizontal;
		boundsBehavior: Flickable.StopAtBounds;
		interactive: false;
		
		spacing: 0;
		
		onContentWidthChanged: {
			tabPanelContainer.viewTabInListView(tabPanelContainer.selectedIndex);
		}
		
		delegate: TabDelegate {
			height: list.height;

			tabPanel: tabPanelContainer;
			isCloseEnable: tabPanelContainer.isCloseEnable;
			listView: list;
			decorator: tabPanelContainer.tabDelegateDecorator;

			onCloseSignal: {
				tabPanelContainer.closeItem(model.index);
			}
		}
	}
	
	// Compact overflow navigation — flat chevrons that match modern tab chrome
	// (not a solid block that steals half the strip height).
	Item {
		id: externButtons;

		readonly property int navButtonSize: Style.controlHeightM;
		readonly property int navContentWidth: navButtonSize * 2 + Style.spacingXS + Style.marginS;

		anchors.right: parent.right;
		anchors.verticalCenter: parent.verticalCenter;

		width: visible ? navContentWidth : 0;
		height: parent.height;

		// Reserve space when tabs overflow the available strip width.
		visible: list.contentWidth > (tabPanelContainer.maxWidth - navContentWidth);

		// Soft fade so scrolling tabs do not hard-cut under the nav cluster.
		Rectangle {
			anchors.right: parent.left;
			anchors.verticalCenter: parent.verticalCenter;
			width: Style.marginL;
			height: parent.height;
			visible: externButtons.visible;
			// Keep the strip surface; only a light edge cue, not a heavy panel.
			gradient: Gradient {
				orientation: Gradient.Horizontal;
				GradientStop {
					position: 0.0;
					color: "transparent";
				}
				GradientStop {
					position: 1.0;
					color: tabPanelContainer.color;
				}
			}
		}

		// Thin separator between the last visible tab and the chevrons.
		Rectangle {
			anchors.left: parent.left;
			anchors.verticalCenter: parent.verticalCenter;
			width: 1;
			height: parent.height * 0.4;
			color: Style.borderColor;
			opacity: 0.5;
		}

		Row {
			id: navRow;

			anchors.right: parent.right;
			anchors.verticalCenter: parent.verticalCenter;
			anchors.rightMargin: Style.spacingXXS;
			spacing: Style.spacingXXS;

			Button {
				id: leftButton;

				width: externButtons.navButtonSize;
				height: externButtons.navButtonSize;
				enabled: tabPanelContainer.selectedIndex > 0;
				opacity: enabled ? 1.0 : 0.4;
				tooltipText: qsTr("Previous tab");
				iconSource: enabled
						? ("../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal))
						: ("../../../" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled));
				decorator: Component {
					ButtonDecorator {
						color: parent.enabled && parent.hovered ? Style.alternateBaseColor : "transparent";
						radius: Style.radiusM;
						border.width: 0;
						icon.width: Style.iconSizeS;
						icon.height: Style.iconSizeS;
					}
				}

				onClicked: {
					tabPanelContainer.leftClicked();
				}
			}

			Button {
				id: rightButton;

				width: externButtons.navButtonSize;
				height: externButtons.navButtonSize;
				enabled: tabPanelContainer.selectedIndex < list.count - 1;
				opacity: enabled ? 1.0 : 0.4;
				tooltipText: qsTr("Next tab");
				iconSource: enabled
						? ("../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal))
						: ("../../../" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled));
				decorator: Component {
					ButtonDecorator {
						color: parent.enabled && parent.hovered ? Style.alternateBaseColor : "transparent";
						radius: Style.radiusM;
						border.width: 0;
						icon.width: Style.iconSizeS;
						icon.height: Style.iconSizeS;
					}
				}

				onClicked: {
					tabPanelContainer.rightClicked();
				}
			}
		}
	}
}
