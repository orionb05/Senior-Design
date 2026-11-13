#pragma once

/**
 * Print startup diagnostics and start the optional shared bring-up heartbeat.
 *
 * The role is included in log messages so the same routine can be used by the
 * collector and central hub applications.
 */
void scesm_bringup_start(const char *role);
