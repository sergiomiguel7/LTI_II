module.exports = (app, express) => {
    const cors = require('cors');
    const helmet = require('helmet');
    const morgan = require('morgan');

    app.use(express.json());
    app.use(express.urlencoded({extended: true}));
    app.use(cors());
    app.use(morgan('dev'));
    app.use(helmet());

}