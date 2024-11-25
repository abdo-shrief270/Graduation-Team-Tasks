import z from "zod";

export const vehicleHistorySchema = z.object({
  latitude: z.number({ required_error: "هذا الحقل مطلوب" }),
  longitude: z.number({ required_error: "هذا الحقل مطلوب" }),
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
