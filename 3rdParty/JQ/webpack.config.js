const path = require('path')

module.exports = {
    entry: "./core.js",
    output: {
        path: path.resolve(__dirname, 'dist'),
    },
}