// Register
fetch("http://localhost:4000/api/v1/auth/register", {
  body: JSON.stringify({
    first_name: "",
    last_name: "",
    email: "",
    password: "",
  }),
  headers: {
    "Content-Type": "application/json",
  },
  method: "POST",
})
  .then((res) => res.json())
  .then((data) => console.log(data));

// Verify email
fetch("http://localhost:4000/api/v1/auth/verify-email", {
  body: JSON.stringify({ token: "" }),
  headers: {
    "Content-Type": "application/json",
  },
  method: "POST",
})
  .then((res) => res.json())
  .then((data) => console.log(data));

// Login
fetch("http://localhost:4000/api/v1/auth/login", {
  body: JSON.stringify({
    email: "",
    password: "",
  }),
  headers: {
    "Content-Type": "application/json",
  },
  method: "POST",
})
  .then((res) => res.json())
  .then((data) => console.log(data));

// Adding new history log
fetch("http://localhost:4000/api/v1/vehicle-history", {
  body: JSON.stringify({
    latitude: 1,
    longitude: 1,
    person_inside_id: "asvnure",
    vehicle_status: "moving",
    vehicle_speed_in_km_hour: 70,
    battery_percentage: 70,
  }),
  headers: {
    "Content-Type": "application/json",
  },
  credentials: "include",
  method: "POST",
})
  .then((res) => res.json())
  .then((data) => console.log(data));

// Retrieving all logs for a specific user
fetch("http://localhost:4000/api/v1/vehicle-history", {
  credentials: "include",
})
  .then((res) => res.json())
  .then((data) => console.log(data));

// Logout
fetch("http://localhost:4000/api/v1/auth/logout", {
  method: "POST",
  credentials: "include",
})
  .then((res) => res.json())
  .then((data) => console.log(data));
