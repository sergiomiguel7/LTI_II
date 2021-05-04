const port = process.env.PORT || 5000;
const express = require('express');
const app = express();

//middleweares
require('./middleweares/index.js')(app, express);
require('./middleweares/router')(app);

app.use('/', (req, res, next) => {
    res.json({
        message: 'it works'
    });
});

app.listen(port, () => console.log(`Server started at port ${port}`));

