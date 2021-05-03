const port = process.env.PORT || 5000;
const express = require('express');
const app = express();

(app, () => {
    require('./middleweares/index')(app);
    require('./middleweares/router.js')(app);

    app.listen(port, () => console.log(`Server started at port ${port}`));
})
