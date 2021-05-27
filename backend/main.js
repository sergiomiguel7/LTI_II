const port = process.env.PORT || 5000;
const express = require('express');
const app = express();

app.use( express.static( __dirname + '/public' ));

//middleweares
require('./middleweares/index.js')(app, express);
require('./middleweares/router')(app);
require('./middleweares/auth');

app.use('/', (req, res, next) => {
    res.send({
        message: 'working..'
    })
});

app.listen(port, () => console.log(`Server started at port ${port}`));

