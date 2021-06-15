const DataController = require('./data.controller');


getConcentradores = async (req, res) => {
    try {
        let response = await DataController.getData("SELECT * FROM concentrador", '');
        res.status(200).json({
            data: response
        })
    } catch (err) {
        res.status(404)
            .send({ message: err })
    }
}

getUsers = async (req, res) => {
    try {
        let response = await DataController.getData("SELECT * FROM user", '');
        res.status(200).json({
            data: response
        })
    } catch (err) {
        res.status(404)
            .send({ message: err })
    }
}



module.exports = {
    getConcentradores,
    getUsers
}