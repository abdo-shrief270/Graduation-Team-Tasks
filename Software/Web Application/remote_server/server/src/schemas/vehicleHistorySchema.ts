import z from "zod";

export const vehicleHistorySchema = z.object({
  id: z.string({ required_error: "هذا الحقل مطلوب" }).min(2, "هذا الحقل مطلوب"),
  latitude: z
    .string({ required_error: "هذا الحقل مطلوب" })
    .min(2, "هذا الحقل مطلوب"),
  longitude: z
    .string({ required_error: "هذا الحقل مطلوب" })
    .min(2, "هذا الحقل مطلوب"),
  person_inside_id: z.nullable(
    z.string({ required_error: "هذا الحقل مطلوب" }).min(2, "هذا الحقل مطلوب")
  ),
  vehicle_status: z
    .string({ required_error: "هذا الحقل مطلوب" })
    .min(2, "هذا الحقل مطلوب"),
  vehicle_speed_in_km_hour: z
    .number({ required_error: "هذا الحقل مطلوب" })
    .min(0, "هذا الحقل مطلوب"),
  battery_percentage: z
    .number({ required_error: "هذا الحقل مطلوب" })
    .min(2, "هذا الحقل مطلوب"),
  user_id: z
    .string({ required_error: "هذا الحقل مطلوب" })
    .min(2, "هذا الحقل مطلوب"),
});
