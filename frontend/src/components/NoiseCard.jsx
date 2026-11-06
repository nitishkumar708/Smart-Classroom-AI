import React from "react";
import { Volume2 } from "lucide-react";
import { usePolling } from "../hooks/usePolling.js";
import api from "../api/client.js";

const STATE_STYLE = {
  QUIET: { color: "#34d399", label: "Quiet" },
  NORMAL: { color: "#fbbf24", label: "Normal" },
  LOUD: { color: "#f87171", label: "Loud" },
};

const BAR_MAX = 1000; // bar full = level 1000

export default function NoiseCard() {
  const { data, loading } = usePolling(api.esp32Sound, 2000);

  const online = !!data?.online;
  const style = STATE_STYLE[data?.state] || { color: "#64748b", label: "No data" };
  const level = data?.level ?? 0;
  const percent = Math.max(0, Math.min(100, (level / BAR_MAX) * 100));

  return (
    <div className="glass-panel p-4">
      <div className="flex items-center gap-2 mb-3">
        <Volume2 size={16} style={{ color: online ? style.color : "#64748b" }} />
        <h2 className="font-display font-semibold text-slate-100 text-[15px]">
          Classroom Noise
        </h2>
      </div>

      {loading ? (
        <p className="text-xs text-slate-500">Checking...</p>
      ) : !online ? (
        <p className="text-xs text-slate-500">
          ESP32 se sound data nahi aa raha (offline).
        </p>
      ) : (
        <>
          <div className="flex items-end justify-between mb-2">
            <span className="text-2xl font-semibold" style={{ color: style.color }}>
              {style.label}
            </span>
            <span className="text-xs text-slate-400 font-mono">Level {level}</span>
          </div>
          <div className="h-2 rounded-full bg-white/10 overflow-hidden">
            <div
              className="h-full rounded-full"
              style={{
                width: `${percent}%`,
                backgroundColor: style.color,
                transition: "width 0.4s",
              }}
            />
          </div>
          <p className="text-[11px] text-slate-500 mt-2">
            Updated {Math.round(data?.age_seconds ?? 0)}s ago
          </p>
        </>
      )}
    </div>
  );
}
