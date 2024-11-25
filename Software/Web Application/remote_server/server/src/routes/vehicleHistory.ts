import {
  addVehicleHistoryLog,
  getVehicleHistory,
} from "@/controllers/vehicle-history";
import express from "express";

const router = express.Router();

router.get("/vehicle-history", getVehicleHistory);
router.post("/vehicle-history", addVehicleHistoryLog);

export default router;
