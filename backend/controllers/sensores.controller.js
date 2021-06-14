const DataController = require('./data.controller');

/**
 * @param id_concentrador - only can get sensor from concentrador id
 */
getSensores = async (req, res) => {
    try {
        let response = await DataController.getData("SELECT * FROM sensor WHERE id_concentrador = ?", [req.params.id_concentrador]);
        res.status(200).json({
            user: req.user,
            data: response
        })
    } catch (err) {
        res.status(404)
            .send({ message: err })
    }

}

createSensor = async (req,res) => {

    req.body["id_concentrador"] = req.params.id_concentrador;
    
    try{
        let response = await DataController.getData("INSERT INTO sensor SET ?" , [req.body]);
        let data = {...req.body, id: response.insertId}
        res.status(200).json({
            user: req.user,
            data: data
        });
    }catch (err){
        req.status(404).send({message: err})
    }
}

/**
 * @param id_concentrador
 */
updateSensor = async (req, res) => {
    console.log(req.body);
    try{ 
        let response = await DataController.getData("UPDATE concentrador SET ? WHERE id = ?",[req.body ,req.params.id_concentrador]);
        let data = {...req.body}
        res.status(200).json({
            user: req.user,
            data: data
        });
    } catch(err){
        req.status(404).send({message: err});
    }
}

/**
 * @param id_concentrador
 */
 deleteSensor = async (req, res) => {
    console.log(req.body);
    try{ 
        let response = await DataController.getData("DELETE FROM concentrador WHERE id = ?",[req.params.id_concentrador]);
        res.status(200).json({
            user: req.user,
            message: "Removed with sucess"
        });
    } catch(err){
        req.status(404).send({message: err});
    }
}


module.exports = {
    getSensores,
    createSensor,
    updateSensor,
    deleteSensor
}