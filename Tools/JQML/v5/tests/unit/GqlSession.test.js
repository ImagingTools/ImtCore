// Run: node --test tests/unit
// A GraphQL request names its sender on the XMLHttpRequest (xhr.gqlRequest), as CNetworkEventInterceptor tells the Qt client:
// XmlHttpRequestProxy.qml queues a request refused with 401 by it, and AuthorizationController sends it again after the
// token is refreshed, with the headers its sender gave. Without the sender nothing is sent again.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

// the browser's XMLHttpRequest for the time of a test: every request is kept, none is answered
function withRequests(){
    let requests = []
    class FakeRequest {
        constructor(){
            this.headers = {}
            requests.push(this)
        }
        open(method, url){ this.method = method; this.url = url }
        setRequestHeader(name, value){ this.headers[name] = value }
        send(body){ this.body = body }
    }
    let previous = Object.getOwnPropertyDescriptor(globalThis, 'XMLHttpRequest')
    Object.defineProperty(globalThis, 'XMLHttpRequest', { value: FakeRequest, configurable: true, writable: true })
    requests.restore = () => {
        if(previous) Object.defineProperty(globalThis, 'XMLHttpRequest', previous)
        else delete globalThis.XMLHttpRequest
    }
    return requests
}

function cases(name, runtime){
    const { engine, types } = runtime

    for(let typeName of ['GqlModel', 'GqlRequest']){
        test(name + ': a ' + typeName + ' names itself on its request, with the headers it was given', () => {
            let requests = withRequests()
            try {
                let sender
                engine.batch(() => { sender = new types[typeName](null) })
                let query = '{"query":"query DevicesList {DevicesList(input: {}) {}}"}'
                engine.batch(() => sender.setGqlQuery(query, { permissionPath: '/Devices', 'X-Request-ID': 'request-1' }))

                assert.equal(requests.length, 1)
                assert.equal(requests[0].gqlRequest, sender)
                assert.equal(requests[0].body, query)
                assert.equal(requests[0].headers.permissionPath, '/Devices')
                assert.equal(requests[0].headers['X-Request-ID'], 'request-1')
            } finally {
                requests.restore()
            }
        })
    }
}

for(let [name, load] of runtimes){
    cases(name, load())
}
