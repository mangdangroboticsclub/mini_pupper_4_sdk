/* ═══════════════════════════════════════════════════════════════════
 * mpx_host.h — MPX-Dog WASM Skill Host Function Reference
 *
 * Include this header in your C/C++ WASM skills to import all
 * available host functions from the "env" module.
 *
 * The ESP32 firmware registers these functions via WAMR's native
 * symbol table. Each function is callable from within the WASM
 * sandbox with no special setup required.
 *
 * Usage:
 *   #include "mpx_host.h"
 *
 *   void on_start(void) {
 *       MPX_print("Hello from my skill!");
 *       robot_gait((int)"advance");
 *       robot_delay_ms(2000);
 *       robot_gait((int)"none");
 *   }
 *
 * Host ABI v9 (MPX_ABI_VERSION 9). Every command function returns ``int``:
 * 0 on success or a negative MPX_ERR_* code. Getters return data or an error
 * as documented below. WAMR requires each import type to match the firmware
 * exactly; rebuild skills after updating this header instead of mixing objects
 * built against different ABI declarations.
 *
 * ABI v2 added direct servo-bus access. v3 added control arbitration, clock,
 * drive and foot control. v4 added overlays, ticks and traces. v5 added board
 * scaling. v6 added display, filesystem, music and robot-state functions. v7
 * added microphone capture, v8 speaker playback, and v9 HTTP requests.
 *
 * Compile with WASI SDK:
 *   /opt/wasi-sdk/bin/clang \
 *       --target=wasm32-wasip1 \
 *       -nostartfiles \
 *       -Wl,--no-entry \
 *       -Wl,--export=on_start \
 *       -Wl,--import-undefined \
 *       -I/path/to/mpx_host/dir \
 *       -o skill.wasm skill.c
 * ═══════════════════════════════════════════════════════════════════ */

#ifndef MPX_HOST_H
#define MPX_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

#define MPX_ABI_VERSION 9

/* Common return codes. Getters may instead return a non-negative value. */
#define MPX_OK              0
#define MPX_ERR_ARG        -1  /* Bad argument: id, index or pointer. */
#define MPX_ERR_NOT_LOCKED -2  /* This skill does not hold the servo bus. */
#define MPX_ERR_NO_REPLY   -3  /* The driver board did not answer. */
#define MPX_ERR_READONLY   -4  /* Calibration parameter is read-only. */
#define MPX_ERR_CANCELLED  -5  /* The skill was stopped mid-call. */
#define MPX_ERR_STATE      -6  /* Right call, wrong time or unavailable. */
#define MPX_ERR_BUSY       -7  /* Another control domain holds the joints. */

/* Control domains for mpx_control_take() and mpx_control_owner(). */
typedef enum {
    MPX_CTRL_NONE   = 0,
    MPX_CTRL_GAIT   = 1,
    MPX_CTRL_FEET   = 2,
    MPX_CTRL_JOINTS = 3,
    MPX_CTRL_BUS    = 4,
} mpx_control_domain_t;

/* Leg identifiers for mpx_foot(). */
typedef enum {
    MPX_LEG_FR = 0,
    MPX_LEG_FL = 1,
    MPX_LEG_RR = 2,
    MPX_LEG_RL = 3,
} mpx_leg_t;

/** Return the firmware host ABI version. WAMR sig: "()i". */
extern int mpx_abi_version(void);

/* ── SDK / Logging ───────────────────────────────────────────── */

/**
 * @brief Print a message to the robot's log (ESP_LOGI).
 *
 * The firmware registers this function as ``"print"`` under module ``"env"``.
 * WAMR sig: "($i)i" — the string pointer is auto-converted to a native pointer.
 *
 * @param ptr  Native pointer to the string (cast with (int))
 * @param len  String length in bytes
 */
extern int print(int ptr, int len);

/** Convenience wrapper — accepts ``const char *`` directly. */
static inline void MPX_print(const char *str, int len) {
    print((int)str, len);
}

/** Convenience macro — prints a string literal. */
#define MPX_LOG(msg)  MPX_print((msg), (int)(sizeof(msg) - 1))

/* ── High-Level Gait Control ─────────────────────────────────── */

/**
 * @brief Start a gait by name.
 *
 * Wasm import:  extern int robot_gait(int name_ptr);
 * WAMR sig:     "($)i"  — name string (auto-converted)
 *
 * Valid names:
 *   "none"       Stop all gait
 *   "init"       Return to init/stand pose
 *   "step"       Step in place
 *   "roll"       Roll body
 *   "pitch"      Pitch body
 *   "stretch"    Stretch legs
 *   "advance"    Walk forward
 *   "back"       Walk backward
 *   "left"       Sidestep left
 *   "right"      Sidestep right
 *   "turnL"      Turn left
 *   "turnR"      Turn right
 *   "twerk"      Twerk!
 *   "jump"       Jump
 *   "jumpfwd"    Jump forward
 *   "testspeed"  Speed test
 *   "lookup"     Look up (gait-based)
 *   "lookdown"   Look down (gait-based)
 *   "lookleft"   Look left (gait-based)
 *   "lookright"  Look right (gait-based)
 *   "lookul"     Look upper-left
 *   "lookur"     Look upper-right
 *   "lookll"     Look lower-left
 *   "looklr"     Look lower-right
 *   "flegL"      Foreleg lift left
 *   "flegR"      Foreleg lift right
 *   "blegL"      Backleg lift left
 *   "blegR"      Backleg lift right
 *   "heightup"   Height up
 *   "heightdown" Height down
 *   "balance"    Balance body
 *   "bowback"    Bow backward
 *   "bodycycle"  Cycle body motion
 *   "headellipse" Head ellipse motion
 *   "moveLF"     Move left front leg
 *   "moveRF"     Move right front leg
 *   "moveLB"     Move left back leg
 *   "moveRB"     Move right back leg
 *   "stanford"   Stanford trot
 *   "frontkick"  Front kick (auto-return)
 *   "wiggle"     Rear-up tail wiggle
 *   "buttshrug"  Front-up butt shrug
 *   "wiggleL" / "wiggleR"       One-sided wiggle
 *   "buttshrugL" / "buttshrugR" One-sided butt shrug
 */
