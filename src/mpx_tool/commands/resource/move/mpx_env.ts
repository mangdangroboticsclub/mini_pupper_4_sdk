/**
 * mpx_env.ts — MPX-Dog WASM Host Function Declarations (AssemblyScript)
 *
 * Import host functions from the "env" module into your skill:
 *
 *   import {
 *     print, robot_gait, robot_delay_ms,
 *     robot_set_config, robot_ping_servo,
 *   } from "../include/mpx_env";
 *
 * All functions are registered under the "env" module by the ESP32 firmware.
 * Host ABI v9: command functions return i32 status; getters return data or
 * a negative MPX_ERR_* value. String and buffer arguments are linear-memory
 * pointers. See mpx_host.h for detailed buffer layouts and call semantics.
 */

export const MPX_ABI_VERSION: i32 = 9;
export const MPX_OK: i32 = 0;
export const MPX_ERR_ARG: i32 = -1;
export const MPX_ERR_NOT_LOCKED: i32 = -2;
export const MPX_ERR_NO_REPLY: i32 = -3;
export const MPX_ERR_READONLY: i32 = -4;
export const MPX_ERR_CANCELLED: i32 = -5;
export const MPX_ERR_STATE: i32 = -6;
export const MPX_ERR_BUSY: i32 = -7;

// ── ABI / Logging ────────────────────────────────────────────────
@external("env", "mpx_abi_version")
export declare function mpx_abi_version(): i32;
@external("env", "print")
export declare function print(ptr: usize, len: i32): i32;

// ── High-Level Gait Control ─────────────────────────────────────
@external("env", "robot_gait")
export declare function robot_gait(name_ptr: usize): i32;
@external("env", "robot_get_mode")
export declare function robot_get_mode(): i32;
@external("env", "robot_set_body_pose")
export declare function robot_set_body_pose(roll_deg: f32, pitch_deg: f32, yaw_deg: f32): i32;
@external("env", "robot_set_attitude_speed")
export declare function robot_set_attitude_speed(dps: i32): i32;
@external("env", "robot_set_attitude_speed_xyz")
export declare function robot_set_attitude_speed_xyz(roll_dps: i32, pitch_dps: i32, yaw_dps: i32): i32;

// ── Configuration ───────────────────────────────────────────────
@external("env", "robot_set_config")
export declare function robot_set_config(
    period: i32, height: i32,
    up_height: i32, stride: i32, tilt: i32,
): i32;
@external("env", "robot_get_period")
export declare function robot_get_period(): i32;
@external("env", "robot_get_height")
export declare function robot_get_height(): i32;
@external("env", "robot_get_up_height")
export declare function robot_get_up_height(): i32;
@external("env", "robot_get_stride")
export declare function robot_get_stride(): i32;
@external("env", "robot_get_tilt")
export declare function robot_get_tilt(): i32;

// ── Low-Level Servo Control ─────────────────────────────────────
@external("env", "robot_set_servo_angle")
export declare function robot_set_servo_angle(id: i32, centideg: i32): i32;
@external("env", "robot_flush")
export declare function robot_flush(): i32;
@external("env", "robot_set_servo_speed")
export declare function robot_set_servo_speed(id: i32, speed: i32): i32;
@external("env", "robot_set_servo_current")
export declare function robot_set_servo_current(id: i32, milliamps: i32): i32;
@external("env", "robot_get_servo_current")
export declare function robot_get_servo_current(id: i32): i32;
@external("env", "robot_read_position")
export declare function robot_read_position(id: i32): i32;
@external("env", "robot_read_angle_cdeg")
export declare function robot_read_angle_cdeg(id: i32): i32;
@external("env", "robot_read_speed")
export declare function robot_read_speed(id: i32): i32;
@external("env", "robot_read_load")
export declare function robot_read_load(id: i32): i32;
@external("env", "robot_read_voltage")
export declare function robot_read_voltage(id: i32): i32;
@external("env", "robot_read_temperature")
export declare function robot_read_temperature(id: i32): i32;
@external("env", "robot_read_moving")
export declare function robot_read_moving(id: i32): i32;
@external("env", "robot_read_current")
export declare function robot_read_current(id: i32): i32;

