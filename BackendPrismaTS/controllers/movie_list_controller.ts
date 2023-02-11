import { PrismaClient } from "@prisma/client";
import { Console } from "console";
import { Request , Response} from "express";
import { stat } from "fs";

const prisma = new PrismaClient

module.exports.addNewTitle = async (req:Request,res:Response) => {
    let title:string = req.body.title
    let poster:string = req.body.poster
    let rating:number = +req.body.rating
    let status:number = +req.body.status
    let currentDate:Date = new Date()

    try {
        await prisma.movie_List.create({
            data: {
                Movie_List_Title: title ,
                Movie_List_Poster: poster ,
                Movie_List_Rating: rating ,
                Movie_List_Watch_Status: Boolean(status),
                Movie_List_Date: currentDate
            }
        })

        res.send({
            status: true ,
            msg : `Add ${title} Success !!`
        })
    } catch (error) {
        console.error(error)

        res.send({
            status: false ,
            msg : `Someting Wrong. Add ${title} Failed !!`
        })
    }

}

module.exports.getAllMovieList = async (req:Request , res:Response) => {
    try {
        let movieList = await prisma.movie_List.findMany({
            orderBy: {
                Movie_List_Date : 'desc'
            }
        })
        res.send({
            status: false ,
            movieList: movieList ,
            msg : `get all movie Success !!`
        })

    } catch (error) {
        console.error(error)

        res.send({
            status: false ,
            msg : `something wrong !! . can't get all movie`
        })
    }
}
