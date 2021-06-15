module.exports = (app) => {
    app.use('/api/auth', require('../routes/auth'));
    app.use('/api/values', require('../routes/values'));
    app.use('/api/concentradores', require('../routes/concentradores'));
    app.use('/api/sensores', require('../routes/sensores'));
    app.use('/api/mqtt', require('./mqtt'));
}