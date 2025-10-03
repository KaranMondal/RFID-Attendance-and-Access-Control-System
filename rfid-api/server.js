// ---------------- Import Libraries ----------------
const express = require("express");
const bodyParser = require("body-parser");
const mongoose = require("mongoose");
const cors = require("cors");

// ---------------- Express Setup ----------------
const app = express();
app.use(cors());
app.use(bodyParser.json());

// ---------------- Connect MongoDB ----------------
mongoose.connect(
  "mongodb://localhost:27017/rfidDB",  // Your local MongoDB
  { useNewUrlParser: true, useUnifiedTopology: true }
);

const db = mongoose.connection;
db.on("error", console.error.bind(console, "connection error:"));
db.once("open", () => {
  console.log("MongoDB connected!");
});

// ---------------- Define Mongoose Schema ----------------
const logSchema = new mongoose.Schema({
  user: { type: String, required: true },
  action: { type: String, enum: ['Check-In','Check-Out'], required: true },
  time: { type: String, required: true },
  distance: { type: Number, required: true }
});

const Log = mongoose.model("Log", logSchema);

// ---------------- API Routes ----------------
app.post("/log", async (req, res) => {
  try {
    const log = new Log(req.body);
    await log.save();
    console.log("Data stored:", req.body);
    res.status(200).send("Data stored in MongoDB");
  } catch (err) {
    console.error("Error:", err);
    res.status(400).send("Failed to store data");
  }
});

app.get("/logs", async (req, res) => {
  try {
    const logs = await Log.find().sort({ _id: -1 });
    res.json(logs);
  } catch (err) {
    res.status(500).send(err);
  }
});

// ---------------- Start Server ----------------
const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});
