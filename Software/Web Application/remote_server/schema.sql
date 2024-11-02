CREATE TABLE
    IF NOT EXISTS users (
        id VARCHAR(26) NOT NULL,
        first_name VARCHAR(50) NOT NULL,
        last_name VARCHAR(50) NOT NULL,
        email VARCHAR(255) NOT NULL UNIQUE,
        password VARCHAR(255) NOT NULL,
        last_login DATETIME,
        created_at DATETIME DEFAULT now (),
        updated_at DATETIME DEFAULT now (),
    );

CREATE TABLE
    IF NOT EXISTS vehicle_history (
        id VARCHAR(26) NOT NULL,
        latitude VARCHAR(100) NOT NULL,
        longitude VARCHAR(100) NOT NULL,
        person_inside_id VARCHAR(50),
        vehicle_status VARCHAR(50) NOT NULL,
        vehicle_speed_in_km_hour INT NOT NULL,
        battery_percentage INT NOT NULL,
        user_id VARCHAR(26) NOT NULL,
        created_at DATETIME DEFAULT now (),
        updated_at DATETIME DEFAULT now (),
        FOREIGN KEY user_id REFERENCES users (id) ON DELETE CASCADE
    );

CREATE TABLE
    video_streams (
        id VARCHAR(26) NOT NULL,
        filename VARCHAR(255) NOT NULL,
        url VARCHAR(255) NOT NULL,
        user_id VARCHAR(26) NOT NULL,
        duration_in_millis INT NOT NULL,
        created_at DATETIME DEFAULT now (),
        updated_at DATETIME DEFAULT now (),
        FOREIGN KEY user_id REFERENCES users (id) ON DELETE CASCADE
    );