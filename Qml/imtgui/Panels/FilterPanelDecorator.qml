import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

DecoratorBase {
	id: filterPanelDecorator;
	objectName: "FilterPanel"
	height: visible ? Style.buttonWidthL : 0

	property alias contentWidth: content.width;
	property int activeFilterCount: 0;
	property int hiddenFilterCount: 0;
	property int hiddenActiveCount: 0;
	readonly property int overflowReserve: Style.controlHeightM + Style.marginXL;
	
	property CollectionFilter complexFilter: baseElement ? baseElement.complexFilter : null;
	property DocCollectionFilter documentFilter: baseElement ? baseElement.documentFilter : null;
	property bool canResetFilters: baseElement ? baseElement.canResetFilters : false
	property bool hasDateFilter: true
	property bool hasDocumentFilter: true

	property alias searchTextInputWidth: tfc.width;
	property alias clearAllButtonWidth: clearAllButton.width;


	onBaseElementChanged: {
		if (baseElement){
			if (hasDateFilter){
				baseElement.registerFieldFilterDelegate("DateFilter", timeDelegateFilterComp)
			}
			else {
				baseElement.removeFieldFilterDelegate("DateFilter")
			}
		}
	}
	
	onDocumentFilterChanged: {
		if (!baseElement){
			return
		}

		if (documentFilter && hasDocumentFilter){
			baseElement.registerFieldFilterDelegate("DocumentState", stateDelegateFilterComp)
		}
		else{
			baseElement.removeFieldFilterDelegate("DocumentState")
		}
	}

	LocalizationEvent {
		onLocalizationChanged: {
		}
	}

	Connections {
		target: filterPanelDecorator && filterPanelDecorator.baseElement ? filterPanelDecorator.baseElement : undefined

		function onFilterChanged(){
			clearAllButton.enabled = !filterPanelDecorator.complexFilter.isEmpty() || target.hasActiveFilter()
			filterPanelDecorator.activeFilterCount = target.activeFilterCount()
			let textFilter = filterPanelDecorator.complexFilter.getTextFilter()
			tfc.text = textFilter
		}

		function onClearAllFilters(beQuiet){
			tfc.clear()
		}
	}

	Connections {
		target: filterPanelDecorator.complexFilter
		function onFilterChanged(){
			filterPanelDecorator.baseElement.filterChanged()
		}
	}

	Connections {
		target: filterPanelDecorator.documentFilter
		function onFilterChanged(){
			filterPanelDecorator.baseElement.filterChanged()
		}
	}

	property bool relayoutRunning: false;

	function scheduleRelayout(){
		if (filterPanelDecorator.relayoutRunning){
			return
		}

		relayoutTimer.restart()
	}

	property string layoutSignature: "";

	property var chipWidthCache: ({});

	function updateOverflow(){
		// The width handler calls straight in, so this can be re-entered from a
		// geometry change the apply step itself caused.
		if (filterPanelDecorator.relayoutRunning){
			return
		}

		filterPanelDecorator.relayoutRunning = true

		let loaders = []
		let widths = []
		let signature = "" + Math.round(content.width)
		for (let i = 0; i < chipsRepeater.count; i++){
			let loader = chipsRepeater.itemAt(i)
			if (loader && loader.item){
				loaders.push(loader)

				// On the row: what the chip asks for, and remember it. Folded: what
				// it asked for the last time anyone could ask, because right now it
				// would answer with the width of an unlaid-out label - nothing.
				let key = loader.item.filterId
				let chipWidth = loader.item.implicitWidth

				if (loader.visible && chipWidth > 0){
					filterPanelDecorator.chipWidthCache[key] = chipWidth
				}
				else if (filterPanelDecorator.chipWidthCache[key] !== undefined){
					chipWidth = filterPanelDecorator.chipWidthCache[key]
				}

				widths.push(chipWidth)
				signature += "|" + Math.round(chipWidth) + (loader.item.isActive ? "a" : "")
			}
		}

		// The latch. Whatever else may still nudge this panel into recomputing -
		// a re-measured label, a loader settling, a signal that arrives a turn
		// late - the same inputs are never given a second answer. An oscillation
		// needs the state to be revisited; unchanged inputs never are.
		if (signature === filterPanelDecorator.layoutSignature){
			filterPanelDecorator.relayoutRunning = false

			return
		}

		filterPanelDecorator.layoutSignature = signature

		let spacing = chipsRow.spacing
		let natural = 0
		for (let n = 0; n < widths.length; n++){
			natural += n > 0 ? spacing + widths[n] : widths[n]
		}

		// The slot is held open whether or not the button is in it. Taken away
		// only once something had folded, the strip lost sixty pixels the instant
		// the button appeared - so the last chip was drawn right up to the edge
		// one moment and gone the next. Held open, the chip simply stops fitting
		// and folds, and the button arrives in room that was always its own.
		let available = content.width - filterPanelDecorator.overflowReserve - spacing

		let show = []
		for (let s = 0; s < loaders.length; s++){
			show.push(false)
		}

		// Folded away from the end, in order, and never out of the middle.
		//
		// Keeping the active ones first meant the row could drop a chip from the
		// middle, and everything behind it slid a whole chip to the left. A
		// pixel of width either way changed which chip that was, so on a window
		// being dragged the same two kept trading places. What is switched on is
		// still accounted for: the overflow button carries the accent whenever
		// one of the chips behind it is.
		// A chip already on the row keeps its place at exactly the width it needs;
		// one that has folded away has to earn a margin back before it returns.
		//
		// Without that deadband the decision sits right on its own threshold: the
		// chip is one pixel too wide, so it folds - and a chip that has folded no
		// longer contributes the width that made it fold, so on the next pass it
		// fits again. Whatever nudges the row afterwards - a re-measured label, a
		// width that settles a fraction differently - starts the same coin flip
		// over, and the chip spends the rest of its life going in and out of the
		// overflow. Sixteen pixels of stickiness ends it: the answer can change
		// once, and then it stays changed.
		let deadband = Style.marginL
		let used = 0
		for (let j = 0; j < loaders.length; j++){
			let projected = used > 0 ? used + spacing + widths[j] : widths[j]
			let limit = loaders[j].visible ? available : available - deadband

			if (projected > limit){
				break
			}

			show[j] = true
			used = projected
		}

		let hidden = 0
		let hiddenActive = 0
		for (let k = 0; k < loaders.length; k++){
			let loader = loaders[k]

			if (loader.visible !== show[k]){
				loader.visible = show[k]
			}

			// A folded chip has no place on screen for its popup to come from,
			// so it borrows the overflow button. Reset on the way back out, or a
			// widened window would still open it over there.
			loader.item.popupAnchorItem = show[k] ? null : overflowButton

			if (!show[k]){
				hidden++

				if (loader.item.isActive){
					hiddenActive++
				}

				// A chip folded away under the pointer takes its card with it.
				if (valueHint.hintOwner === loader.item){
					valueHint.hintOwner = null
					hintIn.stop()
					hintOut.restart()
				}
			}
		}

		filterPanelDecorator.hiddenFilterCount = hidden
		filterPanelDecorator.hiddenActiveCount = hiddenActive

		filterPanelDecorator.relayoutRunning = false
	}

	Timer {
		id: relayoutTimer

		interval: 0
		onTriggered: filterPanelDecorator.updateOverflow()
	}

	// A value too long for its chip is cut short, and the reader has no way of
	// asking what the rest of it said. Hovering the chip spells it out in full,
	// in a card that points back at the chip it belongs to.
	function updateValueHint(loader){
		if (!loader || !loader.item){
			return
		}

		if (loader.visible && loader.item.hovered && loader.item.valueIsCropped){
			valueHint.hintOwner = loader.item
			valueHint.hintText = loader.item.name + ": " + loader.item.valueText
			valueHint.anchorX = loader.mapToItem(content, loader.width / 2, 0).x
			hintOut.stop()
			hintIn.restart()
		}
		else if (valueHint.hintOwner === loader.item){
			valueHint.hintOwner = null
			hintIn.stop()
			hintOut.restart()
		}
	}

	Item {
		id: content

		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		anchors.right: mainFilters.left
		anchors.rightMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter

		height: Style.controlHeightM

		// Answered on the spot, not on the next turn of the event loop. Deferred,
		// the row was laid out once with the old answer first: the last chip was
		// still there while the button, pinned to the right, had already moved
		// over it - so the chip jumped before it folded away.
		onWidthChanged: filterPanelDecorator.updateOverflow()

		// And clipped short of the button, so even a frame that has not caught up
		// can only cut a chip off rather than let the two overlap.
		Item {
			id: chipsStrip

			anchors.left: parent.left
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			anchors.rightMargin: filterPanelDecorator.overflowReserve + Style.spacingS

			clip: true

		Row {
			id: chipsRow

			anchors.left: parent.left
			anchors.verticalCenter: parent.verticalCenter

			spacing: Style.spacingS

			Repeater {
				id: chipsRepeater

				model: filterPanelDecorator.baseElement ? filterPanelDecorator.baseElement.filtersModel : 0

				onCountChanged: filterPanelDecorator.scheduleRelayout()

				delegate: Loader {
					id: delegateLoader

					sourceComponent: model.comp

					// No size of its own: it takes the chip's implicit width, and
					// the chip is free to grow when its filter is set.
					onLoaded: {
						item.filterId = model.id

						if (item.collectionFilter !== undefined){
							item.collectionFilter = filterPanelDecorator.complexFilter
						}

						if (item.documentFilter !== undefined){
							item.documentFilter = filterPanelDecorator.documentFilter
						}

						item.filterMenu = filterPanelDecorator.baseElement

						filterPanelDecorator.baseElement.filtersModel.setProperty(model.index, "item", item)

						filterPanelDecorator.scheduleRelayout()
					}

					// Turning a filter on writes its value into the chip, which
					// makes the chip wider and can cost the row its last place.
					Connections {
						target: delegateLoader.item

						function onImplicitWidthChanged(){
							filterPanelDecorator.scheduleRelayout()
						}

						function onHoveredChanged(){
							filterPanelDecorator.updateValueHint(delegateLoader)
						}
					}

					Connections {
						target: filterPanelDecorator
						function onComplexFilterChanged(){
							if (delegateLoader.item){
								if (delegateLoader.item.collectionFilter !== undefined){
									delegateLoader.item.collectionFilter = filterPanelDecorator.complexFilter
								}

								delegateLoader.item.filterMenu = filterPanelDecorator.baseElement
							}
						}

						function onDocumentFilterChanged(){
							if (delegateLoader.item){
								if (delegateLoader.item.documentFilter !== undefined){
									delegateLoader.item.documentFilter = filterPanelDecorator.documentFilter
								}

								delegateLoader.item.filterMenu = filterPanelDecorator.baseElement
							}
						}
					}
				}
			}
		}
		}

		// The folded-away filters, in the same shape as the chips they stand for
		// so the row still reads as one thing. It carries the accent when any of
		// them is switched on - otherwise a filter could be at work with nothing
		// on screen to say so.
		Rectangle {
			id: overflowButton

			// Pinned to the end of the strip rather than trailing the chips: a
			// button that moves as they fold away is one more thing the layout
			// has to agree with itself about.
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter

			height: Style.controlHeightM
			width: filterPanelDecorator.overflowReserve
			radius: height / 2

			visible: filterPanelDecorator.hiddenFilterCount > 0

			color: filterPanelDecorator.hiddenActiveCount === 0 ? (overflowArea.containsMouse ? Style.hover : "transparent")
				: Style.selectedColor
			border.width: 1
			border.color: filterPanelDecorator.hiddenActiveCount > 0 ? Style.titleColor : Style.borderColor

			Row {
				id: overflowRow

				anchors.centerIn: parent

				spacing: Style.spacingXS

				Image {
					anchors.verticalCenter: parent.verticalCenter

					width: Style.iconSizeXS
					height: width
					sourceSize.width: width
					sourceSize.height: height
					source: "qrc:/" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal)
				}

				BaseText {
					anchors.verticalCenter: parent.verticalCenter

					text: "" + filterPanelDecorator.hiddenFilterCount
					font.family: Style.fontFamilyBold
					color: filterPanelDecorator.hiddenActiveCount > 0 ? Style.titleColor : Style.textColor
				}
			}

			MouseArea {
				id: overflowArea

				anchors.fill: parent

				hoverEnabled: true
				cursorShape: Qt.PointingHandCursor

				onClicked: filterPanelDecorator.openOverflow()

				onPositionChanged: overflowTooltip.show(overflowArea.mouseX, overflowArea.mouseY)
				onExited: overflowTooltip.hide()
			}

			CustomTooltip {
				id: overflowTooltip

				text: qsTr("%1 more filters").arg(filterPanelDecorator.hiddenFilterCount)
				fitToTextWidth: true
			}
		}
	}

	Item {
		id: valueHint

		x: Math.max(content.x,
					Math.min(content.x + content.width - valueHint.width,
							 content.x + valueHint.anchorX - valueHint.width / 2))
		y: content.y + content.height + Style.spacingXS
		z: 200

		width: hintBody.width
		height: hintTip.height / 2 + hintBody.height

		visible: valueHint.opacity > 0
		opacity: 0

		property string hintText: ""
		property var hintOwner: null
		property real anchorX: 0

		// Points back at the chip even when the card had to slide along the row
		// to stay inside it.
		Rectangle {
			id: hintTip

			x: Math.max(Style.marginM,
						Math.min(valueHint.width - Style.marginM,
								 content.x + valueHint.anchorX - valueHint.x)) - width / 2
			y: 0

			width: Style.marginM
			height: width
			rotation: 45
			color: hintBody.color
		}

		Rectangle {
			id: hintBody

			y: hintTip.height / 2

			width: hintLabel.width + 2 * Style.marginM
			height: hintLabel.height + 2 * Style.marginS
			radius: Style.radiusM
			color: Style.titleColor

			BaseText {
				id: hintLabel

				anchors.centerIn: parent

				width: Math.min(hintMetrics.width, Style.sizeHintM)
				elide: Text.ElideNone
				text: valueHint.hintText
				color: Style.baseColor
				wrapMode: Text.WordWrap
			}
		}

		Text {
			id: hintMetrics

			visible: false
			elide: Text.ElideNone
			wrapMode: Text.NoWrap
			text: valueHint.hintText
			font.family: hintLabel.font.family
			font.pixelSize: hintLabel.font.pixelSize
		}

		NumberAnimation {
			id: hintIn

			target: valueHint
			property: "opacity"
			to: 1
			duration: 120
			easing.type: Easing.OutQuad
		}

		NumberAnimation {
			id: hintOut

			target: valueHint
			property: "opacity"
			to: 0
			duration: 90
			easing.type: Easing.InQuad
		}
	}

	// One cascading menu instead of a popup that opens another popup: the
	// filters that folded away are its rows, and the choices each of them
	// offers hang off its row as a submenu. Two clicks and a second dialog
	// became one hover.
	//
	// Built on every open - which chips are folded away changes with the width
	// of the window and with every filter switched on - and torn down first, so
	// nothing outlives the state it was built from.
	Menu {
		id: overflowMenu
	}

	Component {
		id: overflowItemComp

		MenuItem {}
	}

	Component {
		id: overflowSubMenuComp

		Menu {}
	}

	function clearOverflowMenu(){
		while (overflowMenu.count > 0){
			let row = overflowMenu.itemAt(0)
			overflowMenu.removeItem(0)
			if (row){
				row.destroy()
			}
		}

		for (let i = 0; i < overflowMenu.submenusList.length; i++){
			if (overflowMenu.submenusList[i]){
				overflowMenu.submenusList[i].destroy()
			}
		}

		overflowMenu.submenusList = []
	}

	function buildOptionsSubMenu(filterItem){
		let subMenu = overflowSubMenuComp.createObject(filterPanelDecorator)
		subMenu.title = filterItem.isActive ? filterItem.name + ": " + filterItem.valueText : filterItem.name

		for (let optionIndex = 0; optionIndex < filterItem.optionCount(); optionIndex++){
			let optionRow = overflowItemComp.createObject(subMenu)
			optionRow.text = filterItem.optionName(optionIndex)
			optionRow.enabled = filterItem.optionEnabled(optionIndex)
			// Ticked rather than merely highlighted: the reader can see what the
			// filter is set to without leaving the menu.
			optionRow.checkable = true
			optionRow.checked = filterItem.optionChecked(optionIndex)
			optionRow.triggered.connect(function(){
				filterItem.selectOption(optionIndex)
			})

			subMenu.addItem(optionRow)
		}

		// The same reach as the cross on the chip it stands for.
		if (filterItem.isActive){
			subMenu.addSeparator()

			let clearRow = overflowItemComp.createObject(subMenu)
			clearRow.text = qsTr("Clear")
			clearRow.triggered.connect(function(){
				filterItem.clearFilter(false)
			})

			subMenu.addItem(clearRow)
		}

		return subMenu
	}

	function openOverflow(){
		filterPanelDecorator.clearOverflowMenu()

		for (let i = 0; i < chipsRepeater.count; i++){
			let loader = chipsRepeater.itemAt(i)
			if (!loader || !loader.item || loader.visible){
				continue
			}

			let filterItem = loader.item
			if (filterItem.optionCount() > 0){
				overflowMenu.addMenu(filterPanelDecorator.buildOptionsSubMenu(filterItem))
			}
			else{
				// Nothing to list inline - a date range is picked in a dialog of
				// its own - so the row opens the filter the long way.
				let row = overflowItemComp.createObject(overflowMenu)
				row.text = filterItem.isActive ? filterItem.name + ": " + filterItem.valueText : filterItem.name
				row.triggered.connect(function(){
					filterItem.openFilter()
				})

				overflowMenu.addItem(row)
			}
		}

		overflowMenu.popup(overflowButton, 0, overflowButton.height + Style.spacingXS)
	}

	Component {
		id: stateDelegateFilterComp
		OptionsFilterDelegate {
			id: stateDelegateFilter
			name: qsTr("Document State")

			property DocCollectionFilter documentFilter: null

			Component.onCompleted: {
				createAndAddOption("Active", qsTr("Active"), "", true)
				createAndAddOption("Disabled", qsTr("Disabled"), "", true)
			}
			
			onOptionSelectionChanged: {
				if (!documentFilter){
					return
				}
				
				documentFilter.clear()
				
				if (optionIndexes.length > 0){
					let optionIndex = optionIndexes[0]
					if (optionIndex === 0){
						documentFilter.addDocumentState(documentFilter.s_activeState)
					}
					else{
						documentFilter.addDocumentState(documentFilter.s_disabledState)
					}
				}
				
				documentFilter.filterChanged()
			}
		}
	}

	Component {
		id: timeDelegateFilterComp

		TimeFilterDelegate {
			objectName: "CreationDateFilter"
			name: qsTr("Creation Date")
			onCleared: {
				if (filterPanelDecorator.complexFilter){
					if (filterPanelDecorator.complexFilter.hasTimeFilter()){
						filterPanelDecorator.complexFilter.clearTimeFilter()
					}
				}
			}

			onAccepted: {
				if (filterPanelDecorator.complexFilter){
					filterPanelDecorator.complexFilter.setTimeFilter(this.timeFilter)
				}
			}
		}
	}

	Row {
		id: mainFilters
		anchors.right: clearAllButton.left
		anchors.rightMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter
		height: Style.controlHeightM
		spacing: Style.marginM

		SearchTextInput {
			id: tfc;

			radius: Style.radiusM;

			onSearchChanged: {
				if (filterPanelDecorator.complexFilter){
					let currentTextFilter = filterPanelDecorator.complexFilter.getTextFilter()
					if (currentTextFilter !== tfc.text){
						filterPanelDecorator.complexFilter.setTextFilter(tfc.text);
						filterPanelDecorator.complexFilter.filterChanged()
					}
				}
			}
		}
	}
	
	ToolButton {
		id: clearAllButton
		objectName: "ClearAllFilters"
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		// The same funnel as the show/hide button beside it, with a cross added.
		// FilterRemove is a different drawing from a different set, so the two
		// buttons never looked like a pair.


		icon.source: enabled ? "qrc:/" + Style.getIconPath("Icons/FilterReset", Icon.State.On, Icon.Mode.Normal)
								 : "qrc:/" + Style.getIconPath("Icons/FilterReset", Icon.State.Off, Icon.Mode.Disabled)
		width: visible ? Style.buttonWidthL : 0
		height: width
		enabled: false
		// Named by the card below, not by the stock tooltip: this button sits
		// against the right edge and a tooltip opening rightwards has nowhere
		// to go.
		visible: filterPanelDecorator.canResetFilters
		onClicked: {
			filterPanelDecorator.baseElement.clearAllFilters(true)
			filterPanelDecorator.baseElement.filterChanged()
		}
		decorator: Component {
			ToolButtonDecorator {
				// Bigger inside a bigger button: the cross on this icon is a
				// fraction of the drawing, and at twenty pixels that fraction
				// came to about three.
				icon.width: Style.iconSizeM + Style.spacingXS
				radius: height / 2
			}
		}

		// How much is being filtered out, without opening anything. The button
		// alone said only that something was on.
		Rectangle {
			anchors.right: parent.right
			anchors.top: parent.top

			// Above the decorator, which is created after this and would
			// otherwise be painted over it.
			z: 1

			width: Style.iconSizeXS + Style.spacingXS
			height: width
			radius: width / 2
			visible: filterPanelDecorator.activeFilterCount > 1
			color: Style.titleColor

			BaseText {
				anchors.fill: parent

				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "" + filterPanelDecorator.activeFilterCount
				font.family: Style.fontFamilyBold
				font.pixelSize: Style.fontSizeXS
				color: Style.baseColor
			}
		}

		// Names the command in the same card the navigation rail uses, opening to
		// its left: the button stands against the right edge of the row.
		Item {
			id: resetHint

			anchors.right: parent.left
			anchors.rightMargin: resetHint.slide
			anchors.verticalCenter: parent.verticalCenter

			z: 200

			width: resetHintBody.width + Style.spacingS
			height: Style.controlHeightM

			visible: resetHint.opacity > 0
			opacity: 0

			property real slide: 0

			Rectangle {
				x: resetHintBody.width - width / 2
				z: -1

				anchors.verticalCenter: parent.verticalCenter

				width: Style.marginM
				height: width
				rotation: 45
				color: resetHintBody.color
			}

			Rectangle {
				id: resetHintBody

				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter

				width: resetHintLabel.width + 2 * Style.marginM
				height: parent.height
				radius: Style.radiusM
				color: Style.titleColor

				BaseText {
					id: resetHintLabel

					anchors.centerIn: parent

					text: filterPanelDecorator.activeFilterCount > 0
						? qsTr("Reset all filters - %1 active").arg(filterPanelDecorator.activeFilterCount)
						: qsTr("Reset all filters")
					font.pixelSize: Style.fontSizeM
					color: Style.baseColor
				}
			}

			ParallelAnimation {
				id: resetHintIn

				NumberAnimation {
					target: resetHint
					property: "opacity"
					to: 1
					duration: 120
					easing.type: Easing.OutQuad
				}

				NumberAnimation {
					target: resetHint
					property: "slide"
					to: Style.spacingXS
					duration: 120
					easing.type: Easing.OutCubic
				}
			}

			NumberAnimation {
				id: resetHintOut

				target: resetHint
				property: "opacity"
				to: 0
				duration: 90
				easing.type: Easing.InQuad
			}

			// Bound, not listened for.
			//
			// Clearing the filters is the last thing this button is allowed to do
			// - it disables itself the moment they are gone - and Button switches
			// its mouse area off with it. A disabled area never reports the
			// pointer leaving, so a card driven by that signal alone was never
			// told to go, and sat there for good. Read as a condition, the card
			// leaves the moment the button stops being usable, whether the
			// pointer moved or not.
			readonly property bool wanted: clearAllButton.enabled && clearAllButton.visible
				&& clearAllButton.mouseArea && clearAllButton.mouseArea.containsMouse

			onWantedChanged: {
				if (resetHint.wanted){
					resetHintOut.stop()
					resetHint.slide = 0
					resetHintIn.restart()
				}
				else{
					resetHintIn.stop()
					resetHintOut.restart()
				}
			}
		}
	}
}

