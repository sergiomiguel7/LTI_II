module.exports = (app) => {
    const express = require('express');
    const cors = require('cors');
    const helmet = require('helmet');
    const morgan = require('morgan');
    require('dotenv').config();

    app.use(express.json());
    app.use(express.urlencoded({extended: true}));
    app.use(cors());
    app.use(morgan('dev'));
    app.use(helmet());

}