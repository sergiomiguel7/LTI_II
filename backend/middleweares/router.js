module.exports = (app) => {
    app.use('/api/auth', require('../routes/auth'));
    app.use('/api/values', require('../routes/values'));
    app.use('/api/concentradores', require('../routes/concentradores'));
}