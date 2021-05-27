const express = require('express');

const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');


const db = require('../db/connection');

const router = express.Router();  //pass routes to app

db.con.getConnection((err) => {
    if (err) console.log(err);
    else
        console.log('Base de dados a funcionar');
});

module.exports = router; // now can be used by anyone wants to require it
