import QtQuick 2.12
import QtTest 1.12
import imtguigql 1.0
import imtdocgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "GqlSingleDocCollectionPageView"
	when: windowShown

	Component {
		id: managerComp

		SubscriptionManager {
		}
	}

	Component {
		id: pageComp

		GqlSingleDocCollectionPageView {
		}
	}

	Component {
		id: serviceComp

		GqlBasedCollectionDocumentService {
		}
	}

	function init(){
		GqlTestTransport.reset()
	}

	function subscriptionsOf(manager, collectionId){
		return manager.subscriptionModel.filter(function(entry){
			return entry.query.GetQuery().indexOf(collectionId) >= 0
		}).length
	}

	function test_releaseStopsTheNotificationsOfTheService(){
		let manager = createTemporaryObject(managerComp, testCase)
		let service = createTemporaryObject(serviceComp, testCase, {"collectionId": "Released"})
		compare(subscriptionsOf(manager, "Released"), 2, "Document and undo notifications are subscribed")

		service.release()

		compare(subscriptionsOf(manager, "Released"), 0)
	}

	// On the web a page's document service can outlive the page; the page must not leave it subscribed or registered.
	function test_destroyedPageReleasesItsDocumentService(){
		let manager = createTemporaryObject(managerComp, testCase)
		let page = pageComp.createObject(testCase.parent, {"pageId": "PageCollection"})
		let service = page.documentManager
		compare(MainDocumentService.getDocumentService("PageCollection"), service)
		compare(subscriptionsOf(manager, "PageCollection"), 2)

		page.destroy()
		tryVerify(function(){ return subscriptionsOf(manager, "PageCollection") === 0 }, 5000, "The page stops the notifications of its service")
		verify(MainDocumentService.getDocumentService("PageCollection") === undefined, "The service is no longer registered")
	}
}
