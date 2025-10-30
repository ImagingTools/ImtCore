import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

/*!
	\qmltype SwitchCustom
	\inqmlmodule Custom
	\brief A switch control with animation support.
	
	This component represents a switch control with an animated transition when toggled.
	The switch can be in two states: checked and unchecked.
	The appearance and behavior of the switch are customizable, and the state can be toggled by clicking on the switch.
	The animation of the control is optional and can be controlled via the `setChecked` method.
*/
ControlBase {
	id: switchCustom
	objectName: "SwitchButton"
	
	decorator: Style.switchDecorator
	
	/*!
		\qmlproperty string SwitchCustom::text
		\brief The text displayed next to the switch.
		
		This property holds the text that is displayed next to the switch. It can be used for labeling or providing a description next to the switch.
	*/
	property string text: ""
	
	/*!
		\qmlproperty int SwitchCustom::backgroundWidth
		\brief The width of the switch background.
		
		Defines the width of the background area the switch control moves along.
	*/
	property int backgroundWidth: 40
	
	/*!
		\qmlproperty int SwitchCustom::backgroundHeight
		\brief The height of the switch background.
		
		Defines the height of the background area the switch control moves along.
	*/
	property int backgroundHeight: 14
	
	/*!
		\qmlproperty string SwitchCustom::backgroundColor
		\brief The background color of the switch when not checked.
		
		Defines the background color when the switch is in the unchecked state.
	*/
	property string backgroundColor: Style.borderColor
	
	/*!
		\qmlproperty string SwitchCustom::controlColor
		\brief The color of the toggle control when not checked.
		
		Defines the color of the control (toggle button) when the switch is in the unchecked state.
	*/
	property string controlColor: Style.borderColor2
	
	/*!
		\qmlproperty string SwitchCustom::backgroundColorChecked
		\brief The background color of the switch when checked.
		
		Defines the background color when the switch is in the checked state.
	*/
	property string backgroundColorChecked: Style.firstColorHighlight
	
	/*!
		\qmlproperty string SwitchCustom::controlColorChecked
		\brief The color of the toggle control when checked.
		
		Defines the color of the control (toggle button) when the switch is in the checked state.
	*/
	property string controlColorChecked: Style.firstColor
	
	/*!
		\qmlproperty real SwitchCustom::backgroundOpacity
		\brief The opacity of the switch background.
		
		This property defines the opacity level of the background area. A value of 1.0 means fully opaque.
	*/
	property real backgroundOpacity: 1.0
	
	/*!
		\qmlproperty bool SwitchCustom::checked
		\brief The state of the switch.
		
		A boolean value that determines whether the switch is in the checked (true) or unchecked (false) state.
	*/
	property bool checked: false
	
	/*!
		\qmlproperty bool SwitchCustom::isLeftText
		\brief Whether the text is displayed to the left of the switch.
		
		If true, the text will be positioned to the left of the switch. If false, the text will be positioned on the right.
	*/
	property bool isLeftText: false
	
	/*!
		\qmlproperty int SwitchCustom::fontPixelSize
		\brief The font size for the text next to the switch.
		
		Defines the size of the text displayed next to the switch.
	*/
	property int fontPixelSize: Style.fontSizeM
	
	/*!
		\qmlproperty bool SwitchCustom::fontBold
		\brief Whether the text should be bold.
		
		If true, the text next to the switch will be displayed in bold.
	*/
	property bool fontBold: false
	
	/*!
		\qmlproperty int SwitchCustom::mainMargin
		\brief The margin between the switch and the text.
		
		Defines the margin between the switch control and the text that appears next to it.
	*/
	property int mainMargin: 8
	
	/*!
		\qmlproperty bool SwitchCustom::readOnly
		\brief Whether the switch is read-only.
		
		If true, the switch becomes read-only and cannot be toggled by the user.
	*/
	property bool readOnly: false
	
	/*!
		\qmlproperty alias SwitchCustom::tooltipText
		\brief The text for the tooltip displayed when hovering over the switch.
		
		This alias property allows the tooltip's text to be controlled from outside this component.
	*/
	property alias tooltipText: tooltip.text
	
	/*!
		\qmlproperty alias SwitchCustom::tooltipItem
		\brief The tooltip item displayed when hovering over the switch.
		
		This alias property provides access to the tooltip item for customizing its appearance or behavior.
	*/
	property alias tooltipItem: tooltip
	
	/*!
		\qmlreadonly property real SwitchCustom::controlWidth
		\brief The width of the control (toggle button).
		
		This readonly property defines the width of the toggle control, which is based on the height of the switch.
	*/
	readonly property real controlWidth: switchCustom.height > 0 ? switchCustom.height : Style.controlHeightS
	
	/*!
		\qmlproperty real SwitchCustom::controlRecX
		\brief The position of the toggle control.
		
		This property holds the position of the control (toggle button) relative to the background.
	*/
	property real controlRecX: 0
	
	/*!
		\qmlmethod void SwitchCustom::setChecked(bool checkedVal, bool animate)
		\brief Sets the state of the switch.
		
		This method allows you to set the state of the switch (checked or unchecked) and optionally animate the transition.
		
		\param checkedVal The desired state of the switch (true for checked, false for unchecked).
		\param animate If true, the state transition will be animated; otherwise, it will switch immediately.
	*/
	function setChecked(checkedVal, animate){
		if (checkedVal === switchCustom.checked)
			return;
		
		if (animate){
			xAnim.from = switchCustom.controlRecX;
			xAnim.to = checkedVal
					? (switchCustom.backgroundWidth - switchCustom.controlWidth)
					: 0;
			xAnim.start();
		}
		else {
			switchCustom.checked = checkedVal;
		}
	}
	
	onCheckedChanged: {
		if (tooltip.text !== ""){
			tooltip.hide();
		}
		
		switchCustom.controlRecX = checked
				? (switchCustom.backgroundWidth - switchCustom.controlWidth)
				: 0;
	}
	
	MouseArea {
		id: ma
		objectName: "MouseArea"
		anchors.fill: parent
		hoverEnabled: true
		cursorShape: Qt.PointingHandCursor
		visible: !switchCustom.readOnly && parent.enabled
		
		/*!
			\qmlmethod SwitchCustom::onClicked
			\brief Handles the click event on the switch.
			
			This handler toggles the state of the switch when it is clicked. It calls the `setChecked` method with the opposite state of the switch.
		*/
		onClicked: {
			switchCustom.setChecked(!switchCustom.checked, true);
		}
	}
	
	CustomTooltip {
		id: tooltip
		fitToTextWidth: true
	}
	
	/*!
		\qmlanimation SwitchCustom::xAnim
		\brief Animation for toggling the control (toggle button) position.
		
		This animation smoothly transitions the position of the control (toggle button) when the switch state changes.
		
		The transition occurs over a specified duration, providing a visual effect when toggling the switch.
	*/
	NumberAnimation {
		id: xAnim
		target: switchCustom
		property: "controlRecX"
		duration: 100
		
		onFinished: {
			switchCustom.checked = !switchCustom.checked
		}
	}
}
