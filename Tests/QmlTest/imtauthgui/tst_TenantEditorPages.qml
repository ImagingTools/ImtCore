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

	// The view hosting the editor (SingleDocumentCollectionView): keeps the sub headers as "id:name"
	Component {
		id: headerHostComp

		Item {
			property var subHeaderNavigator: null
			property var headers: []

			function addDocumentSubHeader(headerId, name){
				let list = headers.slice()
				list.push(headerId + ":" + name)
				headers = list
			}

			function removeDocumentSubHeader(headerId){
				let list = []
				for (let i = 0; i < headers.length; i++){
					if (headers[i].indexOf(headerId + ":") === 0)
						break
					list.push(headers[i])
				}
				headers = list
			}

			function setDocumentSubHeaderName(headerId, name){
			}
		}
	}

	// The editor of a page, with no unsaved changes to ask about
	Component {
		id: pageEditorComp

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

	function openEditor(){
		let model = createTemporaryObject(emptyDataComp, testCase)
		let editor = createTemporaryObject(editorComp, testCase.parent, {"model": model})
		let pageView = findItem(editor, function(item){ return item.updatePages !== undefined && item.pagesModel !== undefined })
		verify(pageView)
		let spy = createTemporaryObject(spyComp, testCase, {"target": pageView})
		wait(100)

		return {"model": model, "editor": editor, "pageView": pageView, "spy": spy}
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

	// An existing organization in an editor hosted by a view continuing its header
	function openHostedEditor(){
		let context = openEditor()
		context.model.copyFrom(createTemporaryObject(representationComp, testCase))
		wait(100)
		context.host = createTemporaryObject(headerHostComp, testCase.parent)
		context.editor.documentManagerView = context.host
		return context
	}

	// Shows the section and opens a create editor in it
	function openPageEditor(context, pageId){
		context.pageView.activatePage(context.pageView.getIndexById(pageId))
		wait(50)
		let page = context.pageView.getPageById(pageId)
		verify(page, pageId + " is loaded")
		page.customEditorComponent = pageEditorComp
		page.openCreate()
		verify(page.editorOpen)
		return page
	}

	function test_hostShowsSectionAndEditor(){
		let context = openHostedEditor()
		compare(context.host.subHeaderNavigator, context.editor)
		compare(context.host.headers, ["section:General"])

		let page = openPageEditor(context, "Members")
		compare(context.host.headers.length, 2)
		compare(context.host.headers[0], "section:Members")
		compare(context.host.headers[1], "editor:" + page.editorTitle)
		verify(context.editor.canNavigateBack())

		context.editor.navigateBack()
		verify(!page.editorOpen)
		compare(context.host.headers, ["section:Members"])
		verify(!context.editor.canNavigateBack())
	}

	function test_pagesShowNoHeaderOfTheirOwnWhenHosted(){
		let context = openHostedEditor()
		let page = openPageEditor(context, "Members")

		compare(page.headerHost, context.editor)
	}

	// Another section keeps its editor; the header shows the editor of the section shown
	function test_sectionSwitchKeepsTheEditor(){
		let context = openHostedEditor()
		let page = openPageEditor(context, "Members")

		context.pageView.activatePage(0)
		compare(context.host.headers, ["section:General"])
		verify(page.editorOpen, "The editor of the hidden section stays")

		context.pageView.activatePage(context.pageView.getIndexById("Members"))
		compare(context.host.headers[1], "editor:" + page.editorTitle)
	}

	function test_organizationHeaderGoesToTheFirstSection(){
		let context = openHostedEditor()
		let page = openPageEditor(context, "Members")

		context.editor.navigateToHeader("tenant-document-id")

		verify(!page.editorOpen)
		compare(context.pageView.currentIndex, 0)
	}

	// Leaving the organization closes the editors of all sections, each shown while it is closed
	function test_leaveClosesTheEditorsOfAllSections(){
		let context = openHostedEditor()
		let members = openPageEditor(context, "Members")
		let grants = openPageEditor(context, "CrossOrgGrants")
		context.pageView.activatePage(0)
		let shown = []
		members.editorOpenChanged.connect(function(){ shown.push("Members:" + context.pageView.currentIndex) })
		grants.editorOpenChanged.connect(function(){ shown.push("CrossOrgGrants:" + context.pageView.currentIndex) })
		let answers = []

		context.editor.requestLeave(function(left){
			answers.push(left)
		})

		compare(answers, [true])
		verify(!members.editorOpen)
		verify(!grants.editorOpen)
		compare(shown, ["Members:" + context.pageView.getIndexById("Members"), "CrossOrgGrants:" + context.pageView.getIndexById("CrossOrgGrants")])
	}

	// The pages are not rebuilt under an open editor: that would drop its changes
	function test_rebuildWaitsForTheOpenEditor(){
		let context = openHostedEditor()
		let page = openPageEditor(context, "Members")

		// the organization becomes a new one: its pages change
		context.model.m_id = ""
		wait(200)
		compare(context.pageView.getPageById("Members"), page, "The page with the editor stays")

		page.requestCloseEditor(function(closed){})
		tryVerify(function(){ return context.pageView.getIndexById("Members") < 0 }, 3000, "Rebuilt once the editor is closed")
	}
}