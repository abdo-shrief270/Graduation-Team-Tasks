import { login, logout, register, verifyEmail } from "@/controllers/auth";
import express from "express";

const router = express.Router();

router.post("/auth/register", register);
router.post("/auth/verify-email", verifyEmail);
router.post("/auth/login", login);
router.post("/auth/logout", logout);

export default router;