extern int robot_gait(int name_ptr);

/**
 * @brief Get current gait mode as integer.
 *
 * Wasm import:  extern int robot_get_mode(void);
 * WAMR sig:     "()i"
 *
 * @return Gait mode: 0=None, 1=Init, 2=Step, ...
 */
extern int robot_get_mode(void);

/**
 * @brief Hold a body attitude using Stanford IK.
 *
 * Angles are in degrees and firmware-clamped to safe limits:
 * roll +/-25, pitch +/-20, yaw +/-30.
 * WAMR sig: "(fff)i"
 */
extern int robot_set_body_pose(float roll_deg, float pitch_deg, float yaw_deg);

/**
 * @brief Set the roll/pitch/yaw slew speed in degrees per second.
 *
 * Wasm import:  extern int robot_set_attitude_speed(int dps);
 * WAMR sig:     "(i)i"
 *
 * 0 (default) = instant: poses snap straight to the target.
 * >0 makes robot_roll/pitch/yaw/attitude GLIDE to the target at this speed,
 * so repeated pose updates ease smoothly instead of jumping. Persists until
 * changed, so set it once near the start of your skill.
 */
extern int robot_set_attitude_speed(int dps);
/**
 * @brief Per-axis roll/pitch/yaw slew speed in degrees per second.
 *
 * Wasm import:  extern int robot_set_attitude_speed_xyz(int roll_dps,
 *                   int pitch_dps, int yaw_dps);
 * WAMR sig:     "(iii)i"
 *
 * Like robot_set_attitude_speed() but each axis has its own speed, so e.g.
 * yaw can glide slowly while roll/pitch stay instant. 0 on an axis = snap.
 */
extern int robot_set_attitude_speed_xyz(int roll_dps, int pitch_dps, int yaw_dps);

/* ── Configuration ───────────────────────────────────────────── */

/**
 * @brief Set all gait parameters at once.
 *
 * Wasm import:  extern int robot_set_config(int period, int height,
 *                   int up_height, int stride, int tilt);
 * WAMR sig:     "(iiiii)i"
 *
 * @param period     Gait period in milliseconds (e.g. 100)
 * @param height     Body height in millimeters (e.g. 70)
 * @param up_height  Foot lift height in millimeters (e.g. 10)
 * @param stride     Stride length in millimeters (e.g. 10)
 * @param tilt       Max tilt angle in degrees (e.g. 10)
 */
extern int robot_set_config(int period, int height,
                             int up_height, int stride, int tilt);

/** @brief Get gait period in ms.  WAMR sig: "()i" */
extern int robot_get_period(void);

/** @brief Get body height in mm.  WAMR sig: "()i" */
extern int robot_get_height(void);

/** @brief Get foot lift height in mm.  WAMR sig: "()i" */
extern int robot_get_up_height(void);

/** @brief Get stride length in mm.  WAMR sig: "()i" */
extern int robot_get_stride(void);

/** @brief Get max tilt angle in degrees.  WAMR sig: "()i" */
extern int robot_get_tilt(void);

/* ── Low-Level Servo Control ─────────────────────────────────── */

/**
 * @brief Set a servo's target angle.
 *
 * Wasm import:  extern int robot_set_servo_angle(int id, int centideg);
 * WAMR sig:     "(ii)i"
 *
 * @param id        Servo ID (1-12)
 * @param centideg  Target angle in centidegrees (e.g. 4500 = 45.00°)
 */
extern int robot_set_servo_angle(int id, int centideg);

/**
 * @brief Send all buffered servo commands to the bus.
 * WAMR sig:     "()i"
 */
extern int robot_flush(void);

/**
 * @brief Set servo movement speed.
 *
 * Wasm import:  extern int robot_set_servo_speed(int id, int speed);
 * WAMR sig:     "(ii)i"
 *
 * @param id     Servo ID (1-12)
 * @param speed  0 = max speed, higher values = slower
 */
extern int robot_set_servo_speed(int id, int speed);

/**
 * @brief Set a servo's current limit (the hardware's torque control).
 * Clamped to 0..1500 mA and persisted across reboots. WAMR sig: "(ii)i".
 */
extern int robot_set_servo_current(int id, int milliamps);

/** @brief Get the current limit in mA, or 0 for a bad id. WAMR sig: "(i)i". */
extern int robot_get_servo_current(int id);

/**
 * @brief Read a servo's current position.
 *
 * Wasm import:  extern int robot_read_position(int id);
 * WAMR sig:     "(i)i"
 *
 * @param  id  Servo ID (1-12)
 * @return Raw position (0-1023), or -1 on error
 */
extern int robot_read_position(int id);

/**
 * @brief Read measured angle in signed centidegrees relative to centre.
 * Uses the same coordinate frame as robot_set_servo_angle(), unlike
 * robot_read_position(). Returns INT32_MIN for a bad id. WAMR sig: "(i)i".
 */
extern int robot_read_angle_cdeg(int id);

