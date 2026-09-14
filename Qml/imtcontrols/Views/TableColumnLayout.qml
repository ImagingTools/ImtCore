import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

// Column geometry for a table whose header and rows are laid out by hand, as
// TreeExplorerView's headerContentComponent/rowContentComponent pair are.
//
// Widths are kept as shares of the table rather than as pixels, so the table
// stays proportional at any size and a column dragged wider stays wider when
// the window is resized. A column whose breakpoint is above the current width
// folds away entirely and hands its share to the columns that remain.
//
// Nothing here draws: TreeExplorerView renders the drag handles, and the
// header and row components ask widthOf() for their cells. One instance is
// shared by all three, which is what keeps them from drifting apart.
QtObject {
	id: layout

	// Share of the table each column takes. They need not add up to anything in
	// particular - what matters is their ratio, because a folded-away column's
	// share is redistributed among the rest.
	property var fractions: []
	// Table width below which a column is dropped, per column. 0 keeps a column
	// at every size; the first column should normally be 0.
	property var breakpoints: []
	// Floor for dragging, as a share of the table. A column can be made narrow
	// but never squeezed out of existence - that is what the breakpoints are
	// for, and they are not something a drag should be able to trigger.
	property real minFraction: 0.04

	// Captured on the first use so that a double-click on any handle can put the
	// whole table back the way it was designed.
	property var defaultFractions: []

	Component.onCompleted: {
		if (layout.defaultFractions.length === 0)
			layout.defaultFractions = layout.fractions.slice()
	}

	function columnCount() {
		return layout.fractions.length
	}

	function isVisible(index, width) {
		if (index < 0 || index >= layout.breakpoints.length)
			return false
		return width >= layout.breakpoints[index]
	}

	// Sum of the shares actually on screen, which is what a single column's
	// share is measured against.
	function visibleSum(width) {
		let sum = 0
		for (let i = 0; i < layout.fractions.length; ++i) {
			if (layout.isVisible(i, width))
				sum += layout.fractions[i]
		}
		return sum
	}

	function visibleCount(width) {
		let count = 0
		for (let i = 0; i < layout.fractions.length; ++i) {
			if (layout.isVisible(i, width))
				++count
		}
		return count
	}

	// Width in pixels of one column, with the gaps between the visible columns
	// taken off the top so the row adds up to exactly the table width.
	function widthOf(index, width, spacing) {
		if (!layout.isVisible(index, width))
			return 0
		let sum = layout.visibleSum(width)
		if (sum <= 0)
			return 0
		let count = layout.visibleCount(width)
		return (width - (count - 1) * spacing) * layout.fractions[index] / sum
	}

	// Left edge of a column, used to place the drag handles.
	function offsetOf(index, width, spacing) {
		let offset = 0
		for (let i = 0; i < index; ++i) {
			if (!layout.isVisible(i, width))
				continue
			offset += layout.widthOf(i, width, spacing) + spacing
		}
		return offset
	}

	// The visible column that follows the given one, or -1 at the end of the
	// row. Dragging always moves the boundary between a pair of them.
	function nextVisible(index, width) {
		for (let i = index + 1; i < layout.fractions.length; ++i) {
			if (layout.isVisible(i, width))
				return i
		}
		return -1
	}

	// Moves the boundary between a column and the next visible one by deltaPx.
	// The pair keeps its combined share, so everything to the right of the
	// boundary stays exactly where it is - dragging one edge never reflows the
	// whole table, which is what makes the gesture predictable.
	function resize(index, deltaPx, width, spacing) {
		let follower = layout.nextVisible(index, width)
		if (follower < 0 || width <= 0)
			return
		let sum = layout.visibleSum(width)
		let count = layout.visibleCount(width)
		let usable = width - (count - 1) * spacing
		if (usable <= 0 || sum <= 0)
			return
		let delta = deltaPx * sum / usable
		let first = layout.fractions[index]
		let second = layout.fractions[follower]
		// Clamped against both floors before anything is written, so a fast drag
		// past the end stops at the floor instead of overshooting and snapping.
		if (first + delta < layout.minFraction)
			delta = layout.minFraction - first
		if (second - delta < layout.minFraction)
			delta = second - layout.minFraction
		if (delta === 0)
			return
		// Replaced rather than written into: a binding on an array property only
		// re-evaluates when the property itself is assigned.
		let next = layout.fractions.slice()
		next[index] = first + delta
		next[follower] = second - delta
		layout.fractions = next
	}

	function reset() {
		if (layout.defaultFractions.length === 0)
			return
		layout.fractions = layout.defaultFractions.slice()
	}
}
