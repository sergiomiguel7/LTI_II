const DataController = require('./data.controller');

getConcentradores = async (req, res) => {
    try {
        let response = await DataController.getData("SELECT * FROM concentrador WHERE id_user = ?", [req.user.id]);
        res.status(200).json({
            user: req.user,
            data: response
        })
    } catch (err) {
        res.status(404)
            .send({ message: err })
    }

}


createConcentrador = async (req,res) => {

    req.body["id_user"] = req.user.id;

    try{
        let response = await DataController.getData("INSERT INTO concentrador SET ?" , [req.body]);
        let data = {...req.body, id: response.insertId}
        res.status(200).json({
            user: req.user,
            data: data
        });
    }catch (err){
        req.status(404).send({message: err})
    }

}

module.exports = {
    getConcentradores,
    createConcentrador
}