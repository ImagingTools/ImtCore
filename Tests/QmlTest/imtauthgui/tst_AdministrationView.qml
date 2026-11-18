import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0
import imtauthgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "AdministrationView"
	when: windowShown

	// The page of the application that holds the administration, the parent of its sub-page paths.
	NavigableItem {
		id: rootNavigation

		paths: ["Administration"]
	}

	Component {
		id: productPageComp

		Item {
		}
	}

	// A product adds its own sub-pages after the ImtCore ones, as IotPlatform and IronBill do.
	Component {
		id: viewComp

		AdministrationView {
			id: productView

			onMultiPageUpdated: {
				productView.multiPageView.addPage("Abonent", "Abonents", productPageComp, "")
				productView.multiPageView.addPage("Tools", "Tools", productPageComp, "")
			}
		}
	}

	property string previousLogin: ""
	property var previousPermissions: []

	function init(){
		GqlTestTransport.reset()
		NavigationController.clear()
		previousLogin = AuthorizationController.userTokenProvider.login
		previousPermissions = AuthorizationController.userTokenProvider.permissions

		AuthorizationController.userTokenProvider.login = "user"
		AuthorizationController.userTokenProvider.permissions = []
	}

	function cleanup(){
		NavigationController.clear()
		AuthorizationController.userTokenProvider.login = previousLogin
		AuthorizationController.userTokenProvider.permissions = previousPermissions
	}

	function createView(){
		// TestCase itself is invisible, the view needs a visible parent.
		return createTemporaryObject(viewComp, testCase.parent)
	}

	function pageIds(view){
		let ids = []
		for (let i = 0; i < view.multiPageView.pagesModel.count; ++i){
			ids.push(view.multiPageView.pagesModel.get(i).id)
		}

		return ids
	}

	function verifyCurrentPage(view, pageId){
		let index = view.multiPageView.getIndexById(pageId)
		verify(index >= 0, "the sub-page '" + pageId + "' is present")
		compare(view.multiPageView.currentIndex, index, "the sub-page '" + pageId + "' is current")
		verify(view.multiPageView.pagesModel.get(index).loaded, "the sub-page '" + pageId + "' is loaded")
		verify(view.multiPageView.getPageByIndex(index) !== null, "the sub-page '" + pageId + "' has its item")
	}

	// Without Roles, Users and Groups the first sub-page of the product is the one shown, not an empty one.
	function test_theFirstProductPageLoadsWithoutImtCorePages(){
		let view = createView()

		compare(pageIds(view), ["Abonent", "Tools"])
		verifyCurrentPage(view, "Abonent")
		verify(!view.multiPageView.pagesModel.get(1).loaded, "the other sub-pages load when they are opened")
	}

	function test_rolesComeFirstWhenAllPagesAreAllowed_data(){
		return [
			{tag: "Superuser", login: "su", permissions: []},
			{tag: "Permissions", login: "user", permissions: ["/Administration/Roles/ViewPage", "/Administration/Users/ViewPage", "/Administration/Groups/ViewPage"]}
		]
	}

	function test_rolesComeFirstWhenAllPagesAreAllowed(data){
		AuthorizationController.userTokenProvider.login = data.login
		AuthorizationController.userTokenProvider.permissions = data.permissions

		let view = createView()

		compare(pageIds(view), ["Roles", "Users", "Groups", "Abonent", "Tools"])
		verifyCurrentPage(view, "Roles")
	}

	// Sub-pages the user may not see are left out, the others keep their order.
	function test_onlyTheAllowedPagesArePresent(){
		AuthorizationController.userTokenProvider.permissions = ["/Administration/Groups/ViewPage"]

		let view = createView()

		compare(pageIds(view), ["Groups", "Abonent", "Tools"])
		verifyCurrentPage(view, "Groups")
	}

	// A path to a sub-page of the product, waiting since before the view was built, opens that sub-page.
	function test_aPendingPathOpensAProductPage(){
		NavigationController.navigate("Administration/Tools")

		let view = createView()

		verifyCurrentPage(view, "Tools")

		let adminNavigation = null
		for (let i = 0; i < NavigationController.navigableItems.length; ++i){
			if (NavigationController.navigableItems[i].parentSegment === "Administration"){
				adminNavigation = NavigationController.navigableItems[i]
			}
		}

		verify(adminNavigation !== null, "the sub-pages are registered for navigation")
		compare(adminNavigation.paths, ["Abonent", "Tools"])
	}

	// A path given to the built view switches to the sub-page of the product.
	function test_aPathOpensAProductPage(){
		let view = createView()
		verifyCurrentPage(view, "Abonent")

		NavigationController.navigate("Administration/Tools")

		verifyCurrentPage(view, "Tools")
	}
}
