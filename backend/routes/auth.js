const express = require('express');
const validator = require('express-validator');
const AuthController = require('../controllers/auth.controller');

const router = express.Router();

//signup
router.route('/signup')
    .post([validator.body('username').isString(), validator.body('password').isString()],AuthController.register);
//login
router.route('/login')
    .post([validator.body('username').isString()], AuthController.login);

module.exports = router;