// ── Calibration / Utility ───────────────────────────────────────
@external("env", "robot_set_offset")
export declare function robot_set_offset(id: i32, centideg: i32): i32;
@external("env", "robot_get_offset")
export declare function robot_get_offset(id: i32): i32;
@external("env", "robot_ping_servo")
export declare function robot_ping_servo(id: i32): i32;
@external("env", "robot_delay_ms")
export declare function robot_delay_ms(ms: i32): i32;

// ── Inverse Kinematics / IMU ────────────────────────────────────
@external("env", "robot_ik_fr")
export declare function robot_ik_fr(x: f32, th0: f32, z: f32): i32;
@external("env", "robot_ik_fl")
export declare function robot_ik_fl(x: f32, th0: f32, z: f32): i32;
@external("env", "robot_ik_rr")
export declare function robot_ik_rr(x: f32, th0: f32, z: f32): i32;
@external("env", "robot_ik_rl")
export declare function robot_ik_rl(x: f32, th0: f32, z: f32): i32;
@external("env", "robot_imu_read")
export declare function robot_imu_read(buffer_ptr: usize): i32;
@external("env", "robot_imu_print")
export declare function robot_imu_print(): i32;

// ── Direct servo bus (requires servo_lock) ──────────────────────
@external("env", "servo_lock")
export declare function servo_lock(): i32;
@external("env", "servo_unlock")
export declare function servo_unlock(): i32;
@external("env", "servo_is_locked")
export declare function servo_is_locked(): i32;
@external("env", "servo_set_gain")
export declare function servo_set_gain(id: i32, param: i32, value: f32): i32;
@external("env", "servo_get_gain")
export declare function servo_get_gain(id: i32, param: i32, out_ptr: usize): i32;
@external("env", "servo_save_config")
export declare function servo_save_config(id: i32): i32;
@external("env", "servo_restore_config")
export declare function servo_restore_config(id: i32): i32;
@external("env", "servo_stage")
export declare function servo_stage(id: i32, q_deg: f32, tau_ma: f32, kp: f32, kd: f32): i32;
@external("env", "servo_commit")
export declare function servo_commit(): i32;
@external("env", "servo_write_all")
export declare function servo_write_all(cmd_ptr: usize, count: i32): i32;
@external("env", "servo_read")
export declare function servo_read(id: i32, out_ptr: usize): i32;
@external("env", "servo_read_all")
export declare function servo_read_all(out_ptr: usize): i32;
@external("env", "servo_poll")
export declare function servo_poll(): i32;
@external("env", "servo_direct")
export declare function servo_direct(id: i32, mode: i32, q_deg: f32, tau_ma: f32): i32;
@external("env", "servo_scan")
export declare function servo_scan(): i32;

// ── Control, clock and motion ───────────────────────────────────
@external("env", "mpx_control_take")
export declare function mpx_control_take(domain: i32): i32;
@external("env", "mpx_control_release")
export declare function mpx_control_release(): i32;
@external("env", "mpx_control_owner")
export declare function mpx_control_owner(): i32;
@external("env", "mpx_millis")
export declare function mpx_millis(): i32;
@external("env", "mpx_sleep_until")
export declare function mpx_sleep_until(t_ms: i32): i32;
@external("env", "mpx_drive")
export declare function mpx_drive(fwd: f32, strafe: f32, turn: f32): i32;
@external("env", "mpx_drive_stop")
export declare function mpx_drive_stop(): i32;
@external("env", "mpx_set_walk_speed")
export declare function mpx_set_walk_speed(mm_s: i32): i32;
@external("env", "mpx_get_walk_speed")
export declare function mpx_get_walk_speed(): i32;
@external("env", "mpx_foot")
export declare function mpx_foot(leg: i32, x: f32, th0: f32, z: f32): i32;
@external("env", "mpx_set_all_servo_speed")
export declare function mpx_set_all_servo_speed(speed: i32): i32;
@external("env", "mpx_reset_offsets")
export declare function mpx_reset_offsets(): i32;
@external("env", "mpx_read_temperature_c")
export declare function mpx_read_temperature_c(id: i32): f32;
@external("env", "mpx_board_scale_dd")
export declare function mpx_board_scale_dd(): f32;
@external("env", "mpx_param_f")
export declare function mpx_param_f(name_ptr: usize, fallback: f32): f32;
@external("env", "mpx_param_i")
export declare function mpx_param_i(name_ptr: usize, fallback: i32): i32;

