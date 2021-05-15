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
                if (ldr[6] == 'L') {
                    tempValues.push({
                        areaConcentrador: ldr[0],
                        idConcentrador: ldr[1],
                        areaSensor: ldr[3],
                        coordsSensor: ldr[4],
                        timestamp: ldr[5],
                        value: Number(ldr[7]).toFixed(3)
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
            storedValues: tempValues.length,
            data: values
        });
    }
    catch (err) {
        res.status(404)
            .send({message: err})
    }
}
