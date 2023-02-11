import { PrismaClient } from ".prisma/client";
import { table } from "console";
import { Request , Response} from "express";
import moment from 'moment';

const prisma = new PrismaClient()

module.exports.getCurrentLedStatus = async (req:Request , res:Response) => {
    let ledData
    try {
        ledData = await prisma.iot_Lab_LED.findUnique({
            where:{
                Iot_Lab_LED_Id: 1
            }
        })

        res.send({
            data : ledData,
            msg : 'get LED status success !!'
        })

    } catch (error) {
        res.send({
            msg : 'get LED status faild !! '
        })
    }
}

module.exports.updateLedStatus = async (req:Request , res:Response) => {
    let ledStatus = +req.params.status
    try {
        await prisma.iot_Lab_LED.update({
            data:{
                Iot_Lab_LED_Status:ledStatus
            },
            where:{
                Iot_Lab_LED_Id: 1
            }
        })

        if(ledStatus === 1 ){
            res.send({
                status: true ,
                msg : 'turn on success !!'
            })
        }else{
            res.send({
                status: true ,
                msg : 'turn off success !!'
            })
        }

    } catch (error) {
        res.send({
            msg : 'update LED status faild !! '
        })
    }
}

module.exports.updateTempAndHumidity = async (req:Request , res:Response) => {
    let temperature = +req.params.Temp ;
    let Humidity = +req.params.Humidity ;
    let currentDate = new Date()

    try {
        await prisma.iot_Lab_DHT.create({
            data:{
                Iot_Lab_DHT_Temp: temperature ,
                Iot_Lab_DHT_Humidity: Humidity ,
                Iot_Lab_DHT_Date: currentDate
            }
        })

        res.send({
            msg : 'update temperature and humidity success !! '
        })
    } catch (error) {
        res.send({
            msg : 'update temperature and humidity faild !! '
        })
    }
}

module.exports.getTempAndHumidityData = async (req:Request , res:Response) => {
    let temp: number[] = []
    let humidity: number[] = []
    let date: string[] = []
    let ledStatus: number
    try {
        // DHT Data
        await prisma.iot_Lab_DHT.findMany({
            orderBy: {
                Iot_Lab_DHT_id : 'desc'
            },
            take: 30
        }).then(async (result)=>{
            result.map((result)=>{
                date.push(moment(result.Iot_Lab_DHT_Date).add(543 , 'years').format('DD/MM/YY h:mm:ss a'))
                humidity.push(result.Iot_Lab_DHT_Humidity)
                temp.push(result.Iot_Lab_DHT_Temp)
                return result
            })
        })
        // LED Status
        ledStatus = await prisma.iot_Lab_LED.findUnique({
            where:{
                Iot_Lab_LED_Id: 1
            }
        }).then(async (result)=>{
            return +result!.Iot_Lab_LED_Status
        })
        res.send({
            status : true ,
            msg: 'get dht data success !!' ,
            topTemp : temp[0] ,
            topHumidity : humidity[0],
            temp : temp.reverse() ,
            humidity : humidity.reverse() ,
            date : date.reverse() ,
            ledStatus : ledStatus
        })
    } catch (error) {
        console.error(error)
        res.send({
            msg: 'get dht data failed !!'
        })
    }
}
