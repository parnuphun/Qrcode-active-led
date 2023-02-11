import express from "express"

const iot_route = express.Router()


const iot_ct = require('../controllers/iot_lab_controller')

iot_route.get('/getCurrentLedStatus',iot_ct.getCurrentLedStatus)

iot_route.get('/updateLedStatus/:status',iot_ct.updateLedStatus)

iot_route.get('/updateTempAndHumidity/:Temp/:Humidity',iot_ct.updateTempAndHumidity)

iot_route.get('/getTempAndHumidityData',iot_ct.getTempAndHumidityData)

module.exports = iot_route