// ── Overlay, tick and trace ─────────────────────────────────────
@external("env", "mpx_overlay")
export declare function mpx_overlay(id: i32, deg: f32): i32;
@external("env", "mpx_overlay_get")
export declare function mpx_overlay_get(id: i32): f32;
@external("env", "mpx_overlay_clear")
export declare function mpx_overlay_clear(): i32;
@external("env", "mpx_tick_every")
export declare function mpx_tick_every(period_ms: i32): i32;
@external("env", "mpx_tick_stop")
export declare function mpx_tick_stop(): i32;
@external("env", "mpx_trace")
export declare function mpx_trace(name_ptr: usize, value: f32): i32;

// ── Display canvas ──────────────────────────────────────────────
@external("env", "display_width")
export declare function display_width(): i32;
@external("env", "display_height")
export declare function display_height(): i32;
@external("env", "display_rgb")
export declare function display_rgb(r: i32, g: i32, b: i32): i32;
@external("env", "display_clear")
export declare function display_clear(color: i32): i32;
@external("env", "display_pixel")
export declare function display_pixel(x: i32, y: i32, color: i32): i32;
@external("env", "display_rect")
export declare function display_rect(x: i32, y: i32, w: i32, h: i32, color: i32, radius: i32, border: i32): i32;
@external("env", "display_circle")
export declare function display_circle(cx: i32, cy: i32, r: i32, color: i32, border: i32): i32;
@external("env", "display_line")
export declare function display_line(x1: i32, y1: i32, x2: i32, y2: i32, color: i32, width: i32): i32;
@external("env", "display_arc")
export declare function display_arc(cx: i32, cy: i32, r: i32, start_deg: i32, end_deg: i32, color: i32, width: i32): i32;
@external("env", "display_text")
export declare function display_text(x: i32, y: i32, text_ptr: usize, color: i32, size: i32, align: i32, max_w: i32): i32;
@external("env", "display_blit")
export declare function display_blit(x: i32, y: i32, w: i32, h: i32, data_ptr: usize, len: i32): i32;
@external("env", "display_show")
export declare function display_show(hold_ms: i32): i32;
@external("env", "display_face")
export declare function display_face(): i32;
@external("env", "display_show_text")
export declare function display_show_text(text_ptr: usize): i32;
@external("env", "display_show_emotion")
export declare function display_show_emotion(name_ptr: usize): i32;
@external("env", "display_set_colors")
export declare function display_set_colors(background_rgb: i32, eyes_rgb: i32): i32;
@external("env", "display_set_brightness")
export declare function display_set_brightness(percent: i32): i32;

// ── Filesystem / Music / Robot state ────────────────────────────
@external("env", "fs_read")
export declare function fs_read(path_ptr: usize, buf_ptr: usize, cap: i32): i32;
@external("env", "fs_write")
export declare function fs_write(path_ptr: usize, data_ptr: usize, len: i32): i32;
@external("env", "fs_delete")
export declare function fs_delete(path_ptr: usize): i32;
@external("env", "fs_exists")
export declare function fs_exists(path_ptr: usize): i32;
@external("env", "fs_list")
export declare function fs_list(dir_ptr: usize, buf_ptr: usize, cap: i32): i32;
@external("env", "fs_info")
export declare function fs_info(out_ptr: usize): i32;
@external("env", "music_play")
export declare function music_play(what_ptr: usize, title_ptr: usize, cap: i32): i32;
@external("env", "music_stop")
export declare function music_stop(): i32;
@external("env", "robot_get_config")
export declare function robot_get_config(out_ptr: usize): i32;
@external("env", "robot_get_gait")
export declare function robot_get_gait(buf_ptr: usize, cap: i32): i32;

