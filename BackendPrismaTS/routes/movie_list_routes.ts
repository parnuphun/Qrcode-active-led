import express from "express"

const movie_route = express.Router()


const movie_list_ct = require('../controllers/movie_list_controller')

movie_route.post('/movie_List/addNewTitle',movie_list_ct.addNewTitle)

movie_route.get('/movie_List/getAllMovieList',movie_list_ct.getAllMovieList)

module.exports = movie_route

