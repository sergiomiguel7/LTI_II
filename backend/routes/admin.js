const express = require('express');
const AuthController = require('../controllers/auth.controller');
const AdminController = require('../controllers/admin.controller')

const router = express.Router();

//signup
router.route('/users')
    .get(AuthController.checkAuth, AdminController.getUsers);
//login
router.route('/concentradores')
    .get(AuthController.checkAuth, AdminController.getConcentradores);

module.exports = router;