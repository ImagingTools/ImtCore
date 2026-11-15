const engine = require('../Core/engine')

// CGqlModel::s_accessToken / s_refreshToken / s_productId
const session = { accessToken: '', refreshToken: '', productId: '' }

// The web client is served from <app>/<views>/html/; the server answers on <app>/graphql
let graphqlUrl = '../../graphql'

// Other XMLHttpRequests of the QML code carry the token too (the current runtime does the same)
if(typeof XMLHttpRequest !== 'undefined' && !XMLHttpRequest.prototype.$jq2Token){
    let open = XMLHttpRequest.prototype.open
    XMLHttpRequest.prototype.$jq2Token = true
    XMLHttpRequest.prototype.open = function(...args){
        let result = open.apply(this, args)
        if(session.accessToken && !this.$gql) this.setRequestHeader('x-authentication-token', session.accessToken)
        return result
    }
}

function post(owner, query, headers, done){
    if(typeof XMLHttpRequest === 'undefined') return false
    let xhr = new XMLHttpRequest()
    xhr.$gql = true
    xhr.open('POST', graphqlUrl)
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded')
    let given = new Set()
    if(headers && typeof headers === 'object'){
        for(let name in headers){
            xhr.setRequestHeader(name, String(headers[name]))
            given.add(name.toLowerCase())
        }
    }
    if(session.productId && !given.has('productid')) xhr.setRequestHeader('productId', session.productId)
    if(session.accessToken && !given.has('x-authentication-token')) xhr.setRequestHeader('x-authentication-token', session.accessToken)
    xhr.onreadystatechange = () => {
        if(xhr.readyState !== 4 || owner.$f & engine.DESTROYED || engine.isStopped()) return
        engine.batch(() => done(xhr.status, xhr.responseText))
    }
    xhr.send(String(query))
    return true
}

function setGraphqlUrl(url){
    graphqlUrl = url
}

module.exports = { session, post, setGraphqlUrl }
