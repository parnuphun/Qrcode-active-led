const express = require('express');
const mysql = require('mysql2');
const cors = require('cors')
const app = express();
const moment = require('moment')

const conn = mysql.createPool({
    host     : '127.0.0.1',
    user     : 'root',
    password : '',
    database : 'myWebSite',
})

conn.getConnection((err)=>{
    if (err) {
       console.log('db error');
       throw err
   }else{
       console.log('db connected');
   }
})

app.use(cors());
app.use(express.json({ extended: false}));
app.use(express.urlencoded({ extended: true}));

app.get('/ledStatus', async (req,res)=>{
    let ledData
    try {
        conn.query('SELECT * FROM MiniProject_Iot_Led WHERE MiniProject_Iot_Led_Id = 1',(err,result)=>{
            ledData = result ;
            res.send({
                data : ledData[0],
                msg : 'get led success !!'
            })
        })
    } catch (error) {
        res.send('error')
    }
})

app.get('/updateLedStatus/:status',(req,res)=>{
    console.log('in update led');
    let ledStatus = req.params.status
    try {
        conn.query('UPDATE MiniProject_Iot_Led SET MiniProject_Iot_Led_Status = ? WHERE MiniProject_Iot_Led_Id = 1',[ledStatus])
        res.send({
            status : true ,
            msg : 'update success !!'
        });
    } catch (error) {
        res.send({
            status : false ,
            msg : 'update failed !!'
        });
    }
})

app.get('/updateTemp/:Temp/:Humidity',(req,res)=>{
    const temperature = req.params.Temp ;
    const Humidity = req.params.Humidity ;
    const currentDate = new Date()
    try {
        conn.query(`
            INSERT MiniProject_Iot_DHT(
                MiniProject_Iot_DHT_Temp,
                MiniProject_Iot_DHT_Humidity,
                MiniProject_Iot_DHT_Date)
            VALUES(?,?,?)`,[temperature,Humidity,currentDate],(err,result)=>{
                res.send({
                    msg : 'insert success !!'
                });
            })
    } catch (error) {
        res.send({
            msg : 'insert error !!'
        });
    }
})

app.get('/getTempAndHumidityData',(req,res)=>{
    try {
        let date = []
        let temp = []
        let humidity = []
        sql_dht = `
            SELECT
                MiniProject_Iot_DHT_Temp AS Temp,
                MiniProject_Iot_DHT_Humidity AS Humidity,
                MiniProject_Iot_DHT_Date AS oldDate
            FROM MiniProject_Iot_DHT
            ORDER BY MiniProject_Iot_DHT_Date DESC
            LIMIT 30;
        `
        conn.query(sql_dht,(err,result)=>{
            let data = result.map((result)=>{
                date.push(moment(result.oldDate).add(543 , 'years').format('DD/MM/YY h:mm:ss a'))
                humidity.push(result.Humidity)
                temp.push(result.Temp)

                delete result.oldDate;
                return result
            })
            let oldData = result
            console.table(result);
            conn.query('SELECT MiniProject_Iot_Led_Status AS ledStatus FROM MiniProject_Iot_Led WHERE MiniProject_Iot_Led_Id = 1 ',(err,result)=>{
                res.send({
                    date : date.reverse() ,
                    temp : temp.reverse() ,
                    topTemp : oldData[0].Temp ,
                    humidity : humidity.reverse() ,
                    topHumidity : oldData[0].Humidity ,
                    ledStatus : result[0].ledStatus ,
                    status : true ,
                    msg : 'get data success !'
                })
            })
        })
    } catch (error) {
        res.send({
            msg : 'get data error !' ,
            status : false
        })
    }
})
app.listen(4020,console.log('server is stated at port ',4020))
