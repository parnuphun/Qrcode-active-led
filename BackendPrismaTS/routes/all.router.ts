import express from 'express'

const route = express()


route.use(require('../routes/iot_lab_routes'))
route.use(require('../routes/movie_list_routes'))

module.exports = route
