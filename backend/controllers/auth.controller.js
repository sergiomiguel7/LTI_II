const { validationResult } = require('express-validator');
const jwt = require('jsonwebtoken');
const bcrypt = require('bcryptjs');

login = async (req, res, next) => {
    const errors = validationResult(req).array();
    if (errors.length > 0) return res.status(406).send(errors);

    let db = global.con;

    //check if user already exists
    db.query("SELECT * FROM user WHERE username = ?"
        , req.body.username, (err, rows, fields) => {
            if (err) return res.status(503).send();

            if (rows.length == 0) {
                res.status(404).send({
                    message: "user not found"
                });
            } else {
                let parsed = JSON.parse(JSON.stringify(rows));
                bcrypt.compare(req.body.password, parsed[0].password, (err, result) => {
                    if (result == true) {
                        const payload = {
                            id: parsed[0].id,
                            username: parsed[0].username
                        };
                        let token = jwt.sign(payload, process.env.TOKEN_SECRET, { expiresIn: '4d' });
                        console.log("token", token.length);
                        db.query("UPDATE user SET ? WHERE id = ?",
                            [{ token: token }, payload.id], (err, result) => {
                                if (err) res.status(500).json({ message: err.message });
                                else {
                                    console.log("Affected rows" + result);
                                    res.status(200).json({ token: token, id: parsed[0].id });
                                }
                            });

                    } else {
                        const err = new Error('Wrong password');
                        res.status(401).json({ message: err.message });
                    }
                });
            }
        });
}

register = async (req, res, next) => {
    const errors = validationResult(req).array();
    if (errors.length > 0) return res.status(406).send(errors);

    let db = global.con;

    db.query("SELECT * FROM user WHERE username = ?"
        , req.body.username, (err, rows, fields) => {
            if (err) return res.status(503).send();

            if (rows.length > 0) {
                res.status(401).send({
                    message: "user already exists"
                });
            } else {
                bcrypt.hash(req.body.password, 12, (err, hash) => {
                    let newUser = {
                        username: req.body.username,
                        password: hash
                    }

                    console.log(newUser);
                    db.query('INSERT INTO user SET ?', newUser, (err, rows) => {
                        if (err) console.log(err);

                        res.status(200).json({ id: rows.insertId });
                    })
                })
            }

        });
}

module.exports = {
    login,
    register
}