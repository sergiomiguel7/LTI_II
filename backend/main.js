const port = process.env.PORT || 5000;
const express = require('express');
const app = express();
require('dotenv').config();


app.use(express.static(__dirname + '/public'));

//middleweares
require('./db/connection')(app, () => {

    require('./middleweares/index.js')(app, express);
    require('./middleweares/router')(app);

    app.listen(port, () => console.log(`Server started at port ${port}`));

});

