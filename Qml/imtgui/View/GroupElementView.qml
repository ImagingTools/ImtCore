import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: container

	width: Style.sizeHintXXL

	// Height is driven purely by these bindings - never assign 'height' imperatively,
	// otherwise the binding is broken and the group can stay collapsed (white) until
	// an unrelated relayout (e.g. toggling a Switch) forces a recompute. Collapsing
	// is expressed through 'opened' (see GroupHeaderView) so the expanded height stays
	// reactive to content changes even after a collapse/expand cycle.
	readonly property int expandedHeight: contentHeight > 0 ? contentHeight : Style.sizeHintBXS
	height: opened ? expandedHeight : 0

	property bool opened: true

	border.width: 1
	border.color: Style.borderColor

	radius: Style.marginXS

	color: Style.baseColor

	property int contentHeight: -1

	clip: true // is obligatory !

	// Recomputing after a width change matters because child element heights depend
	// on their width (wrapped text, elided fields).
	onWidthChanged: scheduleRecalc()

	// Coalesce recomputes and run them AFTER the current layout pass, so children
	// have already received their final geometry. This is what makes the initial
	// layout robust regardless of when the hosting Loader/Flickable sizes us.
	// A zero-interval Timer (restart() collapses a burst of changes into one run)
	// is used deliberately instead of Qt.callLater / Qt.binding.
	Timer {
		id: recalcTimer
		interval: 0
		repeat: false
		onTriggered: {
			container.relayout()
		}
	}

	function scheduleRecalc(){
		recalcTimer.restart()
	}

	function relayout(){
		update()
		calcHeight()
	}

	function calcHeight(){
		let h = border.width * 2
		let emptyGroup = true
		for (let i = 0; i < children.length; i++) {
			let child = children[i]
			if (!child.visible) continue
			if (child.height > 0){
				emptyGroup = false
			}
			// Items and separators are stacked flush (no inter-item margins), so the
			// total is simply both borders plus the height of every visible child.
			h += child.height
		}

		if (emptyGroup){
			h = 0
		}

		contentHeight = h
	}

	QtObject {
		id: internal
		property bool block: false
	}

	Component.onCompleted: {
		for (let i = 0; i < children.length; i++){
			children[i].visibleChanged.connect(scheduleRecalc)
			children[i].heightChanged.connect(scheduleRecalc)
		}

		relayout()
		scheduleRecalc()
	}

	Component.onDestruction: {
		for (let i = 0; i < children.length; i++){
			children[i].visibleChanged.disconnect(scheduleRecalc)
			children[i].heightChanged.disconnect(scheduleRecalc)
		}
	}

	Component {
		id: separatorComp
		Rectangle {
			height: 1
			color: Style.borderColor
			objectName: "Separator"
		}
	}

	function update(){
		if (internal.block) return

		internal.block = true

		// Remove old separators
		for (let i = children.length - 1; i >= 0; i--){
			let child = children[i]
			if (child.objectName === "Separator"){
				child.destroy()
			}
		}

		// Collect only visible non-separator items
		let visibleItems = []
		for (let i = 0; i < children.length; i++){
			let child = children[i]
			if (child.visible && child.objectName !== "Separator") {
				visibleItems.push(child)
			}
		}

		// Position items and add separators. Items are stacked flush against each
		// other and inset from the frame by exactly the border width, so the top
		// (and side/bottom) borders are always visible and never covered.
		for (let i = 0; i < visibleItems.length; i++){
			let item = visibleItems[i]

			if (item.border){
				item.border.width = 0
			}

			// Reset anchors
			item.anchors.top = undefined
			item.anchors.bottom = undefined

			item.anchors.left = container.left
			item.anchors.right = container.right
			item.anchors.leftMargin = container.border.width
			item.anchors.rightMargin = container.border.width

			if (i === 0){
				item.anchors.top = container.top
				item.anchors.topMargin = container.border.width
			}
			else {
				item.anchors.topMargin = 0

				// Add separator between items
				let separator = separatorComp.createObject(container)

				separator.anchors.top = visibleItems[i-1].bottom
				separator.anchors.left = container.left
				separator.anchors.right = container.right
				separator.anchors.leftMargin = container.border.width
				separator.anchors.rightMargin = container.border.width

				item.anchors.top = separator.bottom
			}
		}

		internal.block = false
	}
}


