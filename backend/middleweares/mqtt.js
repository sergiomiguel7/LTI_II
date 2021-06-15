    const express = require('express');
    const mqtt = require('mqtt')

    const router = express.Router();

    var client = mqtt.connect('mqtt://localhost')

    /**
     * body = {iss: 1, state: 0/1/2}
     */
    router.post('/', async(req,res,next) => {
        console.log("body", req.body);
        client.publish(`room/light/${req.body.iss}`, `${req.body.state}`);

        res.json({msg: "ok"});
    });


    module.exports = router;