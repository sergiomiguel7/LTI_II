const fs = require('fs');
const path = require('path');
const DataController = require('./data.controller');


/**
 * @param id_concentrador - user must especify the id from concentrador
 * @query page, limit
 * @query id_sensor - filter from sensor id
 */
getValues = async (req, res) => {

    const limit = req.query.limit ? req.query.limit : 10;
    const page = req.query.page ? req.query.page : 1;
    const id_sensor = req.query.id_sensor;

    try {
        let sensores = [];

        if (!id_sensor) { //no filter
            let response = await DataController.getData("SELECT * FROM sensor WHERE id_concentrador = ?", [req.params.id_concentrador]);
            sensores.push(...response);
        } else {
            let response = await DataController.getData("SELECT * FROM sensor WHERE id = ?", [id_sensor]);
            sensores.push(...response);
        }

        let values = [];

        let tempValues = [];
        let counter = 0;

        if (sensores.length == 0) {
            res.status(200).json({ user: req.user, data: [] })
        }
        else
            sensores.forEach((sensor) => {
                DataController.getData("SELECT * FROM dado WHERE id_sensor = ? AND unidade = ? ORDER BY timestamp DESC", [sensor.id, req.params.unity]).then((response) => {
                    //               sensor["data"] = response;
                    tempValues.push(...response);

                    counter++;
                    if (counter == sensores.length) {

                        counter = 0;
                        tempValues.forEach(element => {
                            if (counter > ((page-1) * limit - 1) && counter < ((page-1) * limit + Number(limit))) {
                                values.push(element);
                            }
                            counter++;
                        });

                        res.status(200).json({user: req.user,data: values, storedValues: tempValues.length});
                    }
                });
            });

    }
    catch (err) {
        res.status(404)
            .send({ message: err })
    }
}



module.exports = {
    getValues,
}
