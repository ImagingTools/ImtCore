const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { post } = require('./GqlSession')

class GqlRequest extends QtObject {
    setGqlQuery(query, headers){
        this.state = 'Loading'
        return post(this, query, headers, (status, text) => {
            if(status === 401) this.state = 'Unauthorized'
            else if(status === 403) this.state = 'Forbidden'
            else if(status === 200){
                // m_json is set without jsonChanged, like CGqlRequest::replyFinished
                this.$v[GqlRequest.JSON] = text
                if(!text) this.state = 'Error'
                else {
                    if(this.$v[GqlRequest.STATE] === 'Ready') this.state = 'Loading'
                    this.state = 'Ready'
                }
            } else {
                // "Every outcome has to leave Loading"
                this.state = 'Error'
            }
        })
    }

    SetGqlQuery(query, headers){ return this.setGqlQuery(query, headers) }
}

defineType(GqlRequest, {
    props: {
        state: 'string',
        json: 'string',
    },
})

module.exports = { GqlRequest }
