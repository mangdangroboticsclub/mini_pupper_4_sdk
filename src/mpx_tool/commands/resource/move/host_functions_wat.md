# MPX-Dog Host Functions — WAT Import Reference

Host ABI **v9**. All host functions are registered under the `"env"` module by
ESP32 firmware. Import them in `.wat` skills with `(import "env" ...)`.
Every command returns an `i32` status: `0` (`MPX_OK`) or a negative error.
Getters return data or an error as documented in `mpx_host.h`. Use `drop` when
ignoring a result.

String arguments are NUL-terminated string offsets in exported linear memory,
except `print`, which also takes an explicit byte length. Other `i32` pointer
arguments refer to caller-owned linear-memory buffers.

## Common return codes

| Name | Value | Meaning |
|---|---:|---|
| `MPX_OK` | 0 | Success |
| `MPX_ERR_ARG` | -1 | Bad argument, id, index or pointer |
| `MPX_ERR_NOT_LOCKED` | -2 | This skill does not hold the servo bus |
| `MPX_ERR_NO_REPLY` | -3 | Driver board did not answer |
| `MPX_ERR_READONLY` | -4 | Calibration parameter is read-only |
| `MPX_ERR_CANCELLED` | -5 | Skill was stopped mid-call |
| `MPX_ERR_STATE` | -6 | Wrong state or capability unavailable |
| `MPX_ERR_BUSY` | -7 | Another control domain holds the joints |

## ABI / Logging

```wat
(import "env" "mpx_abi_version" (func $mpx_abi_version (result i32)))
(import "env" "print" (func $print (param i32 i32) (result i32)))
```

## High-Level Gait Control

```wat
(import "env" "robot_gait" (func $robot_gait (param i32) (result i32)))
(import "env" "robot_get_mode" (func $robot_get_mode (result i32)))
(import "env" "robot_set_body_pose" (func $robot_set_body_pose (param f32 f32 f32) (result i32)))
(import "env" "robot_set_attitude_speed" (func $robot_set_attitude_speed (param i32) (result i32)))
(import "env" "robot_set_attitude_speed_xyz" (func $robot_set_attitude_speed_xyz (param i32 i32 i32) (result i32)))
```

## Configuration

```wat
(import "env" "robot_set_config" (func $robot_set_config (param i32 i32 i32 i32 i32) (result i32)))
(import "env" "robot_get_period" (func $robot_get_period (result i32)))
(import "env" "robot_get_height" (func $robot_get_height (result i32)))
(import "env" "robot_get_up_height" (func $robot_get_up_height (result i32)))
(import "env" "robot_get_stride" (func $robot_get_stride (result i32)))
(import "env" "robot_get_tilt" (func $robot_get_tilt (result i32)))
```

## Low-Level Servo Control

```wat
(import "env" "robot_set_servo_angle" (func $robot_set_servo_angle (param i32 i32) (result i32)))
(import "env" "robot_flush" (func $robot_flush (result i32)))
(import "env" "robot_set_servo_speed" (func $robot_set_servo_speed (param i32 i32) (result i32)))
(import "env" "robot_set_servo_current" (func $robot_set_servo_current (param i32 i32) (result i32)))
(import "env" "robot_get_servo_current" (func $robot_get_servo_current (param i32) (result i32)))
(import "env" "robot_read_position" (func $robot_read_position (param i32) (result i32)))
(import "env" "robot_read_angle_cdeg" (func $robot_read_angle_cdeg (param i32) (result i32)))
(import "env" "robot_read_speed" (func $robot_read_speed (param i32) (result i32)))
(import "env" "robot_read_load" (func $robot_read_load (param i32) (result i32)))
(import "env" "robot_read_voltage" (func $robot_read_voltage (param i32) (result i32)))
(import "env" "robot_read_temperature" (func $robot_read_temperature (param i32) (result i32)))
(import "env" "robot_read_moving" (func $robot_read_moving (param i32) (result i32)))
(import "env" "robot_read_current" (func $robot_read_current (param i32) (result i32)))
```

## Calibration / Utility

```wat
(import "env" "robot_set_offset" (func $robot_set_offset (param i32 i32) (result i32)))
(import "env" "robot_get_offset" (func $robot_get_offset (param i32) (result i32)))
(import "env" "robot_ping_servo" (func $robot_ping_servo (param i32) (result i32)))
(import "env" "robot_delay_ms" (func $robot_delay_ms (param i32) (result i32)))
```

## Inverse Kinematics / IMU

