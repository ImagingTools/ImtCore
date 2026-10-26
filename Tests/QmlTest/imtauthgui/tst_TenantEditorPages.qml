import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0
import imtauthTenantsSdl 1.0

TestCase {
	id: testCase

	name: "TenantEditorPages"
	when: windowShown

	Component {
		id: editorComp

		TenantEditor {
			width: 800
			height: 600
		}
	}

	Component {
		id: emptyDataComp

		TenantData {
		}
	}

	// What GetTenantRepresentation brings for the organization the user created
	Component {
		id: representationComp

		TenantData {
			m_id: "tenant-id"
			m_name: "Organization"
			m_ownerId: "user-id"
			m_creatorId: "user-id"
			m_currentUserId: "user-id"
			m_isActive: true
		}
	}

	Component {
		id: spyComp

		SignalSpy {
			signalName: "pageLoaded"
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

	function openEditor(){
		let model = createTemporaryObject(emptyDataComp, testCase)
		let editor = createTemporaryObject(editorComp, testCase.parent, {"model": model})
		let pageView = findItem(editor, function(item){ return item.updatePages !== undefined && item.pagesModel !== undefined })
		verify(pageView)
		let spy = createTemporaryObject(spyComp, testCase, {"target": pageView})
		wait(100)

		return {"model": model, "pageView": pageView, "spy": spy}
	}

	// Each field of the representation flips some page flags: the shown page was created again for every flip
	function test_representationRebuildsThePagesOnce(){
		let context = openEditor()
		context.spy.clear()

		context.model.copyFrom(createTemporaryObject(representationComp, testCase))
		wait(100)

		compare(context.spy.count, 1, "The shown page is created once for the arrived representation")
		verify(context.pageView.getIndexById("Members") >= 0, "The pages of an existing organization are listed")
	}

	// A re-read after a notification brings the same organization: the page the user works in stays
	function test_sameRepresentationKeepsTheShownPage(){
		let context = openEditor()
		context.model.copyFrom(createTemporaryObject(representationComp, testCase))
		wait(100)
		context.pageView.currentIndex = 1
		wait(100)
		context.spy.clear()

		context.model.copyFrom(createTemporaryObject(representationComp, testCase))
		// a field passes through empty while it is copied
		context.model.m_currentUserId = ""
		context.model.m_currentUserId = "user-id"
		wait(100)

		compare(context.pageView.currentIndex, 1)
		compare(context.spy.count, 0, "No page is created again")
	}
}