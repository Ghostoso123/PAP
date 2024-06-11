const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const { Pool } = require('pg');

require('dotenv').config();

const toSQLObject = (data) => {
  return {
    SensorLocation: data.sensor,
    DataType: data.type,
    DataValue: data.value
  };
};

const fromSQLObject = (data) => {
  return {
    sensor: data.SensorLocation,
    type: data.DataType,
    value: data.DataValue,
    timestamp: data.Timestamp
  };
};

const update = async () => {
  if (dataStore.length === 0) {
    console.log('.');
    scheduleUpdate(10);
    return;
  }

  console.log('Updating database with', dataStore.length, 'new data points');

  const valuesToInsert = dataStore.map(data => toSQLObject(data));
  const client = await pool.connect();

  try {
    await client.query('BEGIN');
    const insertQuery = `
      INSERT INTO "WeatherData" ("SensorLocation", "DataType", "DataValue") VALUES 
      ${valuesToInsert.map((_, i) => `($${3 * i + 1}, $${3 * i + 2}, $${3 * i + 3})`).join(',')}
    `;
    const values = valuesToInsert.flatMap(data => [data.SensorLocation, data.DataType, data.DataValue]);
    await client.query(insertQuery, values);
    await client.query('COMMIT');
    console.log('Data inserted successfully');
  } catch (err) {
    await client.query('ROLLBACK');
    console.error('Error inserting data:', err);
  } finally {
    client.release();
  }

  while (dataStore.length > 0) {
    dataStore.pop();
  }

  scheduleUpdate(20);
};

// in seconds
const scheduleUpdate = function(delay) {
  setTimeout(update, delay * 1000);
};

const app = express();
const port = process.env.PORT || 80;

app.use(cors());
app.use(bodyParser.json());

// Memory store
const dataStore = [];
const pool = new Pool({
  connectionString: process.env.DATABASE_URL
});


// Start the update loop
update();

app.post('/', (req, res) => {
  const data = req.body;
  if (data) {
    dataStore.push(data);
    res.status(200).send('Data received and stored.');
  } else {
    res.status(400).send('Bad Request: Missing data field in the request.');
  }
});

app.get('/', (req, res) => {
  res.send(`Welcome to Tiago's Weather Data API. Send a POST request to / with the data you want to store. or to see the data stored, send a GET request to /weather-data.`);
});

app.get('/weather-data', async (req, res) => {
  const { page = 1, limit = 10 } = req.query;
  const offset = (page - 1) * limit;
  const client = await pool.connect();

  try {
    const totalCountResult = await client.query('SELECT COUNT(*) AS total FROM "WeatherData"');
    const totalCount = parseInt(totalCountResult.rows[0].total, 10);

    const result = await client.query('SELECT * FROM "WeatherData" LIMIT $1 OFFSET $2', [limit, offset]);
    const nextPage = (offset + parseInt(limit)) < totalCount ? parseInt(page) + 1 : null;
    const nextPageLink = nextPage ? `${req.protocol}://${req.get('host')}/weather-data?page=${nextPage}&limit=${limit}` : null;
    res.json({
      data: result.rows.map(fromSQLObject),
      nextPage: nextPageLink
    });
  } catch (err) {
    console.error('Error retrieving data:', err);
    res.status(500).json({ error: 'Error retrieving data' });
  } finally {
    client.release();
  }
});

app.get('/weather-data/:type', async (req, res) => {
  const { type } = req.params;
  const { page = 1, limit = 10 } = req.query;
  const offset = (page - 1) * limit;
  const client = await pool.connect();

  try {
    const totalCountResult = await client.query('SELECT COUNT(*) AS total FROM "WeatherData" WHERE "DataType" = $1', [type]);
    const totalCount = parseInt(totalCountResult.rows[0].total, 10);

    const result = await client.query('SELECT * FROM "WeatherData" WHERE "DataType" = $1 LIMIT $2 OFFSET $3', [type, limit, offset]);
    const nextPage = (offset + parseInt(limit)) < totalCount ? parseInt(page) + 1 : null;
    const nextPageLink = nextPage ? `${req.protocol}://${req.get('host')}/weather-data/${type}?page=${nextPage}&limit=${limit}` : null;

    res.json({
      data: result.rows.map(fromSQLObject),
      nextPage: nextPageLink
    });
  } catch (err) {
    console.error('Error retrieving data:', err);
    res.status(500).json({ error: 'Error retrieving data' });
  } finally {
    client.release();
  }
});

app.listen(port, () => {
  console.log(`Server is listening on port ${port}`);
});

app.on('close', () => {
  pool.end();
});