```wat
(import "env" "robot_ik_fr" (func $robot_ik_fr (param f32 f32 f32) (result i32)))
(import "env" "robot_ik_fl" (func $robot_ik_fl (param f32 f32 f32) (result i32)))
(import "env" "robot_ik_rr" (func $robot_ik_rr (param f32 f32 f32) (result i32)))
(import "env" "robot_ik_rl" (func $robot_ik_rl (param f32 f32 f32) (result i32)))
(import "env" "robot_imu_read" (func $robot_imu_read (param i32) (result i32)))
(import "env" "robot_imu_print" (func $robot_imu_print (result i32)))
```

`robot_imu_read` writes six `f32` values: `ax, ay, az, gx, gy, gz` (24 bytes).

## Direct Servo Bus (requires `servo_lock`)

```wat
(import "env" "servo_lock" (func $servo_lock (result i32)))
(import "env" "servo_unlock" (func $servo_unlock (result i32)))
(import "env" "servo_is_locked" (func $servo_is_locked (result i32)))
(import "env" "servo_set_gain" (func $servo_set_gain (param i32 i32 f32) (result i32)))
(import "env" "servo_get_gain" (func $servo_get_gain (param i32 i32 i32) (result i32)))
(import "env" "servo_save_config" (func $servo_save_config (param i32) (result i32)))
(import "env" "servo_restore_config" (func $servo_restore_config (param i32) (result i32)))
(import "env" "servo_stage" (func $servo_stage (param i32 f32 f32 f32 f32) (result i32)))
(import "env" "servo_commit" (func $servo_commit (result i32)))
(import "env" "servo_write_all" (func $servo_write_all (param i32 i32) (result i32)))
(import "env" "servo_read" (func $servo_read (param i32 i32) (result i32)))
(import "env" "servo_read_all" (func $servo_read_all (param i32) (result i32)))
(import "env" "servo_poll" (func $servo_poll (result i32)))
(import "env" "servo_direct" (func $servo_direct (param i32 i32 f32 f32) (result i32)))
(import "env" "servo_scan" (func $servo_scan (result i32)))
```

`servo_read` writes `{q_deg, tau_ma, temp_c, q_raw}` as four `f32` values;
`servo_read_all` writes twelve such records. `servo_write_all` takes records of
`{q_deg, tau_ma, kp, kd}`.

## Control, Clock and Motion

Control domains: `0=none`, `1=gait`, `2=feet`, `3=joints`, `4=bus`.
Legs: `0=FR`, `1=FL`, `2=RR`, `3=RL`.

```wat
(import "env" "mpx_control_take" (func $mpx_control_take (param i32) (result i32)))
(import "env" "mpx_control_release" (func $mpx_control_release (result i32)))
(import "env" "mpx_control_owner" (func $mpx_control_owner (result i32)))
(import "env" "mpx_millis" (func $mpx_millis (result i32)))
(import "env" "mpx_sleep_until" (func $mpx_sleep_until (param i32) (result i32)))
(import "env" "mpx_drive" (func $mpx_drive (param f32 f32 f32) (result i32)))
(import "env" "mpx_drive_stop" (func $mpx_drive_stop (result i32)))
(import "env" "mpx_set_walk_speed" (func $mpx_set_walk_speed (param i32) (result i32)))
(import "env" "mpx_get_walk_speed" (func $mpx_get_walk_speed (result i32)))
(import "env" "mpx_foot" (func $mpx_foot (param i32 f32 f32 f32) (result i32)))
(import "env" "mpx_set_all_servo_speed" (func $mpx_set_all_servo_speed (param i32) (result i32)))
(import "env" "mpx_reset_offsets" (func $mpx_reset_offsets (result i32)))
(import "env" "mpx_read_temperature_c" (func $mpx_read_temperature_c (param i32) (result f32)))
(import "env" "mpx_board_scale_dd" (func $mpx_board_scale_dd (result f32)))
(import "env" "mpx_param_f" (func $mpx_param_f (param i32 f32) (result f32)))
(import "env" "mpx_param_i" (func $mpx_param_i (param i32 i32) (result i32)))
```

## Overlay, Tick and Trace

```wat
(import "env" "mpx_overlay" (func $mpx_overlay (param i32 f32) (result i32)))
(import "env" "mpx_overlay_get" (func $mpx_overlay_get (param i32) (result f32)))
(import "env" "mpx_overlay_clear" (func $mpx_overlay_clear (result i32)))
(import "env" "mpx_tick_every" (func $mpx_tick_every (param i32) (result i32)))
(import "env" "mpx_tick_stop" (func $mpx_tick_stop (result i32)))
(import "env" "mpx_trace" (func $mpx_trace (param i32 f32) (result i32)))
```

