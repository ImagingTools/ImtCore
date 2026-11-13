import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0

TestCase {
	id: testCase

	name: "MultiPageView"
	when: windowShown

	Component {
		id: viewComp

		MultiPageView {
			width: 600
			height: 600
		}
	}

	Component {
		id: pageComp

		Item {
		}
	}

	function findItem(item, check){
		if (check(item)){
			return item
		}

		for (let i = 0; i < item.children.length; ++i){
			let found = findItem(item.children[i], check)
			if (found){
				return found
			}
		}

		return null
	}

	function createView(pageCount){
		// TestCase itself is invisible, the view needs a visible parent
		let view = createTemporaryObject(viewComp, testCase.parent)
		for (let i = 0; i < pageCount; ++i){
			view.addPage("Page" + i, "Page " + i, pageComp, "")
		}
		view.currentIndex = 0
		return view
	}

	function sidebarFlickable(view){
		return findItem(view, function(item){ return item.contentHeight !== undefined && item.interactive !== undefined && item.flickableDirection !== undefined })
	}

	function test_fittingPagesDoNotScroll(){
		let view = createView(4)
		let flickable = sidebarFlickable(view)
		verify(flickable)
		wait(50)
		verify(flickable.contentHeight > 4 * 10, "the pages are laid out")
		verify(flickable.contentHeight <= flickable.height)
		verify(!flickable.interactive)
	}

	function test_aNarrowViewKeepsOnlyTheIcons(){
		let view = createView(3)
		view.addSubPage("Page1", "Sub1", "Sub 1", pageComp)
		view.addSubPage("Page1", "Sub2", "Sub 2", pageComp)
		let flickable = sidebarFlickable(view)
		verify(!view.compact)
		let wideWidth = flickable.width

		view.width = view.panelWidth + 200
		verify(view.compact, "200 px are left for the page")
		verify(flickable.width < wideWidth / 2, "the page list shrinks to the icons: " + flickable.width)

		let sub1 = findItem(view, function(item){ return item.objectName === "Page_Sub1" })
		let sub2 = findItem(view, function(item){ return item.objectName === "Page_Sub2" })
		verify(!sub1.visible, "subpages of a collapsed parent are hidden")

		let parentPage = findItem(view, function(item){ return item.objectName === "Page_Page1" })
		// The column lays the rows out again on the next polish
		waitForRendering(view)
		mouseClick(parentPage)
		compare(view.currentIndex, view.getIndexById("Sub1"), "the parent opens its subpage")
		verify(sub1.visible && sub2.visible, "every subpage stays reachable in the compact mode")

		waitForRendering(view)
		mouseClick(sub2)
		compare(view.currentIndex, view.getIndexById("Sub2"))

		mouseClick(parentPage)
		verify(!sub2.visible)
		verify(parentPage.__isSelected, "the collapsed parent shows the selection of its subpage")

		view.width = 600
		verify(!view.compact)
		compare(flickable.width, wideWidth)
	}

	function test_pagesThatDoNotFitScrollToTheLast(){
		let view = createView(30)
		let flickable = sidebarFlickable(view)
		// The column lays the pages out on the next polish
		tryVerify(function(){ return flickable.contentHeight > flickable.height }, 2000, "30 pages do not fit 600 px")
		verify(flickable.interactive)

		let last = findItem(view, function(item){ return item.objectName === "Page_Page29" })
		verify(last)
		flickable.contentY = flickable.contentHeight - flickable.height
		wait(50)
		let bottom = last.mapToItem(flickable, 0, last.height).y
		verify(bottom <= flickable.height + 1, "the last page is reachable: " + bottom + " of " + flickable.height)

		mouseClick(last)
		compare(view.currentIndex, 29)
	}
}