/**
 * @brief Read a servo's current speed.
 *
 * Wasm import:  extern int robot_read_speed(int id);
 * WAMR sig:     "(i)i"
 *
 * @param  id  Servo ID (1-12)
 * @return Signed speed value, or -1 on error
 */
extern int robot_read_speed(int id);

/**
 * @brief Read a servo's current load.
 *
 * Wasm import:  extern int robot_read_load(int id);
 * WAMR sig:     "(i)i"
 *
 * @param  id  Servo ID (1-12)
 * @return Signed load value, or -1 on error
 */
extern int robot_read_load(int id);

/**
 * @brief Read a servo's voltage.
 *
 * Wasm import:  extern int robot_read_voltage(int id);
 * WAMR sig:     "(i)i"
 *
 * @param  id  Servo ID (1-12)
 * @return Voltage in 0.1V units, or -1 on error
 */
extern int robot_read_voltage(int id);

/**
 * @brief Read a servo's temperature.
 *
 * Wasm import:  extern int robot_read_temperature(int id);
 * WAMR sig:     "(i)i"
 *
 * @param  id  Servo ID (1-12)
 * @return Temperature in °C, or -1 on error
 */
extern int robot_read_temperature(int id);

/**
 * @brief Read a servo's moving status.
 *
 * Wasm import:  extern int robot_read_moving(int id);
 * WAMR sig:     "(i)i"
 *
 * @param  id  Servo ID (1-12)
 * @return 0=stopped, 1=moving, or -1 on error
 */
extern int robot_read_moving(int id);

/**
 * @brief Read a servo's current draw.
 *
 * Wasm import:  extern int robot_read_current(int id);
 * WAMR sig:     "(i)i"
 *
 * @param  id  Servo ID (1-12)
 * @return Signed current in mA, or -1 on error
 */
extern int robot_read_current(int id);

/* ── Calibration ─────────────────────────────────────────────── */

/**
 * @brief Set angular offset for a servo.
 *
 * Wasm import:  extern int robot_set_offset(int id, int centideg);
 * WAMR sig:     "(ii)i"
 *
 * @param id        Servo ID (1-12)
 * @param centideg  Offset in centidegrees (e.g. 150 = 1.50°)
 */
extern int robot_set_offset(int id, int centideg);

/**
 * @brief Get angular offset for a servo.
 *
 * Wasm import:  extern int robot_get_offset(int id);
 * WAMR sig:     "(i)i"
 *
 * @param  id  Servo ID (1-12)
 * @return Offset in centidegrees
 */
extern int robot_get_offset(int id);

/**
 * @brief Ping a servo to check connectivity.
 *
 * Wasm import:  extern int robot_ping_servo(int id);
 * WAMR sig:     "(i)i"
 *
 * @param  id  Servo ID (1-12)
 * @return Servo model number (>0) on success, ≤0 on failure
 */
extern int robot_ping_servo(int id);

/* ── Utility ─────────────────────────────────────────────────── */

/**
 * @brief Block the WASM thread for a real-time delay.
 *
 * Wasm import:  extern int robot_delay_ms(int ms);
 * WAMR sig:     "(i)i"
 *
 * IMPORTANT: This is the ONLY reliable way to pause between gait
 * commands from a WASM skill. Pure-WASM busy-loops run at near-zero
 * wall time inside the interpreter and won't produce real delays.
 *
 * @param ms  Milliseconds to block (uses vTaskDelay internally)
 */
extern int robot_delay_ms(int ms);

/* ── Inverse Kinematics (per-leg) ──────────────────────────────── */

/**
 * @brief Front-right leg IK target.
 *
 * Wasm import:  extern int robot_ik_fr(float x, float th0, float z);
 * WAMR sig:     "(fff)i"
 *
 * @param x    Forward/backward position in mm
 * @param th0  Hip rotation angle in degrees
 * @param z    Height in mm
 */
extern int robot_ik_fr(float x, float th0, float z);

/**
 * @brief Front-left leg IK target.
 *
 * Wasm import:  extern int robot_ik_fl(float x, float th0, float z);
 * WAMR sig:     "(fff)i"
 */
extern int robot_ik_fl(float x, float th0, float z);

/**
 * @brief Rear-right leg IK target.
 *
 * Wasm import:  extern int robot_ik_rr(float x, float th0, float z);
 * WAMR sig:     "(fff)i"
 */
extern int robot_ik_rr(float x, float th0, float z);

/**
 * @brief Rear-left leg IK target.
 *
 * Wasm import:  extern int robot_ik_rl(float x, float th0, float z);
 * WAMR sig:     "(fff)i"
 */
extern int robot_ik_rl(float x, float th0, float z);

/* ── IMU ───────────────────────────────────────────────────────── */

/**
 * @brief Read IMU 6-DOF data into a WASM buffer.
 *
 * Wasm import:  extern int robot_imu_read(int buffer_ptr);
 * WAMR sig:     "(i)i"
 *
 * Buffer must be at least 6 × 4 = 24 bytes. Layout:
 *   float[0] = ax (accel X, g)
 *   float[1] = ay (accel Y, g)
 *   float[2] = az (accel Z, g)
 *   float[3] = gx (gyro X, dps)
 *   float[4] = gy (gyro Y, dps)
 *   float[5] = gz (gyro Z, dps)
 */
extern int robot_imu_read(int buffer_ptr);

/**
 * @brief Print the latest IMU data to the robot's log.
 *
 * Wasm import:  extern int robot_imu_print(void);
 * WAMR sig:     "()i"
 */
extern int robot_imu_print(void);

/* ── Direct servo bus (ABI v2; requires servo_lock) ────────────── */

