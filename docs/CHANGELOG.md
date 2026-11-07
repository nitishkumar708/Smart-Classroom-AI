# Changelog

## Repair Pass (2026-08-22)

The following were audited against the actual code (not assumed) and
fixed. Everything else in the codebase was left as-is because it was
already correct.

| Issue | File(s) | Fix | Verified how |
|---|---|---|---|
| `Camera` status in the sidebar used `liveStudents.length >= 0`, which is always `true` | `frontend/src/App.jsx`, `LiveFeedPanel.jsx` | Camera now reports real state (permission, stream active, last frame sent) up to `App`; status uses that | Code review (logic is unit-testable but needs a browser to fully exercise `getUserMedia`) — **REQUIRES RUNTIME VERIFICATION** in-browser |
| `buildDailyAnalyticsPlaceholder` fabricated 6 of 7 "historical" days as zero | `frontend/src/App.jsx`, `DailyAnalyticsPanel.jsx`, `backend/api/routes.py`, `backend/services/firebase_service.py` | Removed the placeholder. Added real `GET /api/analytics/daily`, aggregating persisted `attention_logs`/`attendance` from Firestore; returns `storage_available:false` + empty list (not fake data) when Firebase isn't configured | Booted the real Flask app (Firebase disabled) with the actual test client and confirmed the endpoint returns the honest empty state; full Firestore aggregation still needs a live project — **REQUIRES RUNTIME VERIFICATION** |
| An unimplemented "emotion" signal (always `"unknown"`) silently contributed 10% weight to every attention score; UI claimed "Emotion Analysis: ON" | `backend/config/attention_weights.json`, `backend/modules/attention_engine.py`, `frontend/src/components/LiveFeedPanel.jsx` | Removed emotion from the scoring engine and renormalized the remaining 6 weights to sum to 1.0; UI now shows "N/A" | Re-ran the engine's test scenarios (fully-attentive, sleeping) by hand against the live code — scores/levels unchanged in shape, weights confirmed to sum to 1.0, `emotion` key absent from both weights and sub-scores |
| `AttendanceManager` never reloaded today's Firestore records on startup, so a backend restart mid-class showed 0 present even though data was safely persisted | `backend/modules/attendance.py`, `backend/app.py`, `backend/tests/test_attendance.py` | Added `load_today_from_storage()`, called once at app boot when Firebase is configured; added 2 new tests | Manually executed the new + existing attendance tests' logic directly against the code (pytest wasn't installable offline) — all passed, including duplicate-prevention |
| Frame capture could send overlapping `/api/process-frame` requests if a response took longer than the 1.5s interval | `frontend/src/components/LiveFeedPanel.jsx` | Added an in-flight guard so a new frame is only sent once the previous one resolves | Code review — **REQUIRES RUNTIME VERIFICATION** in-browser |

**Confirmed already correct (no change made):** duplicate-attendance
prevention (deterministic per-student-per-day keying), alert
cooldown/deduplication, the `{success, data|error}` API response
envelope, per-module `/api/system/status` reporting, and Firebase
initialization failing loudly rather than being swallowed.

**Not attempted in this pass** (see spec for full detail): the dashboard
information-architecture redesign (sidebar grouping, glassmorphism/color
system reduction, Quick Actions), authentication implementation,
canonical `alert_id`/`resolved` fields on the Alert schema, and
structured `{code, message}` error bodies (current errors are
`{"success": false, "error": "<message>"}`, which is consistent but not
the exact shape in the original spec). These are real, scoped gaps —
not silently skipped — and are good next steps.
