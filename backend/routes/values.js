const express = require('express');
const ValuesController = require('../controllers/values.controller');
const AuthController = require("../controllers/auth.controller");
const router = express.Router();


//valores
router.route('/:id_concentrador')
    .get(AuthController.checkAuth ,ValuesController.getValues);
    

module.exports = router; 