/** Park the gait and take ownership of the servo bus. WAMR sig: "()i". */
extern int servo_lock(void);
/** Release the servo bus. WAMR sig: "()i". */
extern int servo_unlock(void);
/** Return whether this skill holds the bus (not whether anyone does). */
extern int servo_is_locked(void);
/** Set a servo gain; calibration parameters return MPX_ERR_READONLY. */
extern int servo_set_gain(int id, int param, float value);
/** Write the requested gain to the float at out_ptr. */
extern int servo_get_gain(int id, int param, int out_ptr);
/** Save servo configuration; id 0 means all boards. */
extern int servo_save_config(int id);
/** Restore servo configuration in RAM. */
extern int servo_restore_config(int id);
/** Stage one servo shadow without touching the bus. WAMR sig: "(iffff)i". */
extern int servo_stage(int id, float q_deg, float tau_ma, float kp, float kd);
/** Commit staged values to all four boards. */
extern int servo_commit(void);
/** Stage and commit count records of {q_deg, tau_ma, kp, kd} floats. */
extern int servo_write_all(int cmd_ptr, int count);
/** Write four floats to out_ptr: {q_deg, tau_ma, temp_c, q_raw}. */
extern int servo_read(int id, int out_ptr);
/** Write 12 x 4 floats in servo_read() layout to out_ptr. */
extern int servo_read_all(int out_ptr);
/** Refresh the servo feedback cache. */
extern int servo_poll(void);
/** Direct mode: 0 idle, 1 position, 2 torque. WAMR sig: "(iiff)i". */
extern int servo_direct(int id, int mode, float q_deg, float tau_ma);
/** Return a connected-servo bitmask; bit 0 represents servo 1. */
extern int servo_scan(void);

/* ── Control, clock and motion (ABI v3) ────────────────────────── */

/** Claim a MPX_CTRL_* domain; returns MPX_ERR_BUSY if already claimed. */
extern int mpx_control_take(int domain);
/** Release this skill's control-domain claim. */
extern int mpx_control_release(void);
/** Return the current MPX_CTRL_* owner. */
extern int mpx_control_owner(void);
/** Milliseconds since this skill run started, not since boot. */
extern int mpx_millis(void);
/** Sleep until an absolute mpx_millis() deadline, avoiding timing drift. */
extern int mpx_sleep_until(int t_ms);
/** Set continuous forward, strafe and turn drive. WAMR sig: "(fff)i". */
extern int mpx_drive(float fwd, float strafe, float turn);
/** Stop continuous drive. */
extern int mpx_drive_stop(void);
/** Set walk speed in mm/s, clamped to 10..200. */
extern int mpx_set_walk_speed(int mm_s);
/** Get walk speed in mm/s. */
extern int mpx_get_walk_speed(void);
/** Place a MPX_LEG_* foot; z is down-positive from the hip (about +70 standing). */
extern int mpx_foot(int leg, float x, float th0, float z);
/** Store the speed used for subsequent servo writes; does not send it. */
extern int mpx_set_all_servo_speed(int speed);
/** Zero all twelve servo offsets in NVS. */
extern int mpx_reset_offsets(void);
/** Read fractional temperature in degrees C. WAMR sig: "(i)f". */
extern float mpx_read_temperature_c(int id);

/* ── Board parameters (ABI v3-v5) ──────────────────────────────── */

/** Board travel x10 in deci-degrees, or 0 if no board answered. */
extern float mpx_board_scale_dd(void);
/** Read float parameter by string offset, returning fallback if absent. */
extern float mpx_param_f(int name_ptr, float fallback);
/** Read integer parameter by string offset, returning fallback if absent. */
extern int mpx_param_i(int name_ptr, int fallback);

/* ── Overlay, tick and trace (ABI v4) ──────────────────────────── */

/** Add a clamped degree offset to a gait-generated servo frame. */
extern int mpx_overlay(int id, float deg);
/** Get a servo's current overlay offset in degrees. */
extern float mpx_overlay_get(int id);
/** Clear all servo overlays. */
extern int mpx_overlay_clear(void);
/** Run on_tick every period_ms, clamped to 10..1000; export on_tick in the skill. */
extern int mpx_tick_every(int period_ms);
/** Stop periodic on_tick calls. */
extern int mpx_tick_stop(void);
/** Emit a named numeric trace; name_ptr is a NUL-terminated string offset. */
extern int mpx_trace(int name_ptr, float value);

/* ── Display canvas (ABI v6) ───────────────────────────────────── */
/* Colours are 0xRRGGBB; coordinates start at the top left. Paint, then call
 * display_show(). Robots without a panel return MPX_ERR_STATE. */
extern int display_width(void);
extern int display_height(void);
/** Pack 8-bit red, green and blue channels into 0xRRGGBB. */
extern int display_rgb(int r, int g, int b);
extern int display_clear(int color);
extern int display_pixel(int x, int y, int color);
/** radius -1 draws a pill; border 1 draws only the outline. */
extern int display_rect(int x, int y, int w, int h, int color,
                        int radius, int border);
extern int display_circle(int cx, int cy, int r, int color, int border);
extern int display_line(int x1, int y1, int x2, int y2, int color, int width);
/** Angles are clockwise with 0 degrees at 3 o'clock. */
extern int display_arc(int cx, int cy, int r, int start_deg, int end_deg,
                       int color, int width);
/** size 14/28/48 (0 = 14); align -1/0/1 = left/centre/right; max_w 0 = auto. */
extern int display_text(int x, int y, int text_ptr, int color,
                        int size, int align, int max_w);
