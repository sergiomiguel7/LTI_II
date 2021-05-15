module.exports = (app) => {
    app.use('/api/values', require('../routes/values'));
}