// ── Microphone / Speaker / HTTP ─────────────────────────────────
@external("env", "mpx_mic_start")
export declare function mpx_mic_start(sample_rate: i32): i32;
@external("env", "mpx_mic_read")
export declare function mpx_mic_read(pcm_ptr: usize, frames: i32): i32;
@external("env", "mpx_mic_stop")
export declare function mpx_mic_stop(): i32;
@external("env", "mpx_audio_play_file")
export declare function mpx_audio_play_file(path_ptr: usize): i32;
@external("env", "mpx_audio_stream_url")
export declare function mpx_audio_stream_url(url_ptr: usize): i32;
@external("env", "mpx_http_get")
export declare function mpx_http_get(url_ptr: usize, response_ptr: usize, cap: i32): i32;
@external("env", "mpx_http_post")
export declare function mpx_http_post(url_ptr: usize, body_ptr: usize, response_ptr: usize, cap: i32): i32;


// ═══════════════════════════════════════════════════════════════════
//  Part 2 — High-Level Abstractions
// ═══════════════════════════════════════════════════════════════════

// ──── 1. Host enums ─────────────────────────────────────────────

export const enum ControlDomain {
    NONE   = 0,
    GAIT   = 1,
    FEET   = 2,
    JOINTS = 3,
    BUS    = 4,
}

export const enum Leg {
    FR = 0,
    FL = 1,
    RR = 2,
    RL = 3,
}

export const enum Gait {
    NONE         = 0,
    INIT         = 1,
    STEP         = 2,
    ROLL         = 3,
    PITCH        = 4,
    STRETCH      = 5,
    ADVANCE      = 6,
    BACK         = 7,
    LEFT         = 8,
    RIGHT        = 9,
    TURN_L       = 10,
    TURN_R       = 11,
    TWERK        = 12,
    JUMP         = 13,
    JUMP_FWD     = 14,
    TEST_SPD     = 15,
    LOOK_UP      = 16,
    LOOK_DOWN    = 17,
    LOOK_LEFT    = 18,
    LOOK_RIGHT   = 19,
    LOOK_UL      = 20,
    LOOK_UR      = 21,
    LOOK_LL      = 22,
    LOOK_LR      = 23,
    FORELEG_LIFT_L = 24,
    FORELEG_LIFT_R = 25,
    BACKLEG_LIFT_L = 26,
    BACKLEG_LIFT_R = 27,
    HEIGHT_UP    = 28,
    HEIGHT_DOWN  = 29,
    BALANCE      = 30,
    BOW_BACK     = 31,
    BODY_CYCLE   = 32,
    HEAD_ELLIPSE = 33,
    MOVE_LF      = 34,
    MOVE_RF      = 35,
    MOVE_LB      = 36,
    MOVE_RB      = 37,
}

const GAIT_NAMES: string[] = [
    "none", "init", "step", "roll", "pitch", "stretch", "advance", "back",
    "left", "right", "turnL", "turnR", "twerk", "jump", "jumpfwd", "testspeed",
    "lookup", "lookdown", "lookleft", "lookright", "lookul", "lookur", "lookll",
    "looklr", "flegL", "flegR", "blegL", "blegR", "heightup", "heightdown",
    "balance", "bowback", "bodycycle", "headellipse", "moveLF", "moveRF", "moveLB",
    "moveRB",
];

/** Start a gait using the type-safe enum. */
export function robotGait(g: Gait): void {
    const idx = g as i32;
    if (idx < 0 || idx > 37) return;
    const name = String.UTF8.encode(GAIT_NAMES[idx], true);
    robot_gait(changetype<usize>(name));
}

// ──── 2. Named servo IDs ────────────────────────────────────────

export const enum Servo {
    FR_HIP      = 1,
    FR_SHOULDER = 2,
    FR_KNEE     = 3,
    FL_HIP      = 4,
    FL_SHOULDER = 5,
    FL_KNEE     = 6,
    RR_HIP      = 7,
    RR_SHOULDER = 8,
    RR_KNEE     = 9,
    RL_HIP      = 10,
    RL_SHOULDER = 11,
    RL_KNEE     = 12,
}