/** Blit w*h*2 bytes of little-endian RGB565 data. */
extern int display_blit(int x, int y, int w, int h, int data_ptr, int len);
/** Show the canvas; hold_ms 0 means until something else takes the screen. */
extern int display_show(int hold_ms);
/** Restore the robot face immediately. */
extern int display_face(void);
/** Show one centred line of NUL-terminated text. */
extern int display_show_text(int text_ptr);
/** Show a named emotion; unknown names return MPX_ERR_ARG. */
extern int display_show_emotion(int name_ptr);
/** Set face colours; -1 leaves that colour unchanged. */
extern int display_set_colors(int background_rgb, int eyes_rgb);
/** Set display brightness, clamped to 0..100. */
extern int display_set_brightness(int percent);

/* ── LittleFS filesystem (ABI v6) ──────────────────────────────── */
/* Paths are absolute inside LittleFS, for example "/data.txt". */
/** Copy into buf; return whole file size (a value > cap means truncation). */
extern int fs_read(int path_ptr, int buf_ptr, int cap);
/** Write after phone-app user approval; may wait up to 60 seconds. */
extern int fs_write(int path_ptr, int data_ptr, int len);
/** Delete after phone-app user approval. */
extern int fs_delete(int path_ptr);
/** Return 1 if path exists, otherwise 0. */
extern int fs_exists(int path_ptr);
/** List lines as "name<TAB>size<TAB>d|f\n"; MPX_ERR_ARG if cap is too small. */
extern int fs_list(int dir_ptr, int buf_ptr, int cap);
/** Write total and used bytes to an int[2] at out_ptr. */
extern int fs_info(int out_ptr);

/* ── Music and robot state (ABI v6) ────────────────────────────── */
/** Play a genre/mood or HTTP(S) URL; optional title_ptr receives its title. */
extern int music_play(int what_ptr, int title_ptr, int cap);
extern int music_stop(void);
/** Write period, height, up_height, stride, tilt and sg_speed to an int[6]. */
extern int robot_get_config(int out_ptr);
/** Copy current movement name to buf; MPX_ERR_ARG if it does not fit. */
extern int robot_get_gait(int buf_ptr, int cap);

/* ── Microphone (ABI v7) ───────────────────────────────────────── */
/** Open mono capture; sample_rate 0 selects 16 kHz. A second active call returns MPX_ERR_BUSY. */
extern int mpx_mic_start(int sample_rate);
/** Read up to frames mono int16 samples; maximum 1920 frames per call. */
extern int mpx_mic_read(int pcm_ptr, int frames);
/** Close capture; idempotent and automatically called at run exit. */
extern int mpx_mic_stop(void);

/* ── Speaker audio (ABI v8) ────────────────────────────────────── */
/** Synchronously play a 16-bit PCM mono/stereo WAV from LittleFS. */
extern int mpx_audio_play_file(int path_ptr);
/** Queue asynchronous HTTP(S) audio streaming; use music_stop() to stop. */
extern int mpx_audio_stream_url(int url_ptr);

/* ── HTTP(S) APIs (ABI v9) ─────────────────────────────────────── */
/** Blocking GET, 15 s timeout; NUL-terminate response and return HTTP status. */
extern int mpx_http_get(int url_ptr, int response_ptr, int cap);
/** Blocking JSON POST; body_ptr 0 sends an empty request. */
extern int mpx_http_post(int url_ptr, int body_ptr, int response_ptr, int cap);

/* ═══════════════════════════════════════════════════════════════════
 *  PART 2 — High-Level Abstractions
 *
 *  Enums, structs, and inline helpers that make skill development
 *  easier.  They compile down to the Part 1 host functions — no
 *  firmware changes needed.
 *
 *  Usage:
 *      robot_stand();
 *      robot_walk_forward(3000);
 *      robot_turn_left(1500);
 *      robot_jump();
 *      robot_apply_pose((robot_pose_t){ .fr_shoulder = -30, ... });
 *      MPX_print_int(42);
 * ═══════════════════════════════════════════════════════════════════ */

// ──── 1. Gait enum (no more string typos) ─────────────────────

typedef enum {
    GAIT_NONE         = 0,
    GAIT_INIT         = 1,
    GAIT_STEP         = 2,
    GAIT_ROLL         = 3,
    GAIT_PITCH        = 4,
    GAIT_STRETCH      = 5,
    GAIT_ADVANCE      = 6,
    GAIT_BACK         = 7,
    GAIT_LEFT         = 8,
    GAIT_RIGHT        = 9,
    GAIT_TURN_L       = 10,
    GAIT_TURN_R       = 11,
    GAIT_TWERK        = 12,
    GAIT_JUMP         = 13,
    GAIT_JUMP_FWD     = 14,
    GAIT_TEST_SPD     = 15,
    GAIT_LOOK_UP      = 16,
    GAIT_LOOK_DOWN    = 17,
    GAIT_LOOK_LEFT    = 18,
    GAIT_LOOK_RIGHT   = 19,
    GAIT_LOOK_UL      = 20,
    GAIT_LOOK_UR      = 21,
    GAIT_LOOK_LL      = 22,
    GAIT_LOOK_LR      = 23,
    GAIT_FORELEG_LIFT_L = 24,
    GAIT_FORELEG_LIFT_R = 25,
    GAIT_BACKLEG_LIFT_L = 26,
    GAIT_BACKLEG_LIFT_R = 27,
    GAIT_HEIGHT_UP    = 28,
    GAIT_HEIGHT_DOWN  = 29,
    GAIT_BALANCE      = 30,
    GAIT_BOW_BACK     = 31,
    GAIT_BODY_CYCLE   = 32,
    GAIT_HEAD_ELLIPSE = 33,
    GAIT_MOVE_LF      = 34,
    GAIT_MOVE_RF      = 35,
    GAIT_MOVE_LB      = 36,
    GAIT_MOVE_RB      = 37,
    GAIT_STANFORD     = 38,
    GAIT_FRONT_KICK   = 39,
    GAIT_WIGGLE       = 40,
    GAIT_BUTT_SHRUG   = 41,
    GAIT_WIGGLE_L     = 42,
    GAIT_WIGGLE_R     = 43,
    GAIT_BUTT_SHRUG_L = 44,
    GAIT_BUTT_SHRUG_R = 45,
} robot_gait_t;

