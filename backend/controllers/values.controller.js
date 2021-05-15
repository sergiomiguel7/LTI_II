const fs = require('fs');
const path = require('path');

exports.getValues = async (req, res) => {
    try {
        let counter = 0;
        const data = fs.readFileSync(path.join(__dirname, '../../db/data.txt'), 'utf-8');
        let lines = data.toString().split('\n');
        let values = [];
        tempValues = [];
        const limit = req.query.limit ? req.query.limit : 10;
        const page = req.query.page ? req.query.page : 1;



        
        lines.forEach(element => {
                let ldr = element.split(';');
                if (ldr[5] == 'L') {
                    tempValues.push({
                        areaConcentrador: ldr[0],
                        idConcentrador: ldr[1],
                        areaSensor: ldr[2],
                        coordsSensor: ldr[3],
                        timestamp: ldr[4],
                        value: Number(ldr[6]).toFixed(3)
                    });
                }
        });

        tempValues.sort(function(x, y){
            return x.timestamp - y.timestamp;
        })


        tempValues.forEach(element => {
            if (counter > (page*limit - 1) && counter < (page*limit + Number(limit)) ) {
                values.push(element);
            }
            counter++;
        })



        res.status(200).json({
            storedValues: lines.length,
            data: values
        });
    }
    catch (err) {
        res.status(404)
            .send({message: err})
    }
}

exports.getValuesByDate = async (req, res) => {
    try {
        let counter = 0;
        const data = fs.readFileSync(path.join(__dirname, '../../db/data.txt'), 'utf-8');
        let lines = data.toString().split('\n');
        let values = [];
        const limit = req.query.limit ? req.query.limit : 10;
        const page = req.query.page ? req.query.page : 1;

        
        lines.forEach(element => {

            if (counter > (page*limit - 1) && counter < (page*limit + Number(limit)) ) {
                let ldr = element.split(';');
                if (ldr[5] == 'L') {
                    values.push({
                        counter: counter,
                        areaConcentrador: ldr[0],
                        idConcentrador: ldr[1],
                        areaSensor: ldr[2],
                        coordsSensor: ldr[3],
                        timestamp: ldr[4],
                        value: Number(ldr[6]).toFixed(3)
                    });
                }
            }
            counter++;
        });

        values.sort(function(x, y){
            return x.timestamp - y.timestamp;
        })

        res.status(200).json({
            storedValues: lines.length,
            data: values
        });
    }
    catch (err) {
        res.status(404)
            .send({message: err})
    }
}