// ──── 3. Degree-based servo control ─────────────────────────────

/** Set servo angle in degrees (auto-converts to centidegrees). */
export function setServoDeg(id: Servo, deg: f32): void {
    robot_set_servo_angle(id as i32, (deg * 100.0) as i32);
}

/** Set servo angle + speed in one call. */
export function setServo(id: Servo, deg: f32, speed: i32): void {
    robot_set_servo_speed(id as i32, speed);
    setServoDeg(id, deg);
}

// ──── 4. Config struct ─────────────────────────────────────────

export class RobotConfig {
    constructor(
        public period: i32 = 80,
        public height: i32 = 70,
        public upHeight: i32 = 10,
        public stride: i32 = 10,
        public tilt: i32 = 10,
    ) {}
}

/** Set config from a RobotConfig object. */
export function setRobotConfig(cfg: RobotConfig): void {
    robot_set_config(cfg.period, cfg.height, cfg.upHeight, cfg.stride, cfg.tilt);
}

/** Get current config as a RobotConfig object. */
export function getRobotConfig(): RobotConfig {
    return new RobotConfig(
        robot_get_period(),
        robot_get_height(),
        robot_get_up_height(),
        robot_get_stride(),
        robot_get_tilt(),
    );
}

// ──── 5. Choreography helpers ──────────────────────────────────

