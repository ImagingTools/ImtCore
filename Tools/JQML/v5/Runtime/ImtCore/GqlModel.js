const { defineType } = require('../Core/type')
const { post, session } = require('./GqlSession')
const { TreeItemModel } = require('./TreeItemModel')

class GqlModel extends TreeItemModel {
    setGqlQuery(query, headers){
        this.state = 'Loading'
        return post(this, query, headers, (status, text) => {
            if(status === 401) this.state = 'Unauthorized'
            else if(status === 403) this.state = 'Forbidden'
            else if(this.createFromJson(text)){
                if(this.$v[TreeItemModel.STATE] === 'Ready') this.state = 'Loading'
                this.state = 'Ready'
            } else {
                this.state = 'Error'
            }
        })
    }

    SetGqlQuery(query, headers){ return this.setGqlQuery(query, headers) }

    static SetGlobalAccessToken(token){ session.accessToken = String(token || '') }
    static SetRefreshToken(token){ session.refreshToken = String(token || '') }
    static SetProductId(id){ session.productId = String(id || '') }
    static GetGlobalAccessToken(){ return session.accessToken }
    static GetProductId(){ return session.productId }
}

// the static slots are reachable from instances in QML (gqlModel.SetGlobalAccessToken(...))
for(let name of ['SetGlobalAccessToken', 'SetRefreshToken', 'SetProductId', 'GetGlobalAccessToken', 'GetProductId']){
    GqlModel.prototype[name] = function(...args){ return GqlModel[name](...args) }
}

defineType(GqlModel)

module.exports = { GqlModel }
