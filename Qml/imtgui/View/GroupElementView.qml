import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0

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

	// Children handed in later - by a Loader, a Repeater, or a page building
	// itself in stages - were never noticed at all, so a group that filled up
	// after it had already measured itself stayed the size it measured.
	onChildrenChanged: scheduleRecalc()

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

	// update() has just re-anchored every child to the group's current width, and
	// a child only finds its final height once it has been laid out at that width
	// - wrapped text, a control that has just been given room. Measuring in the
	// same turn reads the heights the children are about to leave behind, which
	// is why the group came up the wrong size and stayed that way until a resize
	// happened to measure it again. The settle pass measures once more after the
	// children have had their turn.
	Timer {
		id: settleTimer
		interval: 0
		repeat: false
		onTriggered: {
			container.calcHeight()
		}
	}

	function relayout(){
		update()
		calcHeight()
		settleTimer.restart()
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
		// Separators, made once and reused; and the children already being
		// watched, so a child is never connected twice.
		property var separators: []
		property var watched: []
	}

	Component.onCompleted: {
		relayout()
		scheduleRecalc()
	}

	Component.onDestruction: {
		if (!internal.watched){
			return
		}
		for (let i = 0; i < internal.watched.length; i++){
			if (internal.watched[i]){
				internal.watched[i].visibleChanged.disconnect(container.scheduleRecalc)
				internal.watched[i].heightChanged.disconnect(container.scheduleRecalc)
				internal.watched[i].widthChanged.disconnect(container.scheduleRecalc)
			}
		}
	}

	Component {
		id: separatorComp
		Rectangle {
			height: 1
			opacity: 0.5
			color: Style.borderColor
			objectName: "Separator"
		}
	}

	function update(){
		if (internal.block) return

		internal.block = true

		// Separators are kept and reused, not thrown away and built again on
		// every pass. destroy() only takes effect later, so the run that followed
		// still found them among the children: it counted their height into the
		// group and scanned them all over again, and a burst of relayouts could
		// leave a stack of them waiting to die.
		for (let i = 0; i < internal.separators.length; i++){
			internal.separators[i].visible = false
		}

		// Every item is watched, whether or not it is showing right now: an item
		// that was hidden on the pass that built this group was never connected,
		// so nothing was listening on the day it appeared. Width counts as well -
		// a child that has been given more room usually answers with a height.
		let visibleItems = []
		for (let i = 0; i < children.length; i++){
			let child = children[i]
			if (child.objectName === "Separator"){
				continue
			}

			if (!internal.watched.includes(child)){
				internal.watched.push(child)
				child.visibleChanged.connect(container.scheduleRecalc)
				child.heightChanged.connect(container.scheduleRecalc)
				child.widthChanged.connect(container.scheduleRecalc)
			}

			if (child.visible){
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

				// One separator per boundary, made on first need and kept.
				while (internal.separators.length < i){
					internal.separators.push(separatorComp.createObject(container))
				}

				let separator = internal.separators[i - 1]
				separator.visible = true

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


