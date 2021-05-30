const { validationResult } = require('express-validator');
const jwt = require('jsonwebtoken');
const bcrypt = require('bcryptjs');
const DataController = require('./data.controller');

login = (req, res, next) => {
    const errors = validationResult(req).array();
    if (errors.length > 0) return res.status(406).send(errors);

    DataController.getData("SELECT * FROM user WHERE username = ?", [req.body.username])
        .then((rows) => {
            if (rows.length == 0) {
                res.status(404).send({
                    message: "user not found"
                });
            } else {
                bcrypt.compare(req.body.password, rows[0].password, (err, result) => {
                    if (result == true) {
                        const payload = {
                            id: rows[0].id,
                            username: rows[0].username
                        };
                        let token = jwt.sign(payload, process.env.TOKEN_SECRET, { expiresIn: '4d' });
                        DataController.getData("UPDATE user SET ? WHERE id = ?",
                            [{ token: token }, payload.id]).then((rows2) => {
                                if (rows2.affectedRows > 0)
                                    res.status(200).json({ token: token, id: payload.id });
                            }).catch((err) => {
                                res.status(500).json({ message: err.message });
                            });
                    } else {
                        const err = new Error('Wrong password');
                        res.status(401).json({ message: err.message });
                    }
                });
            }
        })
        .catch((err) => {
            res.status(401).json({ message: err.message })
        });

}

register = async (req, res, next) => {
    const errors = validationResult(req).array();
    if (errors.length > 0) return res.status(406).send(errors);

    let db = global.con;

    DataController.getData("SELECT * FROM user WHERE username = ?", [req.body.username])
        .then((rows) => {
            if (rows.length > 0) {
                res.status(404).send({
                    message: "user already exists"
                });
            } else {
                bcrypt.hash(req.body.password, 12, (err, hash) => {
                    let newUser = {
                        username: req.body.username,
                        password: hash
                    }
                    DataController.getData('INSERT INTO user SET ?', [newUser])
                        .then((rows2) => {
                            res.status(200).json({ id: rows2.insertId });
                        })
                        .catch((err) => {
                            res.status(503).json({ message: err.message })
                        })
                });
            }
        })
        .catch((err) => {
            res.status(401).json({ message: err.message })
        });

}

module.exports = {
    login,
    register
}