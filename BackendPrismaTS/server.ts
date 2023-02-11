import express from 'express'

const cors = require('cors')
const routes = require('./routes/all.router')
const app = express()
const port:number = 4020 ;

app.use(express.json());
app.use(express.urlencoded({ extended: false}));

// app.use(cors())
app.use(routes)

app.listen(port || 4000 , ()=>{
    console.log('Server start at port ',port, ' ...');
})
