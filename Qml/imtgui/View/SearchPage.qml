import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtbaseSearchSdl 1.0

Item {
	id: root;

	property string currentText: ""
	property int pageSize: 30
	property var categories: null   // groups metadata from discovery (id, name, totalCount). items empty except possibly first group.
	property var activeTabItems: null
	property int activeTabIndex: -1
	property int activeTabTotal: 0
	property bool loadingMore: false
	property string pendingRequestKey: ""
	property var tabDataArrays: ({})

	// Accumulating model for paged items of the currently active group/tab.
	// We use ListModel to support append on loadMore (plain list assignment would lose previous pages).
	ListModel {
		id: activeItemsModel
	}

	Component.onCompleted: {
		Events.subscribeEvent("GlobalSearchActivated", root.updateSearch)
	}
	
	Component.onDestruction: {
		Events.unSubscribeEvent("GlobalSearchActivated", root.updateSearch)
	}
	
	function makeRequestKey(text, rid, off) {
		return text + "|" + (rid ? rid : "") + "|" + off
	}

	function updateSearch(text){
		if (text === ""){
			searchResultsView.searchResults = null;
			root.categories = null
			root.activeTabItems = null
			activeItemsModel.clear()
			root.activeTabIndex = -1
			root.activeTabTotal = 0
			root.currentText = ""
			return;
		}

		if (text !== root.currentText){
			root.tabDataArrays = {}
		}

		if (text === root.currentText && root.categories){
			// same text, just ensure first tab
			if (root.activeTabIndex < 0 && root.categories && root.categories.m_searchResults.count > 0){
				root.selectTab(0)
			}
			return
		}

		root.currentText = text
		root.categories = null
		root.activeTabItems = null
		activeItemsModel.clear()
		root.activeTabIndex = -1
		root.activeTabTotal = 0
		root.loadingMore = false

		searchInput.m_text = text
		searchInput.m_searchResultId = ""
		searchInput.m_offset = 0
		searchInput.m_count = root.pageSize

		root.pendingRequestKey = root.makeRequestKey(text, "", 0)
		searchRequestSender.send(searchInput)
	}

	function selectTab(tabIndex){
		if (!root.categories || tabIndex < 0){
			return
		}
		let catList = root.categories.m_searchResults
		if (tabIndex >= catList.count){
			return
		}
		let cat = catList.get(tabIndex).item
		if (!cat){
			return
		}
		root.activeTabIndex = tabIndex
		root.activeTabTotal = cat.m_totalCount ? cat.m_totalCount : 0

		let rid = cat.m_id ? cat.m_id : ""

		root.activeTabItems = null
		activeItemsModel.clear()

		// Do not re-send request if we already have loaded data for this tab (from discovery or previous switch).
		// Only re-request if search text changed (cache cleared) or for pagination (loadMore).
		if (root.tabDataArrays[rid] && root.tabDataArrays[rid].length > 0){
			let cached = root.tabDataArrays[rid]
			for (let j = 0; j < cached.length; ++j){
				activeItemsModel.append(cached[j])
			}
			root.activeTabItems = activeItemsModel
			return
		}

		// fetch first page for this tab (explicit per-group pagination)
		searchInput.m_text = root.currentText
		searchInput.m_searchResultId = rid
		searchInput.m_offset = 0
		searchInput.m_count = root.pageSize

		root.pendingRequestKey = root.makeRequestKey(root.currentText, rid, 0)
		root.loadingMore = false
		searchRequestSender.send(searchInput)
	}

	function loadMore(){
		if (root.loadingMore || root.activeTabIndex < 0 || !root.categories){
			return
		}
		let catList = root.categories.m_searchResults
		if (root.activeTabIndex >= catList.count){
			return
		}
		let cat = catList.get(root.activeTabIndex).item
		if (!cat){
			return
		}
		let rid = cat.m_id ? cat.m_id : ""
		let currentLoaded = activeItemsModel.count
		if (root.activeTabTotal > 0 && currentLoaded >= root.activeTabTotal){
			return
		}

		// Request next page for the *current group only* (per-group pagination)
		searchInput.m_text = root.currentText
		searchInput.m_searchResultId = rid
		searchInput.m_offset = currentLoaded
		searchInput.m_count = root.pageSize

		root.pendingRequestKey = root.makeRequestKey(root.currentText, rid, currentLoaded)
		root.loadingMore = true
		searchRequestSender.send(searchInput)
	}

	SearchResultsView {
		id: searchResultsView;
		anchors.fill: parent;

		categories: root.categories
		activeItems: root.activeTabItems
		activeTabIndex: root.activeTabIndex
		activeTotal: root.activeTabTotal
		loadingMore: root.loadingMore
		searchText: root.currentText

		onTabSelected: {
			root.selectTab(tabIndex)
		}

		onRequestLoadMore: {
			root.loadMore()
		}
		
		onItemDoubleClicked: {
			// Support both full searchResults shape and our active items
			let itemsSource = root.activeTabItems
			let useTab = root.activeTabIndex
			if (!itemsSource && searchResultsView.searchResults){
				itemsSource = null
				if (searchResultsView.searchResults.m_searchResults && useTab >= 0){
					let r = searchResultsView.searchResults.m_searchResults.get(useTab)
					if (r && r.item){
						itemsSource = r.item.m_items
					}
				}
			}
			if ((!itemsSource || useTab < 0) && searchResultsView.searchResults && useTab >= 0 && itemIndex >= 0){
				let resultItem = searchResultsView.searchResults.m_searchResults.get(useTab).item;
				if (!resultItem){
					return;
				}
				let item = resultItem.m_items.get(itemIndex).item;
				if (!item){
					return;
				}
				let objectLink = item.m_objectLink
				if (!objectLink){
					console.error("Unable to navigate to item. Error: Object Link is invalid")
					return
				}
				let url = objectLink.m_url
				if (!url){
					console.error("Unable to navigate to item. Error: URL is invalid")
					return
				}
				let params = {}
				params["textFilter"] = root.currentText
				params["resultName"] = item.m_resultName
				params["resultDescription"] = item.m_resultDescription
				params["contextId"] = item.m_contextId
				params["contextTypeId"] = item.m_contextTypeId
				NavigationController.navigate(url.m_path, params)
				return
			}
			if (itemsSource && itemIndex >= 0){
				let it = itemsSource.get ? itemsSource.get(itemIndex) : null
				let item = it && it.item ? it.item : it
				if (!item){
					return
				}
				let objectLink = item.m_objectLink
				if (!objectLink){
					console.error("Unable to navigate to item. Error: Object Link is invalid")
					return
				}
				let url = objectLink.m_url
				if (!url){
					console.error("Unable to navigate to item. Error: URL is invalid")
					return
				}
				let params = {}
				params["textFilter"] = root.currentText
				params["resultName"] = item.m_resultName
				params["resultDescription"] = item.m_resultDescription
				params["contextId"] = item.m_contextId
				params["contextTypeId"] = item.m_contextTypeId
				NavigationController.navigate(url.m_path, params)
			}
		}
	}
	
	SearchInput {
		id: searchInput;
		m_text: '';
	}
	
	GqlSdlRequestSender {
		id: searchRequestSender;
		gqlCommandId: ImtbaseSearchSdlCommandIds.s_search;
		
		sdlObjectComp: Component {
			SearchResults {
				onFinished: {
					let key = root.pendingRequestKey
					let thisSearchResults = this
					let resultsList = thisSearchResults && thisSearchResults.m_searchResults

					// Parse request key to know exactly what we asked for (robust vs count===1 heuristic)
					// key format: text|rid|offset
					let reqText = ""
					let reqRid = ""
					let reqOffset = 0
					if (key && key.indexOf("|") >= 0){
						let parts = key.split("|")
						if (parts.length >= 1){
							reqText = parts[0] || ""
						}
						if (parts.length >= 2){
							reqRid = parts[1] || ""
						}
						if (parts.length >= 3){
							reqOffset = parseInt(parts[2]) || 0
						}
					}

					// Stale protection
					if (reqText && reqText !== root.currentText){
						root.loadingMore = false
						return
					}
					if (root.currentText === "" || !thisSearchResults){
						root.loadingMore = false
						return
					}

					let isForSpecificGroup = (reqRid && reqRid.length > 0)

					if (isForSpecificGroup){
						// Guard against late responses after tab switch (same search text)
						if (root.activeTabIndex >= 0 && root.categories && root.categories.m_searchResults){
							let cl = root.categories.m_searchResults
							if (root.activeTabIndex < cl.count){
								let cc = cl.get(root.activeTabIndex).item
								let curRid = (cc && cc.m_id) ? cc.m_id : ""
								if (reqRid !== curRid){
									root.loadingMore = false
									return
								}
							}
						}

						// Paged items response for the active tab/group.
						// Response contains exactly 1 SearchResult for the requested rid.
						let cat = null
						if (resultsList && resultsList.count > 0){
							cat = resultsList.get(0).item
							// If more than one somehow, try to find matching rid
							for (let i = 0; i < resultsList.count; ++i){
								let candidate = resultsList.get(i).item
								if (candidate && candidate.m_id === reqRid){
									cat = candidate
									break
								}
							}
						}

						let pageItems = cat && cat.m_items ? cat.m_items : null
						let isAppend = (reqOffset > 0)

						if (!isAppend){
							activeItemsModel.clear()
						}

						if (pageItems){
							if (!root.tabDataArrays[reqRid]){
								root.tabDataArrays[reqRid] = []
							}
							if (!isAppend){
								root.tabDataArrays[reqRid] = []
							}
							for (let j = 0; j < pageItems.count; ++j){
								let wrapper = pageItems.get(j)
								// Preserve shape expected by delegates: { item: ... }
								let entry = (wrapper && wrapper.item) ? wrapper : { item: wrapper }
								activeItemsModel.append(entry)
								root.tabDataArrays[reqRid].push(entry)
							}
						}

						root.activeTabItems = activeItemsModel

						if (cat && cat.m_totalCount !== undefined && cat.m_totalCount !== null){
							root.activeTabTotal = cat.m_totalCount
						}
						root.loadingMore = false
					}
					else{
						// Discovery response (empty searchResultId): info about ALL SearchResult/groups
						// with their totals. Data (items) empty except for the first connected controller's
						// initial page (using provided pagination), since empty id means show data from first.
						// Only load specific tab data via pagination when user switches tab in GUI.
						activeItemsModel.clear()
						if (resultsList && resultsList.count > 0){
							// populate first BEFORE set categories (updateGui may trigger tabSelected which uses cache)
							let firstCat = resultsList.get(0).item
							if (firstCat){
								let firstRid = firstCat.m_id ? firstCat.m_id : ""
								root.tabDataArrays[firstRid] = []
								root.activeTabIndex = 0
								root.activeTabTotal = firstCat.m_totalCount ? firstCat.m_totalCount : 0
								let firstItems = firstCat.m_items
								if (firstItems && firstItems.count > 0){
									for (let j = 0; j < firstItems.count; ++j){
										let w = firstItems.get(j)
										let entry = (w && w.item) ? w : { item: w }
										activeItemsModel.append(entry)
										root.tabDataArrays[firstRid].push(entry)
									}
									root.activeTabItems = activeItemsModel
								}
							}
						}
						root.categories = thisSearchResults
						searchResultsView.searchResults = thisSearchResults
					}
				}
			}
		}
		
		onStateChanged: {
			if (state === "Loading"){
				if (!root.loadingMore){
					searchResultsView.startLoading()
				}
			}
			else{
				searchResultsView.stopLoading()
				if (state !== "Loading"){
					root.loadingMore = false
				}
			}
		}
	}
}


