


async function getData (query, params) {
    let db = global.con;

    let promise = new Promise((resolve, reject) => {
        db.query(query, params, async(err, rows) => {
            if(err) reject(err);
            let parsed = JSON.parse(JSON.stringify(rows));
            resolve(parsed);
        });
    });

    return promise;
}


module.exports = {
    getData
}