`mpx_tick_every` invokes an exported `on_tick` function every 10–1000 ms.

## Display Canvas

Colours are `0xRRGGBB`; coordinates are pixels from the top left. Paint, then
call `display_show`. Robots without a screen return `MPX_ERR_STATE`.

```wat
(import "env" "display_width" (func $display_width (result i32)))
(import "env" "display_height" (func $display_height (result i32)))
(import "env" "display_rgb" (func $display_rgb (param i32 i32 i32) (result i32)))
(import "env" "display_clear" (func $display_clear (param i32) (result i32)))
(import "env" "display_pixel" (func $display_pixel (param i32 i32 i32) (result i32)))
(import "env" "display_rect" (func $display_rect (param i32 i32 i32 i32 i32 i32 i32) (result i32)))
(import "env" "display_circle" (func $display_circle (param i32 i32 i32 i32 i32) (result i32)))
(import "env" "display_line" (func $display_line (param i32 i32 i32 i32 i32 i32) (result i32)))
(import "env" "display_arc" (func $display_arc (param i32 i32 i32 i32 i32 i32 i32) (result i32)))
(import "env" "display_text" (func $display_text (param i32 i32 i32 i32 i32 i32 i32) (result i32)))
(import "env" "display_blit" (func $display_blit (param i32 i32 i32 i32 i32 i32) (result i32)))
(import "env" "display_show" (func $display_show (param i32) (result i32)))
(import "env" "display_face" (func $display_face (result i32)))
(import "env" "display_show_text" (func $display_show_text (param i32) (result i32)))
(import "env" "display_show_emotion" (func $display_show_emotion (param i32) (result i32)))
(import "env" "display_set_colors" (func $display_set_colors (param i32 i32) (result i32)))
(import "env" "display_set_brightness" (func $display_set_brightness (param i32) (result i32)))
```

## Filesystem / Music / Robot State

Paths are absolute inside LittleFS. Writes and deletes request phone-app user
approval. `fs_read` returns the whole file size; a result greater than `cap`
means the copied response was truncated.

```wat
(import "env" "fs_read" (func $fs_read (param i32 i32 i32) (result i32)))
(import "env" "fs_write" (func $fs_write (param i32 i32 i32) (result i32)))
(import "env" "fs_delete" (func $fs_delete (param i32) (result i32)))
(import "env" "fs_exists" (func $fs_exists (param i32) (result i32)))
(import "env" "fs_list" (func $fs_list (param i32 i32 i32) (result i32)))
(import "env" "fs_info" (func $fs_info (param i32) (result i32)))
(import "env" "music_play" (func $music_play (param i32 i32 i32) (result i32)))
(import "env" "music_stop" (func $music_stop (result i32)))
(import "env" "robot_get_config" (func $robot_get_config (param i32) (result i32)))
(import "env" "robot_get_gait" (func $robot_get_gait (param i32 i32) (result i32)))
```

`fs_info` writes `{total_bytes, used_bytes}` as `i32[2]`.
`robot_get_config` writes `{period, height, up_height, stride, tilt, sg_speed}`
as `i32[6]`.

## Microphone / Speaker Audio

```wat
(import "env" "mpx_mic_start" (func $mpx_mic_start (param i32) (result i32)))
(import "env" "mpx_mic_read" (func $mpx_mic_read (param i32 i32) (result i32)))
(import "env" "mpx_mic_stop" (func $mpx_mic_stop (result i32)))
(import "env" "mpx_audio_play_file" (func $mpx_audio_play_file (param i32) (result i32)))
(import "env" "mpx_audio_stream_url" (func $mpx_audio_stream_url (param i32) (result i32)))
```

`mpx_mic_start(0)` selects 16 kHz mono. `mpx_mic_read` writes at most 1920
`i16` frames. `mpx_audio_play_file` synchronously plays a 16-bit PCM WAV from
LittleFS; `mpx_audio_stream_url` queues asynchronous HTTP(S) playback.

## HTTP(S) APIs

Both calls block for up to 15 seconds, write at most `cap-1` response bytes plus
NUL, and return the HTTP status (including 4xx/5xx) or a negative error.
`mpx_http_post` sends the body as `application/json`; body pointer 0 sends an
empty request.

```wat
(import "env" "mpx_http_get" (func $mpx_http_get (param i32 i32 i32) (result i32)))
(import "env" "mpx_http_post" (func $mpx_http_post (param i32 i32 i32 i32) (result i32)))
```
