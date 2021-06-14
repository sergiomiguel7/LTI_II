const express = require('express');
const ConcentradoresController = require('../controllers/concentradores.controller');
const AuthController = require("../controllers/auth.controller");
const router = express.Router();

//concentradores
router.route('/')
    .get(AuthController.checkAuth, ConcentradoresController.getConcentradores)
    .post(AuthController.checkAuth, ConcentradoresController.createConcentrador)

router.route('/:id_concentrador')
    .patch(AuthController.checkAuth, ConcentradoresController.updateConcentrador)
    .delete(AuthController.checkAuth, ConcentradoresController.deleteConcentrador)

module.exports = router; 