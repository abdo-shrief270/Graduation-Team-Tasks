import "dotenv/config";
import express from "express";
import cors from "cors";
import { vehicleHistoryRoutes } from "@/routes";
import { errorHandler, GlobalError } from "@/middlewares";

const app = express();
const PORT = process.env.PORT || 4000;

app.listen(PORT, () => {
  console.log(`Listening on port: ${PORT}`);
});

app.use(
  cors({
    origin: process.env.FRONTEND_ORIGIN || "http://localhost:5173",
    methods: ["GET", "POST", "PUT", "DELETE"],
    allowedHeaders: ["Content-Type", "Authorization"],
  })
);
app.use(express.json());
app.use("/api/v1", vehicleHistoryRoutes);

app.get("/bing", (req, res) => {
  res.status(200).send({
    status: "success",
    message: "bong",
  });
});

app.all("*", (req, res, next) => {
  const error = new GlobalError(
    `No routes match the specified route ${req.originalUrl}`
  );
  error.statusCode = 404;
  error.status = "Not found";

  next(error);
});

app.use(errorHandler);
