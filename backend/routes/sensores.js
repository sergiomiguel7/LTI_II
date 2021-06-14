const express = require('express');
const SensoresController = require('../controllers/sensores.controller');
const AuthController = require("../controllers/auth.controller");
const router = express.Router();

//sensores
router.route('/:id_concentrador')
    .get(AuthController.checkAuth, SensoresController.getSensores)
    .post(AuthController.checkAuth, SensoresController.createSensor) // nedded ? -> it's done

router.route('/:id_sensor')
    .patch(AuthController.checkAuth, SensoresController.updateSensor) // nedded ?
    .delete(AuthController.checkAuth, SensoresController.deleteSensor) // nedded ?

module.exports = router; 