export function walkForward(ms: i32): void {
    robotGait(Gait.ADVANCE);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function walkBackward(ms: i32): void {
    robotGait(Gait.BACK);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function turnLeft(ms: i32): void {
    robotGait(Gait.TURN_L);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function turnRight(ms: i32): void {
    robotGait(Gait.TURN_R);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function strafeLeft(ms: i32): void {
    robotGait(Gait.LEFT);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function strafeRight(ms: i32): void {
    robotGait(Gait.RIGHT);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function jump(): void {
    robotGait(Gait.JUMP);
    robot_delay_ms(2000);
    robotGait(Gait.NONE);
}

export function stand(): void {
    robotGait(Gait.INIT);
    robot_delay_ms(2000);
}

// ──── 6. New gait choreography helpers ─────────────────────────

export function lookUp(ms: i32): void {
    robotGait(Gait.LOOK_UP);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function lookDown(ms: i32): void {
    robotGait(Gait.LOOK_DOWN);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function lookLeft(ms: i32): void {
    robotGait(Gait.LOOK_LEFT);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function lookRight(ms: i32): void {
    robotGait(Gait.LOOK_RIGHT);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function lookUpperLeft(ms: i32): void {
    robotGait(Gait.LOOK_UL);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function lookUpperRight(ms: i32): void {
    robotGait(Gait.LOOK_UR);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function lookLowerLeft(ms: i32): void {
    robotGait(Gait.LOOK_LL);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function lookLowerRight(ms: i32): void {
    robotGait(Gait.LOOK_LR);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function forelegLiftL(ms: i32): void {
    robotGait(Gait.FORELEG_LIFT_L);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function forelegLiftR(ms: i32): void {
    robotGait(Gait.FORELEG_LIFT_R);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function backlegLiftL(ms: i32): void {
    robotGait(Gait.BACKLEG_LIFT_L);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function backlegLiftR(ms: i32): void {
    robotGait(Gait.BACKLEG_LIFT_R);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function heightUp(ms: i32): void {
    robotGait(Gait.HEIGHT_UP);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function heightDown(ms: i32): void {
    robotGait(Gait.HEIGHT_DOWN);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function balance(ms: i32): void {
    robotGait(Gait.BALANCE);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function bowBack(ms: i32): void {
    robotGait(Gait.BOW_BACK);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function bodyCycle(ms: i32): void {
    robotGait(Gait.BODY_CYCLE);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function headEllipse(ms: i32): void {
    robotGait(Gait.HEAD_ELLIPSE);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function moveLF(ms: i32): void {
    robotGait(Gait.MOVE_LF);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function moveRF(ms: i32): void {
    robotGait(Gait.MOVE_RF);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function moveLB(ms: i32): void {
    robotGait(Gait.MOVE_LB);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function moveRB(ms: i32): void {
    robotGait(Gait.MOVE_RB);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function dance(ms: i32): void {
    robot_set_config(60, 60, 15, 8, 15);
    robotGait(Gait.TWERK);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

export function stepInPlace(ms: i32): void {
    robotGait(Gait.STEP);
    robot_delay_ms(ms);
    robotGait(Gait.NONE);
}

// ──── 6. Pose helper ───────────────────────────────────────────

export class RobotPose {
    constructor(
        public frHip: f32 = 0, public frShoulder: f32 = 0, public frKnee: f32 = 0,
        public flHip: f32 = 0, public flShoulder: f32 = 0, public flKnee: f32 = 0,
        public rrHip: f32 = 0, public rrShoulder: f32 = 0, public rrKnee: f32 = 0,
        public rlHip: f32 = 0, public rlShoulder: f32 = 0, public rlKnee: f32 = 0,
    ) {}
}

/** Apply a complete pose — sets all 12 servos and flushes. */
export function applyPose(p: RobotPose): void {
    robot_set_servo_angle(Servo.FR_HIP,      (p.frHip      * 100.0) as i32);
    robot_set_servo_angle(Servo.FR_SHOULDER,  (p.frShoulder * 100.0) as i32);
    robot_set_servo_angle(Servo.FR_KNEE,      (p.frKnee     * 100.0) as i32);
    robot_set_servo_angle(Servo.FL_HIP,      (p.flHip      * 100.0) as i32);
    robot_set_servo_angle(Servo.FL_SHOULDER,  (p.flShoulder * 100.0) as i32);
    robot_set_servo_angle(Servo.FL_KNEE,      (p.flKnee     * 100.0) as i32);
    robot_set_servo_angle(Servo.RR_HIP,      (p.rrHip      * 100.0) as i32);
    robot_set_servo_angle(Servo.RR_SHOULDER,  (p.rrShoulder * 100.0) as i32);
    robot_set_servo_angle(Servo.RR_KNEE,      (p.rrKnee     * 100.0) as i32);
    robot_set_servo_angle(Servo.RL_HIP,      (p.rlHip      * 100.0) as i32);
    robot_set_servo_angle(Servo.RL_SHOULDER,  (p.rlShoulder * 100.0) as i32);
    robot_set_servo_angle(Servo.RL_KNEE,      (p.rlKnee     * 100.0) as i32);
    robot_flush();
}

// ──── 7. IK helper ──────────────────────────────────────────────

/** Per-leg IK target. */
export class ikTarget {
    constructor(
        public x: f32 = 0,   /**< Forward/backward position in mm */
        public th0: f32 = 0, /**< Hip rotation in degrees */
        public z: f32 = 0,   /**< Height in mm */
    ) {}
}

/** Set front-right leg IK from an ikTarget. */
export function ikFR(t: ikTarget): void {
    robot_ik_fr(t.x, t.th0, t.z);
}
/** Set front-left leg IK from an ikTarget. */
export function ikFL(t: ikTarget): void {
    robot_ik_fl(t.x, t.th0, t.z);
}
/** Set rear-right leg IK from an ikTarget. */
export function ikRR(t: ikTarget): void {
    robot_ik_rr(t.x, t.th0, t.z);
}
/** Set rear-left leg IK from an ikTarget. */
export function ikRL(t: ikTarget): void {
    robot_ik_rl(t.x, t.th0, t.z);
}

// ──── 8. IMU data ───────────────────────────────────────────────

export class ImuData {
    constructor(
        public ax: f32 = 0, /**< Accel X (g) */
        public ay: f32 = 0, /**< Accel Y (g) */
        public az: f32 = 0, /**< Accel Z (g) */
        public gx: f32 = 0, /**< Gyro X (dps) */
        public gy: f32 = 0, /**< Gyro Y (dps) */
        public gz: f32 = 0, /**< Gyro Z (dps) */
    ) {}
}

/** Read IMU data into an ImuData object. */
export function readImu(): ImuData {
    const buf = new Float32Array(6);
    robot_imu_read(changetype<usize>(buf));
    return new ImuData(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
}