/**
 * @brief Start a gait using the type-safe enum.
 *
 * Internally maps enum → string → robot_gait().  No more
 * typos like "advnce" silently failing.
 */
static inline void robot_gait_enum(robot_gait_t g) {
    /* String lookup table in WASM linear memory.
     * Must match the order of robot::GaitCmd on the firmware side. */
    static const char *names[] = {
        "none",      /* 0  GAIT_NONE         */
        "init",      /* 1  GAIT_INIT         */
        "step",      /* 2  GAIT_STEP         */
        "roll",      /* 3  GAIT_ROLL         */
        "pitch",     /* 4  GAIT_PITCH        */
        "stretch",   /* 5  GAIT_STRETCH      */
        "advance",   /* 6  GAIT_ADVANCE      */
        "back",      /* 7  GAIT_BACK         */
        "left",      /* 8  GAIT_LEFT         */
        "right",     /* 9  GAIT_RIGHT        */
        "turnL",     /* 10 GAIT_TURN_L       */
        "turnR",     /* 11 GAIT_TURN_R       */
        "twerk",     /* 12 GAIT_TWERK        */
        "jump",      /* 13 GAIT_JUMP         */
        "jumpfwd",   /* 14 GAIT_JUMP_FWD     */
        "testspeed", /* 15 GAIT_TEST_SPD     */
        "lookup",    /* 16 GAIT_LOOK_UP      */
        "lookdown",  /* 17 GAIT_LOOK_DOWN    */
        "lookleft",  /* 18 GAIT_LOOK_LEFT    */
        "lookright", /* 19 GAIT_LOOK_RIGHT   */
        "lookul",    /* 20 GAIT_LOOK_UL      */
        "lookur",    /* 21 GAIT_LOOK_UR      */
        "lookll",    /* 22 GAIT_LOOK_LL      */
        "looklr",    /* 23 GAIT_LOOK_LR      */
        "flegL",     /* 24 GAIT_FORELEG_LIFT_L */
        "flegR",     /* 25 GAIT_FORELEG_LIFT_R */
        "blegL",     /* 26 GAIT_BACKLEG_LIFT_L */
        "blegR",     /* 27 GAIT_BACKLEG_LIFT_R */
        "heightup",  /* 28 GAIT_HEIGHT_UP    */
        "heightdown",/* 29 GAIT_HEIGHT_DOWN  */
        "balance",   /* 30 GAIT_BALANCE      */
        "bowback",   /* 31 GAIT_BOW_BACK     */
        "bodycycle", /* 32 GAIT_BODY_CYCLE   */
        "headellipse",/* 33 GAIT_HEAD_ELLIPSE */
        "moveLF",    /* 34 GAIT_MOVE_LF      */
        "moveRF",    /* 35 GAIT_MOVE_RF      */
        "moveLB",    /* 36 GAIT_MOVE_LB      */
        "moveRB",    /* 37 GAIT_MOVE_RB      */
        "stanford",  /* 38 GAIT_STANFORD     */
        "frontkick", /* 39 GAIT_FRONT_KICK   */
        "wiggle",    /* 40 GAIT_WIGGLE       */
        "buttshrug", /* 41 GAIT_BUTT_SHRUG   */
        "wiggleL",   /* 42 GAIT_WIGGLE_L     */
        "wiggleR",   /* 43 GAIT_WIGGLE_R     */
        "buttshrugL",/* 44 GAIT_BUTT_SHRUG_L */
        "buttshrugR",/* 45 GAIT_BUTT_SHRUG_R */
    };
    if (g >= GAIT_NONE && g <= GAIT_BUTT_SHRUG_R) {
        robot_gait((int)names[(int)g]);
    }
}

// ──── 2. Named servo IDs (no more magic numbers) ──────────────

typedef enum {
    /* Front Right leg */
    SERVO_FR_HIP      = 1,
    SERVO_FR_SHOULDER = 2,
    SERVO_FR_KNEE     = 3,
    /* Front Left leg */
    SERVO_FL_HIP      = 4,
    SERVO_FL_SHOULDER = 5,
    SERVO_FL_KNEE     = 6,
    /* Rear Right leg */
    SERVO_RR_HIP      = 7,
    SERVO_RR_SHOULDER = 8,
    SERVO_RR_KNEE     = 9,
    /* Rear Left leg */
    SERVO_RL_HIP      = 10,
    SERVO_RL_SHOULDER = 11,
    SERVO_RL_KNEE     = 12,
} robot_servo_t;

// ──── 3. Degree-based servo control (no more centidegree math) ─

/** Set servo angle in degrees (auto-converts to centidegrees). */
static inline void robot_set_servo_deg(robot_servo_t id, float deg) {
    robot_set_servo_angle((int)id, (int)(deg * 100.0f));
}

/** Set servo angle in raw 0-1023 units (maps 0°→0, 300°→1023). */
static inline void robot_set_servo_raw(robot_servo_t id, int raw) {
    robot_set_servo_angle((int)id, (raw * 30000) / 1023);
}

