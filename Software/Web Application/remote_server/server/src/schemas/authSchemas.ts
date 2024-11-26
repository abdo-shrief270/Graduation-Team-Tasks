import { z } from "zod";

export const userSchema = z.object({
  first_name: z
    .string({ required_error: "هذا الحقل مطلوب" })
    .min(2, "هذا الحقل مطلوب"),
  last_name: z
    .string({ required_error: "هذا الحقل مطلوب" })
    .min(2, "هذا الحقل مطلوب"),
  email: z
    .string({ required_error: "هذا الحقل مطلوب" })
    .min(2, "هذا الحقل مطلوب")
    .email("هذا البريد الاليكتروني غير صحيح"),
  password: z
    .string({ required_error: "هذا الحقل مطلوب" })
    .min(8, "كلمة المرور يجب ألا تقل عن 8 احرف"),
});
