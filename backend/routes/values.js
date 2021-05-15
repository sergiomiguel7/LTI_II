const express = require('express');
const ValuesController = require('../controllers/values.controller');

const router = express.Router();

router.route('/')
    .get(ValuesController.getValues);

module.exports = router; 