/** Set servo angle + speed in one call. */
static inline void robot_set_servo(robot_servo_t id, float deg, int speed) {
    robot_set_servo_speed((int)id, speed);
    robot_set_servo_deg(id, deg);
}

// ──── 4. Config struct (no more flat 5-arg calls) ────────────

typedef struct {
    int period;      /**< Gait period in ms per phase */
    int height;      /**< Body height in mm */
    int up_height;   /**< Foot lift height in mm */
    int stride;      /**< Stride length in mm */
    int tilt;        /**< Max body tilt in degrees */
} robot_config_t;

/** Set config from a struct. */
static inline void robot_set_config_ex(robot_config_t cfg) {
    robot_set_config(cfg.period, cfg.height,
                     cfg.up_height, cfg.stride, cfg.tilt);
}

/** Get current config as a struct. */
static inline robot_config_t robot_get_config_ex(void) {
    robot_config_t c;
    c.period    = robot_get_period();
    c.height    = robot_get_height();
    c.up_height = robot_get_up_height();
    c.stride    = robot_get_stride();
    c.tilt      = robot_get_tilt();
    return c;
}

// ──── 5. Choreography helpers (one-liner actions) ────────────

/** Walk forward for N ms, then stop. */
static inline void robot_walk_forward(int ms) {
    robot_gait_enum(GAIT_ADVANCE);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Walk backward for N ms, then stop. */
static inline void robot_walk_backward(int ms) {
    robot_gait_enum(GAIT_BACK);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Turn left (spin) for N ms, then stop. */
static inline void robot_turn_left(int ms) {
    robot_gait_enum(GAIT_TURN_L);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Turn right (spin) for N ms, then stop. */
static inline void robot_turn_right(int ms) {
    robot_gait_enum(GAIT_TURN_R);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Strafe left for N ms, then stop. */
static inline void robot_strafe_left(int ms) {
    robot_gait_enum(GAIT_LEFT);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Strafe right for N ms, then stop. */
static inline void robot_strafe_right(int ms) {
    robot_gait_enum(GAIT_RIGHT);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Perform a single jump. */
static inline void robot_jump(void) {
    robot_gait_enum(GAIT_JUMP);
    robot_delay_ms(2000);
    robot_gait_enum(GAIT_NONE);
}

/** Stand to init pose (blocks ~2 s). */
static inline void robot_stand(void) {
    robot_gait_enum(GAIT_INIT);
    robot_delay_ms(2000);
}

/** Do a fun dance for N ms. */
static inline void robot_dance(int ms) {
    robot_set_config(60, 60, 15, 8, 15);
    robot_gait_enum(GAIT_TWERK);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Step in place for N ms, then stop. */
static inline void robot_step_in_place(int ms) {
    robot_gait_enum(GAIT_STEP);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Look up for N ms, using gait-based head control. */
static inline void robot_look_up(int ms) {
    robot_gait_enum(GAIT_LOOK_UP);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Look down for N ms, using gait-based head control. */
static inline void robot_look_down(int ms) {
    robot_gait_enum(GAIT_LOOK_DOWN);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Look left for N ms, using gait-based head control. */
static inline void robot_look_left(int ms) {
    robot_gait_enum(GAIT_LOOK_LEFT);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Look right for N ms, using gait-based head control. */
static inline void robot_look_right(int ms) {
    robot_gait_enum(GAIT_LOOK_RIGHT);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Look upper-left for N ms, using gait-based head control. */
static inline void robot_look_upper_left(int ms) {
    robot_gait_enum(GAIT_LOOK_UL);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Look upper-right for N ms, using gait-based head control. */
static inline void robot_look_upper_right(int ms) {
    robot_gait_enum(GAIT_LOOK_UR);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Look lower-left for N ms, using gait-based head control. */
static inline void robot_look_lower_left(int ms) {
    robot_gait_enum(GAIT_LOOK_LL);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Look lower-right for N ms, using gait-based head control. */
static inline void robot_look_lower_right(int ms) {
    robot_gait_enum(GAIT_LOOK_LR);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Lift left foreleg for N ms. */
static inline void robot_foreleg_lift_left(int ms) {
    robot_gait_enum(GAIT_FORELEG_LIFT_L);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Lift right foreleg for N ms. */
static inline void robot_foreleg_lift_right(int ms) {
    robot_gait_enum(GAIT_FORELEG_LIFT_R);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Lift left back leg for N ms. */
static inline void robot_backleg_lift_left(int ms) {
    robot_gait_enum(GAIT_BACKLEG_LIFT_L);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Lift right back leg for N ms. */
static inline void robot_backleg_lift_right(int ms) {
    robot_gait_enum(GAIT_BACKLEG_LIFT_R);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Raise body height for N ms. */
static inline void robot_height_up(int ms) {
    robot_gait_enum(GAIT_HEIGHT_UP);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Lower body height for N ms. */
static inline void robot_height_down(int ms) {
    robot_gait_enum(GAIT_HEIGHT_DOWN);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Balance on the spot for N ms. */
static inline void robot_balance(int ms) {
    robot_gait_enum(GAIT_BALANCE);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Bow backward for N ms. */
static inline void robot_bow_back(int ms) {
    robot_gait_enum(GAIT_BOW_BACK);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Cycle body for N ms. */
static inline void robot_body_cycle(int ms) {
    robot_gait_enum(GAIT_BODY_CYCLE);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Head ellipse motion for N ms. */
static inline void robot_head_ellipse(int ms) {
    robot_gait_enum(GAIT_HEAD_ELLIPSE);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Move left front leg for N ms. */
static inline void robot_move_lf(int ms) {
    robot_gait_enum(GAIT_MOVE_LF);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Move right front leg for N ms. */
static inline void robot_move_rf(int ms) {
    robot_gait_enum(GAIT_MOVE_RF);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Move left back leg for N ms. */
static inline void robot_move_lb(int ms) {
    robot_gait_enum(GAIT_MOVE_LB);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Move right back leg for N ms. */
static inline void robot_move_rb(int ms) {
    robot_gait_enum(GAIT_MOVE_RB);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Walk using the Stanford trot for N ms. */
static inline void robot_stanford_walk(int ms) {
    robot_gait_enum(GAIT_STANFORD);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Perform one front kick. */
static inline void robot_front_kick(void) {
    robot_gait_enum(GAIT_FRONT_KICK);
    robot_delay_ms(2000);
}

/** Wiggle the raised rear for N ms. */
static inline void robot_wiggle(int ms) {
    robot_gait_enum(GAIT_WIGGLE);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Perform the distinct front-up butt shrug for N ms. */
static inline void robot_butt_shrug(int ms) {
    robot_gait_enum(GAIT_BUTT_SHRUG);
    robot_delay_ms(ms);
    robot_gait_enum(GAIT_NONE);
}

/** Hold any roll/pitch/yaw combination; the next movement interrupts it. */
static inline void robot_attitude(float roll_deg, float pitch_deg, float yaw_deg) {
    robot_set_body_pose(roll_deg, pitch_deg, yaw_deg);
}

/** Angle-only body controls, matching the reference movement API. */
static inline void robot_roll(float angle_deg)  { robot_set_body_pose(angle_deg, 0.0f, 0.0f); }
static inline void robot_pitch(float angle_deg) { robot_set_body_pose(0.0f, angle_deg, 0.0f); }
static inline void robot_yaw(float angle_deg)   { robot_set_body_pose(0.0f, 0.0f, angle_deg); }
static inline void robot_reset_attitude(void)   { robot_gait((int)"none"); }

/** Set the roll/pitch/yaw glide speed in degrees/second (0 = instant snap).
 *  Float-friendly wrapper around robot_set_attitude_speed(). */
static inline void robot_attitude_speed(float dps) { robot_set_attitude_speed((int)dps); }

/** Per-axis glide speed in degrees/second (0 on an axis = instant snap). */
static inline void robot_attitude_speed_xyz(float roll_dps, float pitch_dps, float yaw_dps) {
    robot_set_attitude_speed_xyz((int)roll_dps, (int)pitch_dps, (int)yaw_dps);
}

/** Set the glide speed AND move to a pose in one call. */
static inline void robot_attitude_at(float roll_deg, float pitch_deg,
                                     float yaw_deg, float dps) {
    robot_set_attitude_speed((int)dps);
    robot_set_body_pose(roll_deg, pitch_deg, yaw_deg);
}

// ──── 6. Full pose helper (all 12 servos at once) ────────────

typedef struct {
    float fr_hip, fr_shoulder, fr_knee;
    float fl_hip, fl_shoulder, fl_knee;
    float rr_hip, rr_shoulder, rr_knee;
    float rl_hip, rl_shoulder, rl_knee;
} robot_pose_t;

/** Apply a complete pose — sets all 12 servos and flushes. */
static inline void robot_apply_pose(robot_pose_t p) {
    robot_set_servo_angle(SERVO_FR_HIP,      (int)(p.fr_hip      * 100.0f));
    robot_set_servo_angle(SERVO_FR_SHOULDER,  (int)(p.fr_shoulder * 100.0f));
    robot_set_servo_angle(SERVO_FR_KNEE,      (int)(p.fr_knee     * 100.0f));
    robot_set_servo_angle(SERVO_FL_HIP,      (int)(p.fl_hip      * 100.0f));
    robot_set_servo_angle(SERVO_FL_SHOULDER,  (int)(p.fl_shoulder * 100.0f));
    robot_set_servo_angle(SERVO_FL_KNEE,      (int)(p.fl_knee     * 100.0f));
    robot_set_servo_angle(SERVO_RR_HIP,      (int)(p.rr_hip      * 100.0f));
    robot_set_servo_angle(SERVO_RR_SHOULDER,  (int)(p.rr_shoulder * 100.0f));
    robot_set_servo_angle(SERVO_RR_KNEE,      (int)(p.rr_knee     * 100.0f));
    robot_set_servo_angle(SERVO_RL_HIP,      (int)(p.rl_hip      * 100.0f));
    robot_set_servo_angle(SERVO_RL_SHOULDER,  (int)(p.rl_shoulder * 100.0f));
    robot_set_servo_angle(SERVO_RL_KNEE,      (int)(p.rl_knee     * 100.0f));
    robot_flush();
}

// ──── 7. Integer printing (for debugging) ─────────────────────

/** Print an integer to the robot's log (handles negatives). */
static inline void MPX_print_int(int value) {
    char buf[16];
    int  pos      = 15;
    int  negative = 0;

    buf[14] = '\n';
    buf[15] = '\0';

    if (value < 0) {
        negative = 1;
        value = -value;
    }

    if (value == 0) {
        buf[--pos] = '0';
    } else {
        while (value > 0 && pos > 0) {
            buf[--pos] = '0' + (value % 10);
            value /= 10;
        }
    }

    if (negative && pos > 0) {
        buf[--pos] = '-';
    }

    print((int)(buf + pos), 15 - pos);
}

/* ═══════════════════════════════════════════════════════════════════
 *  Quick reference: see the declarations above for the full function list
 *  (raw imports in Part 1, high-level helpers in Part 2).
 * ═══════════════════════════════════════════════════════════════════ */

#ifdef __cplusplus
}
#endif

#endif /* MPX_HOST_H */
