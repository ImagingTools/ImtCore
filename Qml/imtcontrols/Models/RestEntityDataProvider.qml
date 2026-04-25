import QtQuick 2.12

FilterableSelectDataProvider {
    id: root

    // Backend contract: GET /entities?search=<filter>&cursor=<cursor>&limit=<limit>
    property string endpoint: "/entities"
    property var headers: ({})
    property string itemsKey: "items"
    property string nextCursorKey: "nextCursor"
    property int timeout: 30000

    function buildUrl(params){
        let parts = []
        let filter = params && params.filter !== undefined ? params.filter : ""
        let cursor = params && params.cursor !== undefined ? params.cursor : null
        let limit = params && params.limit !== undefined ? params.limit : root.defaultLimit

        parts.push("search=" + encodeURIComponent(filter))
        if (cursor !== null && cursor !== undefined && cursor !== ""){
            parts.push("cursor=" + encodeURIComponent(cursor))
        }
        parts.push("limit=" + encodeURIComponent(limit))

        return root.endpoint + (root.endpoint.indexOf("?") >= 0 ? "&" : "?") + parts.join("&")
    }

    function fetch(params){
        return new Promise(function(resolve, reject){
            let xhr = new XMLHttpRequest()
            let timedOut = false
            let timer = Qt.createQmlObject('import QtQuick 2.12; Timer { interval: ' + root.timeout + '; repeat: false }', root)

            timer.triggered.connect(function(){
                timedOut = true
                xhr.abort()
                timer.destroy()
                reject("Request timed out")
            })

            xhr.onreadystatechange = function(){
                if (xhr.readyState !== XMLHttpRequest.DONE || timedOut){
                    return
                }

                timer.stop()
                timer.destroy()

                if (xhr.status < 200 || xhr.status >= 300){
                    reject(xhr.status + " " + xhr.statusText)
                    return
                }

                try {
                    let response = JSON.parse(xhr.responseText)
                    resolve({
                                "items": response[root.itemsKey] || [],
                                "nextCursor": response[root.nextCursorKey] === undefined ? null : response[root.nextCursorKey]
                            })
                }
                catch (error){
                    reject(error)
                }
            }

            xhr.open("GET", buildUrl(params), true)
            for (let headerName in root.headers){
                xhr.setRequestHeader(headerName, root.headers[headerName])
            }
            timer.start()
            xhr.send()
        })
    }
}
