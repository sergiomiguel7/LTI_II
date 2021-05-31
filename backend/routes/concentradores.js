const express = require('express');
const ConcentradoresController = require('../controllers/concentradores.controller');
const AuthController = require("../controllers/auth.controller");
const router = express.Router();

//concentradores
router.route('/')
    .get(AuthController.checkAuth, ConcentradoresController.getConcentradores)
    .post(AuthController.checkAuth, ConcentradoresController.createConcentrador)

module.exports = router; 