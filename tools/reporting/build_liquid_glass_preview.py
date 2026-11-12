#!/usr/bin/env python3
"""
Builder script to generate the Liquid Glass Web UI Preview for SORTED.
Includes:
- Authentic Google Chrome Window Background Layer (visible through the app)
- Interactive Background Switcher (Chrome, macOS Sonoma, Windows 11, Cosmic Mesh)
- SVG Turbulence & Displacement Filter (#glass-distortion)
- Multi-layer Glassmorphism (.glass-card, .glass-filter, .glass-overlay, .glass-specular, .glass-distortion-overlay)
- Interactive Cursor Tracking with Real-time Specular Sheen & Dynamic Turbulence Scale
- Full inspectable Organization Plan, Duplicates, Overview with Discrepancy Reconciliation Banner
"""

import sys
from pathlib import Path

DEST_PATH = Path(r"C:\Users\satya\.gemini\antigravity\brain\6146e3ed-a344-4a5c-aa29-a0c8ad8ef9f6\sorted_ui_preview.html")

html_content = """<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>SORTED — Liquid Glass UI (See-Through Preview)</title>
  <script src="https://www.gstatic.com/antigravity/web/dev/tailwindcss.min.js"></script>
  <style>
    :root {
      --canvas: rgba(247, 247, 245, 0.45);
      --sidebar: rgba(244, 244, 240, 0.55);
      --surface: rgba(255, 255, 255, 0.55);
      --surface-card: rgba(255, 255, 255, 0.65);
      --border: rgba(255, 255, 255, 0.65);
      --border-subtle: rgba(255, 255, 255, 0.35);
      --text-pri: #1D1D1F;
      --text-sec: #5E5E63;
      --text-muted: #86868B;
      --hover: rgba(255, 255, 255, 0.50);
      --accent: #1D1D1F;
      --accent-tint: rgba(29, 29, 31, 0.08);
      --highlight: rgba(255, 255, 255, 0.85);
      --success: #1E6B24;
      --success-bg: rgba(232, 245, 233, 0.8);
      --warning: #8C5200;
      --warning-bg: rgba(255, 243, 224, 0.8);
      --danger: #A82020;
      --danger-bg: rgba(255, 235, 238, 0.8);
    }
    .dark {
      --canvas: rgba(18, 18, 20, 0.55);
      --sidebar: rgba(24, 24, 28, 0.65);
      --surface: rgba(30, 30, 35, 0.65);
      --surface-card: rgba(36, 36, 42, 0.70);
      --border: rgba(255, 255, 255, 0.16);
      --border-subtle: rgba(255, 255, 255, 0.08);
      --text-pri: #F5F5F7;
      --text-sec: #A1A1A6;
      --text-muted: #6E6E73;
      --hover: rgba(255, 255, 255, 0.12);
      --accent: #FFFFFF;
      --accent-tint: rgba(255, 255, 255, 0.12);
      --highlight: rgba(255, 255, 255, 0.25);
      --success: #66BB6A;
      --success-bg: rgba(23, 56, 34, 0.8);
      --warning: #FFA726;
      --warning-bg: rgba(56, 42, 18, 0.8);
      --danger: #EF5350;
      --danger-bg: rgba(59, 28, 28, 0.8);
    }

    body {
      font-family: -apple-system, BlinkMacSystemFont, "SF Pro Text", "Segoe UI Variable Text", "Segoe UI", system-ui, sans-serif;
      overflow-x: hidden;
      margin: 0;
      padding: 0;
      user-select: none;
    }

    /* ============================================================
       LIQUID GLASS CARD ARCHITECTURE (Exact Multi-Layer Spec)
       ============================================================ */
    .glass-card {
      --bg-color: var(--surface-card);
      --highlight-color: var(--highlight);
      --text: var(--text-pri);
      position: relative;
      border-radius: 16px;
      overflow: hidden;
      box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.15), 0 2px 8px 0 rgba(0, 0, 0, 0.08);
      transition: box-shadow 0.25s ease, transform 0.2s ease;
    }

    .glass-filter,
    .glass-overlay,
    .glass-specular {
      position: absolute;
      inset: 0;
      border-radius: inherit;
    }

    .glass-filter {
      z-index: 1;
      backdrop-filter: blur(14px) saturate(135%) brightness(1.04);
      -webkit-backdrop-filter: blur(14px) saturate(135%) brightness(1.04);
      filter: url(#glass-distortion);
    }

    .glass-distortion-overlay {
      position: absolute;
      inset: 0;
      background: radial-gradient(circle at 20% 30%, rgba(255,255,255,0.06) 0%, transparent 80%),
                  radial-gradient(circle at 80% 70%, rgba(255,255,255,0.06) 0%, transparent 80%);
      background-size: 300% 300%;
      animation: floatDistort 12s infinite ease-in-out;
      mix-blend-mode: overlay;
      z-index: 2;
      pointer-events: none;
    }

    @keyframes floatDistort {
      0% { background-position: 0% 0%; }
      50% { background-position: 100% 100%; }
      100% { background-position: 0% 0%; }
    }

    .glass-overlay {
      z-index: 2;
      background: var(--bg-color);
      border: 1px solid var(--border);
    }

    .glass-specular {
      z-index: 3;
      box-shadow: inset 1px 1px 1.5px var(--highlight-color), inset -1px -1px 1px rgba(255, 255, 255, 0.2);
      pointer-events: none;
      transition: background 0.15s ease-out;
    }

    .glass-content {
      position: relative;
      z-index: 4;
      color: var(--text);
      height: 100%;
    }

    /* Subtle Glass Controls */
    .glass-btn {
      background: rgba(255, 255, 255, 0.35);
      border: 1px solid var(--border);
      backdrop-filter: blur(8px);
      -webkit-backdrop-filter: blur(8px);
      transition: all 0.2s cubic-bezier(0.16, 1, 0.3, 1);
    }
    .glass-btn:hover {
      background: rgba(255, 255, 255, 0.65);
      border-color: var(--highlight);
      transform: translateY(-1px);
    }
    .glass-btn-primary {
      background: #1D1D1F;
      color: #FFFFFF;
      border: 1px solid #1D1D1F;
      box-shadow: 0 2px 8px rgba(0, 0, 0, 0.15);
      transition: all 0.2s ease;
    }
    .glass-btn-primary:hover {
      background: #3A3A3C;
      border-color: #3A3A3C;
      transform: translateY(-1px);
    }
    .dark .glass-btn-primary {
      background: #FFFFFF;
      color: #1D1D1F;
      border: 1px solid #FFFFFF;
    }
    .dark .glass-btn-primary:hover {
      background: #E5E5EA;
    }

    .table-row-44 {
      height: 44px;
    }
    .lucide-icon {
      display: inline-block;
      stroke-width: 2;
      stroke-linecap: round;
      stroke-linejoin: round;
      fill: none;
    }

    /* Background wallpaper styles */
    .bg-chrome {
      background-color: #202124;
    }
    .bg-sonoma {
      background: linear-gradient(135deg, #1c3b5e 0%, #2f6587 35%, #5a8ca5 65%, #c88975 85%, #df8f5f 100%);
    }
    .bg-windows {
      background: radial-gradient(circle at 50% 50%, #0d47a1 0%, #1565c0 30%, #002171 70%, #000a12 100%);
    }
    .bg-aurora {
      background: linear-gradient(125deg, #09090e 0%, #161b33 30%, #290740 60%, #082930 100%);
    }

    /* Scrollbar */
    ::-webkit-scrollbar {
      width: 7px;
      height: 7px;
    }
    ::-webkit-scrollbar-track {
      background: transparent;
    }
    ::-webkit-scrollbar-thumb {
      background: rgba(134, 134, 139, 0.4);
      border-radius: 4px;
    }
    ::-webkit-scrollbar-thumb:hover {
      background: rgba(134, 134, 139, 0.7);
    }
  </style>
</head>
<body class="relative min-h-screen text-[var(--text-pri)] flex flex-col items-center justify-start p-4 sm:p-6 overflow-y-auto">

  <!-- ============================================================
       SVG FILTER FOR LIQUID GLASS DISTORTION
       ============================================================ -->
  <svg style="display: none" id="svg-filter-host">
    <filter id="glass-distortion">
      <feTurbulence type="turbulence" baseFrequency="0.008" numOctaves="2" result="noise" />
      <feDisplacementMap in="SourceGraphic" in2="noise" scale="35" xChannelSelector="R" yChannelSelector="G" />
    </filter>
  </svg>

  <!-- ============================================================
       1. UNDERLYING BACKGROUND SCREEN LAYER (Google Chrome, macOS, etc.)
       This sits directly behind SORTED. You see through the liquid glass!
       ============================================================ -->
  <div id="screenBackgroundLayer" class="fixed inset-0 z-0 bg-chrome transition-all duration-300 flex flex-col overflow-hidden pointer-events-auto">
    
    <!-- AUTHENTIC GOOGLE CHROME BROWSER WINDOW BEHIND SORTED -->
    <div id="chromeWindowView" class="w-full h-full flex flex-col bg-[#202124] text-[#E8EAED] select-none font-sans">
      <!-- Chrome Title Bar & Tab Strip -->
      <div class="h-10 bg-[#1F1F23] flex items-end px-3 space-x-1.5 border-b border-[#303134]">
        <!-- Traffic lights / Windows Controls -->
        <div class="flex items-center space-x-2 mr-3 pb-2.5">
          <span class="w-3 h-3 rounded-full bg-[#FF5F56] inline-block"></span>
          <span class="w-3 h-3 rounded-full bg-[#FFBD2E] inline-block"></span>
          <span class="w-3 h-3 rounded-full bg-[#27C93F] inline-block"></span>
        </div>
        
        <!-- Tab 1: Google -->
        <div class="px-4 py-2 bg-[#292A2D] rounded-t-lg text-xs font-medium flex items-center space-x-2 border-t-2 border-transparent text-[#9AA0A6] cursor-pointer hover:bg-[#35363A]">
          <svg class="w-3.5 h-3.5" viewBox="0 0 24 24"><path fill="#4285F4" d="M22.56 12.25c0-.78-.07-1.53-.2-2.25H12v4.26h5.92c-.26 1.37-1.04 2.53-2.21 3.31v2.77h3.57c2.08-1.92 3.28-4.74 3.28-8.09z"/><path fill="#34A853" d="M12 23c2.97 0 5.46-.98 7.28-2.66l-3.57-2.77c-.98.66-2.23 1.06-3.71 1.06-2.86 0-5.29-1.93-6.16-4.53H2.18v2.84C3.99 20.53 7.7 23 12 23z"/><path fill="#FBBC05" d="M5.84 14.09c-.22-.66-.35-1.36-.35-2.09s.13-1.43.35-2.09V7.06H2.18C1.43 8.55 1 10.22 1 12s.43 3.45 1.18 4.94l2.85-2.22.81-.63z"/><path fill="#EA4335" d="M12 5.38c1.62 0 3.06.56 4.21 1.64l3.15-3.15C17.45 2.09 14.97 1 12 1 7.7 1 3.99 3.47 2.18 7.06l3.66 2.84c.87-2.6 3.3-4.52 6.16-4.52z"/></svg>
          <span>Google Search</span>
        </div>

        <!-- Tab 2: GitHub (Active Tab) -->
        <div class="px-4 py-2 bg-[#323639] rounded-t-lg text-xs font-semibold flex items-center space-x-2 border-t-2 border-[#8AB4F8] text-[#FFFFFF] shadow-sm">
          <svg class="w-3.5 h-3.5 fill-white" viewBox="0 0 24 24"><path d="M12 0C5.37 0 0 5.37 0 12c0 5.31 3.435 9.795 8.205 11.385.6.105.825-.255.825-.57 0-.285-.015-1.23-.015-2.235-3.015.555-3.795-.735-4.035-1.41-.135-.345-.72-1.41-1.23-1.695-.42-.225-1.02-.78-.015-.795.945-.015 1.62.87 1.845 1.23 1.08 1.815 2.805 1.305 3.495.99.105-.78.42-1.305.765-1.605-2.67-.3-5.46-1.335-5.46-5.925 0-1.305.465-2.385 1.23-3.225-.12-.3-.54-1.53.12-3.18 0 0 1.005-.315 3.3 1.23.96-.27 1.98-.405 3-.405s2.04.135 3 .405c2.295-1.56 3.3-1.23 3.3-1.23.66 1.65.24 2.88.12 3.18.765.84 1.23 1.905 1.23 3.225 0 4.605-2.805 5.625-5.475 5.925.435.375.81 1.095.81 2.22 0 1.605-.015 2.895-.015 3.3 0 .315.225.69.825.57A12.02 12.02 0 0024 12c0-6.63-5.37-12-12-12z"/></svg>
          <span>GitHub — deepmind / sorted</span>
          <span class="text-[10px] text-[#9AA0A6] ml-1">×</span>
        </div>

        <!-- Tab 3: YouTube Music -->
        <div class="px-4 py-2 bg-[#292A2D] rounded-t-lg text-xs font-medium flex items-center space-x-2 text-[#9AA0A6] cursor-pointer hover:bg-[#35363A]">
          <svg class="w-3.5 h-3.5 fill-[#FF0000]" viewBox="0 0 24 24"><path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm-1 14.5v-9l6 4.5-6 4.5z"/></svg>
          <span>YouTube Music — Chill Lo-Fi</span>
        </div>

        <button class="w-6 h-6 rounded-full hover:bg-[#35363A] text-gray-400 flex items-center justify-center text-sm mb-1">+</button>
      </div>

      <!-- Chrome Omnibox Address Bar -->
      <div class="h-11 bg-[#292A2D] px-4 flex items-center space-x-3 border-b border-[#3C4043]">
        <div class="flex items-center space-x-2 text-[#9AA0A6]">
          <button class="hover:text-white transition-colors">←</button>
          <button class="hover:text-white transition-colors">→</button>
          <button class="hover:text-white transition-colors">↻</button>
        </div>
        <div class="flex-1 max-w-2xl h-7 bg-[#202124] rounded-full px-4 flex items-center space-x-2 text-xs text-[#E8EAED] border border-[#3C4043]">
          <svg class="w-3.5 h-3.5 stroke-[#8AB4F8]" fill="none" viewBox="0 0 24 24" stroke="currentColor"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 15v2m-6 4h12a2 2 0 002-2v-6a2 2 0 00-2-2H6a2 2 0 00-2 2v6a2 2 0 002 2zm10-10V7a4 4 0 00-8 0v4h8z"/></svg>
          <span class="text-[#8AB4F8]">https://</span><span>github.com/deepmind/sorted</span>
          <span class="ml-auto text-yellow-400">★</span>
        </div>
        <div class="flex items-center space-x-3 ml-auto text-xs text-[#9AA0A6]">
          <span class="px-2 py-0.5 rounded bg-[#35363A] text-[#8AB4F8] font-mono">Gemini AI Active</span>
          <span class="w-6 h-6 rounded-full bg-gradient-to-tr from-purple-500 to-indigo-500 text-white font-bold flex items-center justify-center text-[10px]">S</span>
        </div>
      </div>

      <!-- Chrome Web Page Body (Visible directly through the glass cards!) -->
      <div class="flex-1 p-8 bg-[#0D1117] text-[#C9D1D9] overflow-hidden relative">
        <div class="max-w-5xl mx-auto space-y-6 opacity-90">
          
          <!-- GitHub Repo Header -->
          <div class="flex items-center justify-between border-b border-[#30363D] pb-5">
            <div class="flex items-center space-x-3">
              <span class="text-xl font-bold text-[#58A6FF]">deepmind</span>
              <span class="text-xl text-gray-500">/</span>
              <span class="text-xl font-bold text-white">sorted</span>
              <span class="px-2 py-0.5 rounded-full text-xs font-semibold border border-[#30363D] text-[#8B949E]">Public</span>
            </div>
            <div class="flex items-center space-x-2 text-xs">
              <span class="px-3 py-1.5 rounded-md bg-[#21262D] border border-[#30363D] font-semibold text-white">★ Star 42.8k</span>
              <span class="px-3 py-1.5 rounded-md bg-[#21262D] border border-[#30363D] font-semibold text-white">⑂ Fork 3.1k</span>
            </div>
          </div>

          <!-- Colorful Badges & Highlights Strip -->
          <div class="grid grid-cols-3 gap-4">
            <div class="p-4 rounded-xl bg-gradient-to-br from-indigo-900/60 to-purple-900/40 border border-indigo-700/40">
              <span class="text-xs uppercase font-bold text-indigo-300">Architecture</span>
              <div class="text-lg font-bold text-white mt-1">C++17 + Qt 6 Widgets</div>
              <p class="text-xs text-indigo-200/80 mt-1">Sub-millisecond SHA-256 duplicate detection & DAA Merge Sort.</p>
            </div>
            <div class="p-4 rounded-xl bg-gradient-to-br from-emerald-900/60 to-teal-900/40 border border-emerald-700/40">
              <span class="text-xs uppercase font-bold text-emerald-300">Safety Guardrails</span>
              <div class="text-lg font-bold text-white mt-1">Strict Root-Only Scope</div>
              <p class="text-xs text-emerald-200/80 mt-1">Zero subfolder mutation. All code repos & projects safe.</p>
            </div>
            <div class="p-4 rounded-xl bg-gradient-to-br from-amber-900/60 to-orange-900/40 border border-amber-700/40">
              <span class="text-xs uppercase font-bold text-amber-300">Aesthetics</span>
              <div class="text-lg font-bold text-white mt-1">Apple macOS × Notion</div>
              <p class="text-xs text-amber-200/80 mt-1">Monochrome palette with real-time Liquid Glass specular reflection.</p>
            </div>
          </div>

          <!-- Code Files Browser View -->
          <div class="rounded-xl border border-[#30363D] bg-[#161B22] overflow-hidden text-xs">
            <div class="px-4 py-2.5 bg-[#21262D] border-b border-[#30363D] flex items-center justify-between text-[#8B949E]">
              <span class="font-mono text-white">main branch — 1 commit ahead of origin</span>
              <span>Updated 3 mins ago</span>
            </div>
            <div class="divide-y divide-[#21262D]">
              <div class="px-4 py-2 flex items-center space-x-3 hover:bg-[#1C2128]"><span class="text-blue-400">📁 src/core</span><span class="text-white">FileScanner.cpp & OrganizationPlanner.cpp</span><span class="ml-auto text-gray-500">Root-only discovery</span></div>
              <div class="px-4 py-2 flex items-center space-x-3 hover:bg-[#1C2128]"><span class="text-blue-400">📁 src/ui</span><span class="text-white">Theme.h & LiquidGlassBackdrop.cpp</span><span class="ml-auto text-gray-500">Windows Acrylic DWM</span></div>
              <div class="px-4 py-2 flex items-center space-x-3 hover:bg-[#1C2128]"><span class="text-yellow-400">📄 desktop_app.py</span><span class="text-white">Translucent background & interactive inspectability</span><span class="ml-auto text-gray-500">100% verified</span></div>
              <div class="px-4 py-2 flex items-center space-x-3 hover:bg-[#1C2128]"><span class="text-emerald-400">📄 CMakeLists.txt</span><span class="text-white">Build configuration for Qt 6.5+ Widgets & Core</span><span class="ml-auto text-gray-500">MinGW64 / MSVC</span></div>
            </div>
          </div>

          <!-- YouTube Music Mini Player Overlay in Chrome -->
          <div class="fixed bottom-6 right-8 p-3 rounded-2xl bg-[#181818]/95 border border-[#333] shadow-2xl flex items-center space-x-3 w-80 text-white z-0">
            <div class="w-12 h-12 rounded-xl bg-gradient-to-tr from-rose-500 to-amber-500 flex items-center justify-center font-bold text-xs shadow-md">LO-FI</div>
            <div class="flex-1 min-w-0">
              <div class="text-xs font-semibold truncate">Synthwave & Lo-Fi Coding Beats</div>
              <div class="text-[11px] text-gray-400">Deep Work Radio • 2.4k listening</div>
              <div class="w-full bg-gray-700 h-1 rounded-full mt-1.5 overflow-hidden">
                <div class="bg-red-500 h-full w-2/3"></div>
              </div>
            </div>
          </div>

        </div>
      </div>
    </div>
  </div>

  <!-- ============================================================
       2. TOP FLOATING INTERACTIVE CONTROLS BAR
       Switch backgrounds (Chrome, macOS, Windows 11), adjust blur & distortion
       ============================================================ -->
  <header class="relative z-30 mb-4 w-full max-w-6xl flex flex-wrap items-center justify-between gap-3 px-5 py-2.5 rounded-2xl glass-card text-xs">
    <div class="glass-filter"></div>
    <div class="glass-overlay" style="background: rgba(255, 255, 255, 0.55);"></div>
    <div class="glass-specular"></div>
    <div class="glass-distortion-overlay"></div>
    <div class="glass-content flex flex-wrap items-center justify-between w-full gap-3">
      
      <!-- Brand & Status -->
      <div class="flex items-center space-x-3">
        <div class="flex items-center space-x-1.5 font-bold tracking-tight text-sm">
          <span class="w-2.5 h-2.5 rounded-full bg-gradient-to-r from-cyan-400 to-blue-500 animate-pulse"></span>
          <span>SORTED</span>
          <span class="text-[11px] font-normal text-[var(--text-muted)]">— Liquid Glass Engine</span>
        </div>
        <span class="px-2.5 py-0.5 rounded-full text-[10px] font-bold tracking-wide uppercase bg-emerald-500/20 text-emerald-800 border border-emerald-500/30">
          See-Through Active
        </span>
      </div>

      <!-- Background Switcher Buttons -->
      <div class="flex items-center space-x-1.5 bg-black/10 p-1 rounded-xl">
        <span class="text-[11px] font-medium px-2 text-[var(--text-sec)]">Behind Window:</span>
        <button onclick="setScreenBg('chrome')" id="btnBgChrome" class="px-2.5 py-1 rounded-lg text-xs font-semibold bg-white text-black shadow-sm transition-all flex items-center space-x-1">
          <span>🌐</span><span>Google Chrome</span>
        </button>
        <button onclick="setScreenBg('sonoma')" id="btnBgSonoma" class="px-2.5 py-1 rounded-lg text-xs font-medium text-[var(--text-pri)] hover:bg-white/40 transition-all flex items-center space-x-1">
          <span>🖥️</span><span>macOS Sonoma</span>
        </button>
        <button onclick="setScreenBg('windows')" id="btnBgWindows" class="px-2.5 py-1 rounded-lg text-xs font-medium text-[var(--text-pri)] hover:bg-white/40 transition-all flex items-center space-x-1">
          <span>🪟</span><span>Windows 11 Bloom</span>
        </button>
        <button onclick="setScreenBg('aurora')" id="btnBgAurora" class="px-2.5 py-1 rounded-lg text-xs font-medium text-[var(--text-pri)] hover:bg-white/40 transition-all flex items-center space-x-1">
          <span>🌌</span><span>Aurora Mesh</span>
        </button>
      </div>

      <!-- Real-time Glass Tuning Sliders -->
      <div class="flex items-center space-x-4">
        <div class="flex items-center space-x-1.5">
          <span class="text-[11px] text-[var(--text-sec)]">Blur:</span>
          <input type="range" id="sliderBlur" min="4" max="28" value="14" oninput="updateBlur(this.value)" class="w-16 accent-[#1D1D1F] cursor-pointer">
          <span id="lblBlur" class="font-mono text-[11px] w-6">14px</span>
        </div>
        <div class="flex items-center space-x-1.5">
          <span class="text-[11px] text-[var(--text-sec)]">Distortion:</span>
          <input type="range" id="sliderDistort" min="0" max="77" value="35" oninput="updateDistortion(this.value)" class="w-16 accent-[#1D1D1F] cursor-pointer">
          <span id="lblDistort" class="font-mono text-[11px] w-6">35</span>
        </div>
        <button onclick="toggleTheme()" class="px-2.5 py-1 rounded-lg glass-btn text-xs font-semibold flex items-center space-x-1">
          <span id="themeEmoji">🌙</span><span id="themeLabel">Dark</span>
        </button>
      </div>

    </div>
  </header>

  <!-- ============================================================
       3. MAIN SORTED APPLICATION WINDOW CONTAINER (Liquid Glass Card)
       Floating over the Chrome screen layer!
       ============================================================ -->
  <div id="sortedMainWindow" class="relative z-20 w-full max-w-6xl rounded-2xl glass-card flex flex-col h-[780px] transition-all duration-300">
    <div class="glass-filter"></div>
    <div class="glass-overlay"></div>
    <div class="glass-specular"></div>
    <div class="glass-distortion-overlay"></div>
    
    <div class="glass-content flex flex-col h-full overflow-hidden">
      
      <!-- macOS Style Glass Window Title Bar -->
      <div class="border-b border-[var(--border)] px-6 py-3 flex items-center justify-between shrink-0" style="background: rgba(255, 255, 255, 0.35);">
        <div class="flex items-center space-x-3">
          <span class="w-3 h-3 rounded-full bg-[#FF5F56] inline-block shadow-sm"></span>
          <span class="w-3 h-3 rounded-full bg-[#FFBD2E] inline-block shadow-sm"></span>
          <span class="w-3 h-3 rounded-full bg-[#27C93F] inline-block shadow-sm"></span>
          <span class="text-xs font-semibold tracking-wide pl-2" style="color: var(--text-sec);">SORTED — Safe Desktop File Organizer</span>
          <span class="text-[10px] font-mono px-2 py-0.5 rounded-full border border-[var(--border)]" style="background: var(--hover); color: var(--text-muted);">
            C++17 Core / DWM Acrylic
          </span>
        </div>

        <!-- Top Persistent Toolbar: Folder Capsule & Scan Action -->
        <div class="flex items-center space-x-3">
          <div id="folderPill" class="flex items-center space-x-2 px-3 py-1.5 rounded-[10px] border border-[var(--border)] glass-btn">
            <svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="m6 14 1.5-2.9A2 2 0 0 1 9.24 10H20a2 2 0 0 1 1.94 2.5l-1.54 6a2 2 0 0 1-1.95 1.5H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h3.9a2 2 0 0 1 1.69.9l.81 1.2a2 2 0 0 0 1.67.9H18a2 2 0 0 1 2 2v2"/></svg>
            <span id="topFolderName" class="text-xs font-semibold">SORTED_TEST</span>
            <span id="topFolderPath" class="text-[11px] text-[var(--text-sec)]">(.../SORTED_TEST)</span>
            <button onclick="toggleEmptyOrPopulated()" class="ml-1 text-[11px] px-2 py-0.5 rounded-lg border border-[var(--border)] hover:bg-[var(--hover)] font-medium">Change</button>
          </div>

          <span id="scanStatusBadge" class="text-xs px-3 py-1.5 rounded-full font-medium" style="background: var(--hover); color: var(--text-sec); border: 1px solid var(--border);">Ready</span>
          <button id="btnScanTop" onclick="runSimulatedScan()" class="text-xs font-semibold glass-btn-primary px-4 py-2 rounded-[10px] flex items-center space-x-1.5">
            <svg class="lucide-icon w-3.5 h-3.5 stroke-white dark:stroke-black" viewBox="0 0 24 24"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/></svg>
            <span>Scan Workspace</span>
          </button>
        </div>
      </div>

      <!-- Main Workspace Split: Sidebar + Content -->
      <div class="flex flex-1 overflow-hidden">
        
        <!-- Collapsible Liquid Glass Sidebar (244px expanded / 72px collapsed) -->
        <aside id="appSidebar" class="w-[244px] border-r border-[var(--border)] flex flex-col justify-between p-4 transition-all duration-200 shrink-0" style="background: var(--sidebar);">
          <div>
            <!-- Expanded Header -->
            <div id="expandedHeader" class="flex flex-col space-y-1 mb-4">
              <div class="flex items-center justify-between px-1">
                <div class="flex items-center space-x-1.5">
                  <h1 class="text-2xl font-bold tracking-tight" style="color: var(--text-pri);">sorted.</h1>
                  <span class="w-2 h-2 rounded-full inline-block mt-2 bg-[#1D1D1F] dark:bg-white"></span>
                </div>
                <button onclick="toggleSidebar()" class="w-[34px] h-[34px] flex items-center justify-center rounded-[9px] border border-[var(--border)] hover:bg-[var(--hover)] transition-colors" title="Collapse sidebar">
                  <svg class="lucide-icon w-4 h-4 stroke-[var(--text-sec)]" viewBox="0 0 24 24"><rect width="18" height="18" x="3" y="3" rx="2"/><path d="M9 3v18"/><path d="m16 15-3-3 3-3"/></svg>
                </button>
              </div>
              <p class="text-xs px-1" style="color: var(--text-muted);">Root-only safe organizer</p>
            </div>

            <!-- Collapsed Header -->
            <div id="collapsedHeader" class="hidden flex-col items-center space-y-2 mb-4">
              <div class="w-[34px] h-[34px] rounded-[8px] flex items-center justify-center font-bold text-xs" style="background: var(--accent-tint); color: var(--accent);">
                S•
              </div>
              <button onclick="toggleSidebar()" class="w-[34px] h-[34px] flex items-center justify-center rounded-[9px] border border-[var(--border)] hover:bg-[var(--hover)] transition-colors" title="Expand sidebar">
                <svg class="lucide-icon w-4 h-4 stroke-[var(--text-sec)]" viewBox="0 0 24 24"><rect width="18" height="18" x="3" y="3" rx="2"/><path d="M9 3v18"/><path d="m14 9 3 3-3 3"/></svg>
              </button>
            </div>

            <!-- Nav Items List -->
            <nav class="space-y-1">
              <!-- Overview -->
              <button onclick="switchTab('overview')" id="nav-overview" class="nav-btn w-full text-left px-3 py-2.5 rounded-[10px] text-sm font-semibold flex items-center justify-between transition-colors relative" style="background: var(--accent-tint); color: var(--accent);">
                <span class="active-indicator hidden absolute left-1 top-2.5 bottom-2.5 w-1 rounded-full bg-[var(--accent)]"></span>
                <div class="flex items-center space-x-3 nav-content">
                  <svg class="lucide-icon w-5 h-5 stroke-[var(--accent)] shrink-0" viewBox="0 0 24 24"><rect width="7" height="9" x="3" y="3" rx="1"/><rect width="7" height="5" x="14" y="3" rx="1"/><rect width="7" height="9" x="14" y="12" rx="1"/><rect width="7" height="5" x="3" y="16" rx="1"/></svg>
                  <span class="nav-label">Overview</span>
                </div>
                <span class="nav-badge text-xs font-mono font-bold" id="nav-count-overview">7</span>
              </button>

              <!-- All Files -->
              <button onclick="switchTab('files')" id="nav-files" class="nav-btn w-full text-left px-3 py-2.5 rounded-[10px] text-sm font-medium hover:bg-[var(--hover)] transition-colors flex items-center justify-between relative" style="color: var(--text-sec);">
                <span class="active-indicator hidden absolute left-1 top-2.5 bottom-2.5 w-1 rounded-full bg-[var(--accent)]"></span>
                <div class="flex items-center space-x-3 nav-content">
                  <svg class="lucide-icon w-5 h-5 stroke-[var(--text-sec)] shrink-0" viewBox="0 0 24 24"><path d="M20 7h-3a2 2 0 0 1-2-2V2"/><path d="M9 18a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h7l4 4v10a2 2 0 0 1-2 2Z"/><path d="M3 7.6v12.8A1.6 1.6 0 0 0 4.6 22h9.8"/></svg>
                  <span class="nav-label">All Files</span>
                </div>
                <span class="nav-badge text-xs font-mono" style="color: var(--text-muted);">7</span>
              </button>

              <!-- Categories -->
              <button onclick="switchTab('categories')" id="nav-categories" class="nav-btn w-full text-left px-3 py-2.5 rounded-[10px] text-sm font-medium hover:bg-[var(--hover)] transition-colors flex items-center justify-between relative" style="color: var(--text-sec);">
                <span class="active-indicator hidden absolute left-1 top-2.5 bottom-2.5 w-1 rounded-full bg-[var(--accent)]"></span>
                <div class="flex items-center space-x-3 nav-content">
                  <svg class="lucide-icon w-5 h-5 stroke-[var(--text-sec)] shrink-0" viewBox="0 0 24 24"><path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z"/></svg>
                  <span class="nav-label">Categories</span>
                </div>
                <span class="nav-badge text-xs font-mono" style="color: var(--text-muted);">4</span>
              </button>

              <!-- Duplicates -->
              <button onclick="switchTab('duplicates')" id="nav-duplicates" class="nav-btn w-full text-left px-3 py-2.5 rounded-[10px] text-sm font-medium hover:bg-[var(--hover)] transition-colors flex items-center justify-between relative" style="color: var(--text-sec);">
                <span class="active-indicator hidden absolute left-1 top-2.5 bottom-2.5 w-1 rounded-full bg-[var(--accent)]"></span>
                <div class="flex items-center space-x-3 nav-content">
                  <svg class="lucide-icon w-5 h-5 stroke-[var(--text-sec)] shrink-0" viewBox="0 0 24 24"><rect width="14" height="14" x="8" y="8" rx="2" ry="2"/><path d="M4 16c-1.1 0-2-.9-2-2V4c0-1.1.9-2 2-2h10c1.1 0 2 .9 2 2"/></svg>
                  <span class="nav-label">Duplicates</span>
                </div>
                <span class="nav-badge text-xs font-mono" style="color: var(--text-muted);" id="nav-count-duplicates">2 groups</span>
              </button>

              <!-- Organization Plan -->
              <button onclick="switchTab('organize')" id="nav-organize" class="nav-btn w-full text-left px-3 py-2.5 rounded-[10px] text-sm font-medium hover:bg-[var(--hover)] transition-colors flex items-center justify-between relative" style="color: var(--text-sec);">
                <span class="active-indicator hidden absolute left-1 top-2.5 bottom-2.5 w-1 rounded-full bg-[var(--accent)]"></span>
                <div class="flex items-center space-x-3 nav-content">
                  <svg class="lucide-icon w-5 h-5 stroke-[var(--text-sec)] shrink-0" viewBox="0 0 24 24"><path d="m3 17 2 2 4-4"/><path d="m3 7 2 2 4-4"/><path d="M13 6h8"/><path d="M13 12h8"/><path d="M13 18h8"/></svg>
                  <span class="nav-label">Organization</span>
                </div>
                <span class="nav-badge text-xs font-mono font-bold px-1.5 py-0.5 rounded bg-[#1D1D1F] text-white dark:bg-white dark:text-black" id="nav-count-plan">7</span>
              </button>

              <!-- History -->
              <button onclick="switchTab('history')" id="nav-history" class="nav-btn w-full text-left px-3 py-2.5 rounded-[10px] text-sm font-medium hover:bg-[var(--hover)] transition-colors flex items-center justify-between relative" style="color: var(--text-sec);">
                <span class="active-indicator hidden absolute left-1 top-2.5 bottom-2.5 w-1 rounded-full bg-[var(--accent)]"></span>
                <div class="flex items-center space-x-3 nav-content">
                  <svg class="lucide-icon w-5 h-5 stroke-[var(--text-sec)] shrink-0" viewBox="0 0 24 24"><path d="M3 12a9 9 0 1 0 9-9 9.75 9.75 0 0 0-6.74 2.74L3 8"/><path d="M3 3v5h5"/><path d="M12 7v5l4 2"/></svg>
                  <span class="nav-label">History & Log</span>
                </div>
                <span class="nav-badge text-xs font-mono" style="color: var(--text-muted);" id="nav-count-history">1</span>
              </button>
            </nav>
          </div>

          <!-- Protected Subfolders Footnote Pill in Sidebar -->
          <div class="p-2.5 rounded-xl border border-[var(--border)] glass-btn text-[11px] space-y-1">
            <div class="flex items-center space-x-1.5 font-semibold text-[var(--text-pri)]">
              <svg class="lucide-icon w-3.5 h-3.5 stroke-emerald-600" viewBox="0 0 24 24"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10"/></svg>
              <span>Subfolders Protected</span>
            </div>
            <p class="text-[10px] text-[var(--text-sec)]">3 folders preserved intact (empty-folder, nested, same-name).</p>
          </div>
        </aside>

        <!-- Main Content Area with Transparent Scroll -->
        <main class="flex-1 flex flex-col overflow-y-auto" style="background: transparent;">
          
          <!-- Header Info Strip -->
          <div class="px-8 py-5 border-b border-[var(--border)] flex items-center justify-between shrink-0" style="background: rgba(255, 255, 255, 0.25);">
            <div>
              <h2 id="pageTitle" class="text-xl font-bold tracking-tight" style="color: var(--text-pri);">Overview</h2>
              <p id="pageSubtitle" class="text-xs mt-0.5" style="color: var(--text-sec);">Workspace metrics, storage footprint reconciliation, and deterministic classification</p>
            </div>
            <div class="flex items-center space-x-2">
              <span class="text-xs font-mono px-3 py-1 rounded-[8px] border border-[var(--border)] glass-btn" style="color: var(--text-sec);">
                DAA MergeSort: <span class="font-bold text-[var(--text-pri)]">O(n log n)</span>
              </span>
            </div>
          </div>

          <!-- Content Views Container -->
          <div class="p-8 flex-1 space-y-6">
            
            <!-- TAB 1: OVERVIEW -->
            <div id="tab-overview" class="space-y-6">
              
              <!-- Storage Discrepancy Reconciliation Banner (Explaining 895.7 MB vs 462.7 MB) -->
              <div class="glass-card p-4 rounded-xl border border-[var(--border)]">
                <div class="glass-filter"></div>
                <div class="glass-overlay" style="background: rgba(242, 246, 255, 0.55);"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content flex items-start space-x-3 text-xs">
                  <svg class="lucide-icon w-5 h-5 stroke-[var(--text-pri)] shrink-0 mt-0.5" viewBox="0 0 24 24"><circle cx="12" cy="12" r="10"/><path d="M12 16v-4"/><path d="M12 8h.01"/></svg>
                  <div class="flex-1 space-y-1">
                    <div class="font-semibold text-sm">Storage Footprint & Scope Reconciliation:</div>
                    <p class="text-[var(--text-sec)]">
                      Windows Explorer displays <b>895.70 MB</b> for the workspace because it recursively counts nested folders. 
                      SORTED strictly isolates <b>462.73 MB</b> (58 root loose files) for organization, protecting <b>423.12 MB</b> (22 subfolders, 2,314 files) and excluding <b>9.84 MB</b> in <code class="px-1 py-0.5 rounded bg-black/10">Sorted/</code>.
                    </p>
                    <div class="flex items-center space-x-4 pt-1 font-mono text-[11px] text-[var(--text-pri)]">
                      <span>• Root Loose Files: <b>7 files (130 B)</b></span>
                      <span>• Protected Folders: <b>3 folders</b></span>
                      <span>• Subfolder Content: <b>Intact & Untouched</b></span>
                    </div>
                  </div>
                </div>
              </div>

              <!-- 4 Metric Cards with Multi-Layer Liquid Glass -->
              <div class="grid grid-cols-4 gap-4">
                
                <!-- Card 1 -->
                <div class="glass-card p-5 rounded-2xl">
                  <div class="glass-filter"></div>
                  <div class="glass-overlay"></div>
                  <div class="glass-specular"></div>
                  <div class="glass-distortion-overlay"></div>
                  <div class="glass-content flex flex-col justify-between">
                    <div class="flex items-center justify-between">
                      <span class="text-xs font-semibold uppercase tracking-wider" style="color: var(--text-sec);">Root Loose Files</span>
                      <svg class="lucide-icon w-4 h-4 stroke-[var(--text-sec)]" viewBox="0 0 24 24"><path d="M20 7h-3a2 2 0 0 1-2-2V2"/><path d="M9 18a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h7l4 4v10a2 2 0 0 1-2 2Z"/><path d="M3 7.6v12.8A1.6 1.6 0 0 0 4.6 22h9.8"/></svg>
                    </div>
                    <div class="text-3xl font-bold mt-2 font-mono" style="color: var(--text-pri);" id="statTotalFiles">7</div>
                    <div class="text-xs mt-1" style="color: var(--text-muted);">Direct root candidates</div>
                  </div>
                </div>

                <!-- Card 2 -->
                <div class="glass-card p-5 rounded-2xl">
                  <div class="glass-filter"></div>
                  <div class="glass-overlay"></div>
                  <div class="glass-specular"></div>
                  <div class="glass-distortion-overlay"></div>
                  <div class="glass-content flex flex-col justify-between">
                    <div class="flex items-center justify-between">
                      <span class="text-xs font-semibold uppercase tracking-wider" style="color: var(--text-sec);">Indexed Volume</span>
                      <svg class="lucide-icon w-4 h-4 stroke-[var(--text-sec)]" viewBox="0 0 24 24"><ellipse cx="12" cy="5" rx="9" ry="3"/><path d="M3 5V19A9 3 0 0 0 21 19V5"/><path d="M3 12A9 3 0 0 0 21 12"/></svg>
                    </div>
                    <div class="text-3xl font-bold mt-2 font-mono" style="color: var(--text-pri);" id="statTotalVolume">130 B</div>
                    <div class="text-xs mt-1" style="color: var(--text-muted);">Safe eligible volume</div>
                  </div>
                </div>

                <!-- Card 3 -->
                <div class="glass-card p-5 rounded-2xl">
                  <div class="glass-filter"></div>
                  <div class="glass-overlay"></div>
                  <div class="glass-specular"></div>
                  <div class="glass-distortion-overlay"></div>
                  <div class="glass-content flex flex-col justify-between">
                    <div class="flex items-center justify-between">
                      <span class="text-xs font-semibold uppercase tracking-wider" style="color: var(--text-sec);">Categories</span>
                      <svg class="lucide-icon w-4 h-4 stroke-[var(--text-sec)]" viewBox="0 0 24 24"><path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z"/></svg>
                    </div>
                    <div class="text-3xl font-bold mt-2 font-mono" style="color: var(--text-pri);" id="statTotalCategories">4</div>
                    <div class="text-xs mt-1" style="color: var(--text-muted);">Deterministic classification</div>
                  </div>
                </div>

                <!-- Card 4 -->
                <div class="glass-card p-5 rounded-2xl">
                  <div class="glass-filter"></div>
                  <div class="glass-overlay"></div>
                  <div class="glass-specular"></div>
                  <div class="glass-distortion-overlay"></div>
                  <div class="glass-content flex flex-col justify-between">
                    <div class="flex items-center justify-between">
                      <span class="text-xs font-semibold uppercase tracking-wider" style="color: var(--text-sec);">Duplicates</span>
                      <svg class="lucide-icon w-4 h-4 stroke-[var(--text-sec)]" viewBox="0 0 24 24"><rect width="14" height="14" x="8" y="8" rx="2" ry="2"/><path d="M4 16c-1.1 0-2-.9-2-2V4c0-1.1.9-2 2-2h10c1.1 0 2 .9 2 2"/></svg>
                    </div>
                    <div class="text-3xl font-bold mt-2 font-mono" style="color: var(--text-pri);" id="statTotalDuplicates">2 groups</div>
                    <div class="text-xs mt-1 font-semibold text-amber-700 dark:text-amber-300">4 duplicate copies</div>
                  </div>
                </div>

              </div>

              <!-- Category Distribution Table Card -->
              <div class="glass-card rounded-2xl overflow-hidden">
                <div class="glass-filter"></div>
                <div class="glass-overlay"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content p-5 space-y-3">
                  <div class="flex items-center justify-between">
                    <h3 class="text-sm font-semibold" style="color: var(--text-pri);">Category Distribution</h3>
                    <span class="text-xs font-mono text-[var(--text-muted)]">Verified Taxonomy</span>
                  </div>
                  <div class="overflow-x-auto rounded-xl border border-[var(--border)]">
                    <table class="w-full text-left text-xs">
                      <thead class="border-b border-[var(--border)] font-semibold uppercase tracking-wider" style="background: rgba(0,0,0,0.05); color: var(--text-sec);">
                        <tr>
                          <th class="py-3 px-5">Category</th>
                          <th class="py-3 px-5">Files Count</th>
                          <th class="py-3 px-5">Storage Volume</th>
                          <th class="py-3 px-5">% Share</th>
                        </tr>
                      </thead>
                      <tbody class="divide-y divide-[var(--border-subtle)]">
                        <tr class="table-row-44 hover:bg-[var(--hover)] transition-colors"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z"/></svg><span>Documents</span></td><td class="py-3 px-5">4</td><td class="py-3 px-5 text-[var(--text-sec)]">72 B</td><td class="py-3 px-5 text-[var(--text-sec)]">55.4%</td></tr>
                        <tr class="table-row-44 hover:bg-[var(--hover)] transition-colors"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z"/></svg><span>Images</span></td><td class="py-3 px-5">2</td><td class="py-3 px-5 text-[var(--text-sec)]">58 B</td><td class="py-3 px-5 text-[var(--text-sec)]">44.6%</td></tr>
                        <tr class="table-row-44 hover:bg-[var(--hover)] transition-colors"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z"/></svg><span>Other</span></td><td class="py-3 px-5">1</td><td class="py-3 px-5 text-[var(--text-sec)]">0 B</td><td class="py-3 px-5 text-[var(--text-sec)]">0.0%</td></tr>
                      </tbody>
                    </table>
                  </div>
                </div>
              </div>

            </div>

            <!-- TAB 2: ALL FILES (with DAA MergeSort) -->
            <div id="tab-files" class="space-y-4 hidden">
              <div class="flex items-center space-x-3">
                <div class="relative flex-1">
                  <svg class="lucide-icon w-4 h-4 stroke-[var(--text-muted)] absolute left-3 top-3" viewBox="0 0 24 24"><circle cx="11" cy="11" r="8"/><path d="m21 21-4.3-4.3"/></svg>
                  <input id="searchInput" type="text" placeholder="Search file name... (Ctrl+K)" class="w-full pl-9 pr-4 py-2 rounded-xl text-xs border border-[var(--border)] glass-btn outline-none">
                </div>
                <select class="px-3 py-2 rounded-xl text-xs border border-[var(--border)] glass-btn outline-none">
                  <option value="ALL">All Categories</option>
                  <option value="Documents">Documents</option>
                  <option value="Images">Images</option>
                  <option value="Other">Other</option>
                </select>
                <select class="px-3 py-2 rounded-xl text-xs border border-[var(--border)] glass-btn outline-none">
                  <option value="NAME_ASC">Sort: Name (A-Z)</option>
                  <option value="SIZE_ASC">Sort: Size (Smallest)</option>
                  <option value="SIZE_DESC">Sort: Size (Largest)</option>
                </select>
              </div>

              <div class="text-xs text-[var(--text-sec)]">DAA Merge Sort: 7 items sorted in 0.22 ms</div>

              <div class="glass-card rounded-2xl overflow-hidden">
                <div class="glass-filter"></div>
                <div class="glass-overlay"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content">
                  <table class="w-full text-left text-xs">
                    <thead class="border-b border-[var(--border)] font-semibold uppercase tracking-wider" style="background: rgba(0,0,0,0.05); color: var(--text-sec);">
                      <tr>
                        <th class="py-3 px-5">Name</th>
                        <th class="py-3 px-5">Category</th>
                        <th class="py-3 px-5">Ext</th>
                        <th class="py-3 px-5">Size</th>
                        <th class="py-3 px-5">Location</th>
                      </tr>
                    </thead>
                    <tbody class="divide-y divide-[var(--border-subtle)]">
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 7h-3a2 2 0 0 1-2-2V2"/><path d="M9 18a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h7l4 4v10a2 2 0 0 1-2 2Z"/></svg><span>document.pdf</span></td><td class="py-3 px-5">Documents</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">pdf</td><td class="py-3 px-5">35 B</td><td class="py-3 px-5 text-[var(--text-sec)]">SORTED_TEST</td></tr>
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 7h-3a2 2 0 0 1-2-2V2"/><path d="M9 18a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h7l4 4v10a2 2 0 0 1-2 2Z"/></svg><span>duplicate-a.txt</span></td><td class="py-3 px-5">Documents</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">txt</td><td class="py-3 px-5">36 B</td><td class="py-3 px-5 text-[var(--text-sec)]">SORTED_TEST</td></tr>
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 7h-3a2 2 0 0 1-2-2V2"/><path d="M9 18a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h7l4 4v10a2 2 0 0 1-2 2Z"/></svg><span>duplicate-b.txt</span></td><td class="py-3 px-5">Documents</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">txt</td><td class="py-3 px-5">36 B</td><td class="py-3 px-5 text-[var(--text-sec)]">SORTED_TEST</td></tr>
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 7h-3a2 2 0 0 1-2-2V2"/><path d="M9 18a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h7l4 4v10a2 2 0 0 1-2 2Z"/></svg><span>notes.txt</span></td><td class="py-3 px-5">Documents</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">txt</td><td class="py-3 px-5">29 B</td><td class="py-3 px-5 text-[var(--text-sec)]">SORTED_TEST</td></tr>
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 7h-3a2 2 0 0 1-2-2V2"/><path d="M9 18a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h7l4 4v10a2 2 0 0 1-2 2Z"/></svg><span>photo.jpg</span></td><td class="py-3 px-5">Images</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">jpg</td><td class="py-3 px-5">29 B</td><td class="py-3 px-5 text-[var(--text-sec)]">SORTED_TEST</td></tr>
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 7h-3a2 2 0 0 1-2-2V2"/><path d="M9 18a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h7l4 4v10a2 2 0 0 1-2 2Z"/></svg><span>photo_dup.jpg</span></td><td class="py-3 px-5">Images</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">jpg</td><td class="py-3 px-5">29 B</td><td class="py-3 px-5 text-[var(--text-sec)]">SORTED_TEST</td></tr>
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 7h-3a2 2 0 0 1-2-2V2"/><path d="M9 18a2 2 0 0 1-2-2V4a2 2 0 0 1 2-2h7l4 4v10a2 2 0 0 1-2 2Z"/></svg><span>unknown.xyz</span></td><td class="py-3 px-5">Other</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">xyz</td><td class="py-3 px-5">0 B</td><td class="py-3 px-5 text-[var(--text-sec)]">SORTED_TEST</td></tr>
                    </tbody>
                  </table>
                </div>
              </div>
            </div>

            <!-- TAB 3: CATEGORIES -->
            <div id="tab-categories" class="space-y-4 hidden">
              <h3 class="text-sm font-semibold" style="color: var(--text-pri);">Taxonomy & Target Directories</h3>
              <div class="glass-card rounded-2xl overflow-hidden">
                <div class="glass-filter"></div>
                <div class="glass-overlay"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content">
                  <table class="w-full text-left text-xs">
                    <thead class="border-b border-[var(--border)] font-semibold uppercase tracking-wider" style="background: rgba(0,0,0,0.05); color: var(--text-sec);">
                      <tr>
                        <th class="py-3 px-5">Category</th>
                        <th class="py-3 px-5">Files Count</th>
                        <th class="py-3 px-5">Storage Volume</th>
                        <th class="py-3 px-5">Target Routing Hierarchy</th>
                      </tr>
                    </thead>
                    <tbody class="divide-y divide-[var(--border-subtle)]">
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z"/></svg><span>Documents</span></td><td class="py-3 px-5">4</td><td class="py-3 px-5">72 B</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">Sorted/Documents/[EXT]/</td></tr>
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z"/></svg><span>Images</span></td><td class="py-3 px-5">2</td><td class="py-3 px-5">58 B</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">Sorted/Images/[EXT]/</td></tr>
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 font-medium flex items-center space-x-2"><svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><path d="M20 20a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-7.9a2 2 0 0 1-1.69-.9L9.6 3.9A2 2 0 0 0 7.93 3H4a2 2 0 0 0-2 2v13a2 2 0 0 0 2 2Z"/></svg><span>Other</span></td><td class="py-3 px-5">1</td><td class="py-3 px-5">0 B</td><td class="py-3 px-5 font-mono text-[var(--text-sec)]">Sorted/Other/[EXT]/</td></tr>
                    </tbody>
                  </table>
                </div>
              </div>
            </div>

            <!-- TAB 4: DUPLICATES -->
            <div id="tab-duplicates" class="space-y-4 hidden">
              <div class="glass-card p-4 rounded-xl border border-[var(--border)]">
                <div class="glass-filter"></div>
                <div class="glass-overlay" style="background: rgba(242, 246, 255, 0.55);"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content flex items-center space-x-3 text-xs">
                  <svg class="lucide-icon w-5 h-5 stroke-[var(--text-pri)] shrink-0" viewBox="0 0 24 24"><path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10"/></svg>
                  <div>
                    <b>Safe Duplicate Engine:</b> Groups are detected via size prefiltering and streaming 1 MiB SHA-256 chunked hashing. Files are never deleted automatically.
                  </div>
                </div>
              </div>

              <!-- Duplicate Group 1 -->
              <div class="glass-card rounded-2xl p-5 space-y-3">
                <div class="glass-filter"></div>
                <div class="glass-overlay"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content">
                  <div class="text-xs font-semibold flex items-center space-x-2 mb-2" style="color: var(--text-pri);">
                    <svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><rect width="14" height="14" x="8" y="8" rx="2" ry="2"/><path d="M4 16c-1.1 0-2-.9-2-2V4c0-1.1.9-2 2-2h10c1.1 0 2 .9 2 2"/></svg>
                    <span>Duplicate Group #1 (2 copies, 36 bytes, SHA-256: 9a9df381d892...)</span>
                  </div>
                  <ul class="text-xs space-y-2 pl-4 text-[var(--text-sec)]">
                    <li class="flex items-center space-x-2"><span class="w-2 h-2 rounded-full bg-emerald-500"></span><span class="font-mono">SORTED_TEST/duplicate-a.txt</span> <span class="text-[10px] px-2 py-0.5 rounded bg-black/10">Original (Oldest)</span></li>
                    <li class="flex items-center space-x-2"><span class="w-2 h-2 rounded-full bg-amber-500"></span><span class="font-mono">SORTED_TEST/duplicate-b.txt</span> <span class="text-[10px] px-2 py-0.5 rounded bg-amber-500/20 text-amber-800">Duplicate</span></li>
                  </ul>
                </div>
              </div>

              <!-- Duplicate Group 2 -->
              <div class="glass-card rounded-2xl p-5 space-y-3">
                <div class="glass-filter"></div>
                <div class="glass-overlay"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content">
                  <div class="text-xs font-semibold flex items-center space-x-2 mb-2" style="color: var(--text-pri);">
                    <svg class="lucide-icon w-4 h-4 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><rect width="14" height="14" x="8" y="8" rx="2" ry="2"/><path d="M4 16c-1.1 0-2-.9-2-2V4c0-1.1.9-2 2-2h10c1.1 0 2 .9 2 2"/></svg>
                    <span>Duplicate Group #2 (2 copies, 29 bytes, SHA-256: f3835cd98108...)</span>
                  </div>
                  <ul class="text-xs space-y-2 pl-4 text-[var(--text-sec)]">
                    <li class="flex items-center space-x-2"><span class="w-2 h-2 rounded-full bg-emerald-500"></span><span class="font-mono">SORTED_TEST/photo.jpg</span> <span class="text-[10px] px-2 py-0.5 rounded bg-black/10">Original (Oldest)</span></li>
                    <li class="flex items-center space-x-2"><span class="w-2 h-2 rounded-full bg-amber-500"></span><span class="font-mono">SORTED_TEST/photo_dup.jpg</span> <span class="text-[10px] px-2 py-0.5 rounded bg-amber-500/20 text-amber-800">Duplicate</span></li>
                  </ul>
                </div>
              </div>
            </div>

            <!-- TAB 5: ORGANIZATION PLAN (Inspectable, Resizable, Copyable) -->
            <div id="tab-organize" class="space-y-4 hidden">
              <div class="flex items-center justify-between">
                <div class="text-xs font-semibold" style="color: var(--text-pri);">
                  Proposed Moves: 7 direct root files | All destination paths 100% inspectable before execution
                </div>
                <div class="flex items-center space-x-2">
                  <button onclick="copyTableDestinations()" class="px-3 py-1.5 rounded-xl text-xs glass-btn font-medium flex items-center space-x-1.5">
                    <svg class="lucide-icon w-3.5 h-3.5 stroke-[var(--text-pri)]" viewBox="0 0 24 24"><rect width="14" height="14" x="8" y="8" rx="2" ry="2"/><path d="M4 16c-1.1 0-2-.9-2-2V4c0-1.1.9-2 2-2h10c1.1 0 2 .9 2 2"/></svg>
                    <span id="lblCopyPaths">Copy Dest Paths</span>
                  </button>
                  <button class="px-3 py-1.5 rounded-xl text-xs glass-btn font-medium">Select All</button>
                  <button class="px-3 py-1.5 rounded-xl text-xs glass-btn font-medium">Deselect All</button>
                </div>
              </div>

              <!-- Organization Table with Tooltips & Resizable Scroll -->
              <div class="glass-card rounded-2xl overflow-hidden">
                <div class="glass-filter"></div>
                <div class="glass-overlay"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content overflow-x-auto">
                  <table class="w-full text-left text-xs whitespace-nowrap min-w-[850px]">
                    <thead class="border-b border-[var(--border)] font-semibold uppercase tracking-wider" style="background: rgba(0,0,0,0.05); color: var(--text-sec);">
                      <tr>
                        <th class="py-3 px-4 w-12 text-center">Inc</th>
                        <th class="py-3 px-4 w-44">File Name</th>
                        <th class="py-3 px-4 w-80">Current Location</th>
                        <th class="py-3 px-4 w-96">Proposed Destination</th>
                        <th class="py-3 px-4 w-32">Status</th>
                      </tr>
                    </thead>
                    <tbody class="divide-y divide-[var(--border-subtle)] font-mono">
                      
                      <!-- Row 1 -->
                      <tr class="table-row-44 hover:bg-[var(--hover)] group transition-colors" title="Current: .../SORTED_TEST/document.pdf &#10;Proposed: .../Sorted/Documents/PDF/document.pdf &#10;Status: Ready (No Collision)">
                        <td class="py-3 px-4 text-center"><input type="checkbox" checked class="rounded"></td>
                        <td class="py-3 px-4 font-semibold text-[var(--text-pri)] font-sans">document.pdf</td>
                        <td class="py-3 px-4 text-[var(--text-sec)] truncate max-w-xs">SORTED_TEST\document.pdf</td>
                        <td class="py-3 px-4 font-medium text-[var(--text-pri)] truncate max-w-sm">Sorted\Documents\PDF\document.pdf</td>
                        <td class="py-3 px-4 text-emerald-600 dark:text-emerald-400 font-sans font-semibold">✓ Ready</td>
                      </tr>

                      <!-- Row 2 -->
                      <tr class="table-row-44 hover:bg-[var(--hover)] group transition-colors" title="Current: .../SORTED_TEST/duplicate-a.txt &#10;Proposed: .../Sorted/Documents/TXT/duplicate-a.txt">
                        <td class="py-3 px-4 text-center"><input type="checkbox" checked class="rounded"></td>
                        <td class="py-3 px-4 font-semibold text-[var(--text-pri)] font-sans">duplicate-a.txt</td>
                        <td class="py-3 px-4 text-[var(--text-sec)] truncate max-w-xs">SORTED_TEST\duplicate-a.txt</td>
                        <td class="py-3 px-4 font-medium text-[var(--text-pri)] truncate max-w-sm">Sorted\Documents\TXT\duplicate-a.txt</td>
                        <td class="py-3 px-4 text-emerald-600 dark:text-emerald-400 font-sans font-semibold">✓ Ready</td>
                      </tr>

                      <!-- Row 3 -->
                      <tr class="table-row-44 hover:bg-[var(--hover)] group transition-colors" title="Current: .../SORTED_TEST/duplicate-b.txt &#10;Proposed: .../Sorted/Documents/TXT/duplicate-b.txt">
                        <td class="py-3 px-4 text-center"><input type="checkbox" checked class="rounded"></td>
                        <td class="py-3 px-4 font-semibold text-[var(--text-pri)] font-sans">duplicate-b.txt</td>
                        <td class="py-3 px-4 text-[var(--text-sec)] truncate max-w-xs">SORTED_TEST\duplicate-b.txt</td>
                        <td class="py-3 px-4 font-medium text-[var(--text-pri)] truncate max-w-sm">Sorted\Documents\TXT\duplicate-b.txt</td>
                        <td class="py-3 px-4 text-emerald-600 dark:text-emerald-400 font-sans font-semibold">✓ Ready</td>
                      </tr>

                      <!-- Row 4 -->
                      <tr class="table-row-44 hover:bg-[var(--hover)] group transition-colors" title="Current: .../SORTED_TEST/notes.txt &#10;Proposed: .../Sorted/Documents/TXT/notes.txt">
                        <td class="py-3 px-4 text-center"><input type="checkbox" checked class="rounded"></td>
                        <td class="py-3 px-4 font-semibold text-[var(--text-pri)] font-sans">notes.txt</td>
                        <td class="py-3 px-4 text-[var(--text-sec)] truncate max-w-xs">SORTED_TEST\notes.txt</td>
                        <td class="py-3 px-4 font-medium text-[var(--text-pri)] truncate max-w-sm">Sorted\Documents\TXT\notes.txt</td>
                        <td class="py-3 px-4 text-emerald-600 dark:text-emerald-400 font-sans font-semibold">✓ Ready</td>
                      </tr>

                      <!-- Row 5 -->
                      <tr class="table-row-44 hover:bg-[var(--hover)] group transition-colors" title="Current: .../SORTED_TEST/photo.jpg &#10;Proposed: .../Sorted/Images/JPG/photo.jpg">
                        <td class="py-3 px-4 text-center"><input type="checkbox" checked class="rounded"></td>
                        <td class="py-3 px-4 font-semibold text-[var(--text-pri)] font-sans">photo.jpg</td>
                        <td class="py-3 px-4 text-[var(--text-sec)] truncate max-w-xs">SORTED_TEST\photo.jpg</td>
                        <td class="py-3 px-4 font-medium text-[var(--text-pri)] truncate max-w-sm">Sorted\Images\JPG\photo.jpg</td>
                        <td class="py-3 px-4 text-emerald-600 dark:text-emerald-400 font-sans font-semibold">✓ Ready</td>
                      </tr>

                      <!-- Row 6 -->
                      <tr class="table-row-44 hover:bg-[var(--hover)] group transition-colors" title="Current: .../SORTED_TEST/photo_dup.jpg &#10;Proposed: .../Sorted/Images/JPG/photo_dup.jpg">
                        <td class="py-3 px-4 text-center"><input type="checkbox" checked class="rounded"></td>
                        <td class="py-3 px-4 font-semibold text-[var(--text-pri)] font-sans">photo_dup.jpg</td>
                        <td class="py-3 px-4 text-[var(--text-sec)] truncate max-w-xs">SORTED_TEST\photo_dup.jpg</td>
                        <td class="py-3 px-4 font-medium text-[var(--text-pri)] truncate max-w-sm">Sorted\Images\JPG\photo_dup.jpg</td>
                        <td class="py-3 px-4 text-emerald-600 dark:text-emerald-400 font-sans font-semibold">✓ Ready</td>
                      </tr>

                      <!-- Row 7 -->
                      <tr class="table-row-44 hover:bg-[var(--hover)] group transition-colors" title="Current: .../SORTED_TEST/unknown.xyz &#10;Proposed: .../Sorted/Other/XYZ/unknown.xyz">
                        <td class="py-3 px-4 text-center"><input type="checkbox" checked class="rounded"></td>
                        <td class="py-3 px-4 font-semibold text-[var(--text-pri)] font-sans">unknown.xyz</td>
                        <td class="py-3 px-4 text-[var(--text-sec)] truncate max-w-xs">SORTED_TEST\unknown.xyz</td>
                        <td class="py-3 px-4 font-medium text-[var(--text-pri)] truncate max-w-sm">Sorted\Other\XYZ\unknown.xyz</td>
                        <td class="py-3 px-4 text-emerald-600 dark:text-emerald-400 font-sans font-semibold">✓ Ready</td>
                      </tr>

                    </tbody>
                  </table>
                </div>
              </div>

              <!-- Sticky Bottom Action Bar with Liquid Glass Sheen -->
              <div class="glass-card p-4 rounded-xl border border-[var(--border)] flex items-center justify-between">
                <div class="glass-filter"></div>
                <div class="glass-overlay"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content flex items-center justify-between w-full">
                  <div class="text-xs">
                    <span class="font-semibold text-[var(--text-pri)]">7 of 7 operations approved.</span>
                    <span class="text-[var(--text-sec)] ml-2">Operation journal initialized. Undo available anytime.</span>
                  </div>
                  <button class="px-5 py-2 rounded-xl text-xs font-semibold glass-btn-primary flex items-center space-x-2">
                    <svg class="lucide-icon w-4 h-4 stroke-white dark:stroke-black" viewBox="0 0 24 24"><polygon points="13 2 3 14 12 14 11 22 21 10 12 10 13 2"/></svg>
                    <span>Execute Approved Moves</span>
                  </button>
                </div>
              </div>
            </div>

            <!-- TAB 6: HISTORY & JOURNAL -->
            <div id="tab-history" class="space-y-4 hidden">
              <div class="flex items-center justify-between">
                <h3 class="text-sm font-semibold" style="color: var(--text-pri);">Operation Audit Trail (.sorted_journal.json)</h3>
                <div class="flex items-center space-x-2">
                  <button class="px-3 py-1.5 rounded-xl text-xs glass-btn font-medium flex items-center space-x-1.5">
                    <svg class="lucide-icon w-3.5 h-3.5 stroke-[var(--text-sec)]" viewBox="0 0 24 24"><path d="M9 14 4 9l5-5"/><path d="M4 9h10.5a5.5 5.5 0 0 1 5.5 5.5v0a5.5 5.5 0 0 1-5.5 5.5H11"/></svg>
                    <span>Undo Last Batch</span>
                  </button>
                  <button class="px-3 py-1.5 rounded-xl text-xs glass-btn font-medium flex items-center space-x-1.5">
                    <svg class="lucide-icon w-3.5 h-3.5 stroke-[var(--text-sec)]" viewBox="0 0 24 24"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"/><polyline points="7 10 12 15 17 10"/><line x1="12" x2="12" y1="15" y2="3"/></svg>
                    <span>Export CSV</span>
                  </button>
                </div>
              </div>

              <div class="glass-card rounded-2xl overflow-hidden">
                <div class="glass-filter"></div>
                <div class="glass-overlay"></div>
                <div class="glass-specular"></div>
                <div class="glass-distortion-overlay"></div>
                <div class="glass-content">
                  <table class="w-full text-left text-xs font-mono">
                    <thead class="border-b border-[var(--border)] font-semibold uppercase tracking-wider font-sans" style="background: rgba(0,0,0,0.05); color: var(--text-sec);">
                      <tr>
                        <th class="py-3 px-5">Timestamp</th>
                        <th class="py-3 px-5">Source File</th>
                        <th class="py-3 px-5">Destination File</th>
                        <th class="py-3 px-5">Status</th>
                      </tr>
                    </thead>
                    <tbody class="divide-y divide-[var(--border-subtle)]">
                      <tr class="table-row-44 hover:bg-[var(--hover)]"><td class="py-3 px-5 text-[var(--text-sec)]">2026-11-12 14:22:01</td><td class="py-3 px-5">SORTED_TEST\document.pdf</td><td class="py-3 px-5">Sorted\Documents\PDF\document.pdf</td><td class="py-3 px-5 text-emerald-600 font-sans font-semibold">Completed</td></tr>
                    </tbody>
                  </table>
                </div>
              </div>
            </div>

          </div>
        </main>

      </div>
    </div>
  </div>

  <!-- ============================================================
       4. INTERACTIVE JAVASCRIPT: LIQUID SPECULAR & TURBULENCE TRACKING
       Exact user implementation augmented for all glass components
       ============================================================ -->
  <script>
    let isDarkMode = false;
    let isCollapsed = false;
    let isEmptyState = false;
    let currentScale = 35;

    // Add mouse movement interactivity to glass elements
    document.addEventListener('DOMContentLoaded', function() {
      // Get all glass elements
      const glassElements = document.querySelectorAll('.glass-card');
      
      // Add mousemove effect for each glass element
      glassElements.forEach(element => {
        element.addEventListener('mousemove', handleMouseMove);
        element.addEventListener('mouseleave', handleMouseLeave);
      });
      
      // Handle mouse movement over glass elements
      function handleMouseMove(e) {
        const rect = this.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        
        // Update filter turbulence based on mouse position
        const filter = document.querySelector('#glass-distortion feDisplacementMap') || this.querySelector('filter feDisplacementMap');
        if (filter) {
          const scaleX = (x / rect.width) * currentScale;
          const scaleY = (y / rect.height) * currentScale;
          filter.setAttribute('scale', Math.max(8, Math.min(scaleX, scaleY)));
        }
        
        // Add dynamic highlight specular effect following cursor
        const specular = this.querySelector('.glass-specular');
        if (specular) {
          const alpha1 = isDarkMode ? 0.28 : 0.22;
          const alpha2 = isDarkMode ? 0.08 : 0.06;
          specular.style.background = `radial-gradient(
            circle at ${x}px ${y}px,
            rgba(255,255,255,${alpha1}) 0%,
            rgba(255,255,255,${alpha2}) 30%,
            rgba(255,255,255,0) 65%
          )`;
        }
      }
      
      // Reset effects when mouse leaves
      function handleMouseLeave() {
        const filter = document.querySelector('#glass-distortion feDisplacementMap');
        if (filter) {
          filter.setAttribute('scale', currentScale.toString());
        }
        
        const specular = this.querySelector('.glass-specular');
        if (specular) {
          specular.style.background = 'none';
        }
      }
    });

    // Background Switcher
    function setScreenBg(bgType) {
      const bgLayer = document.getElementById('screenBackgroundLayer');
      const chromeView = document.getElementById('chromeWindowView');
      
      // Reset button classes
      ['btnBgChrome', 'btnBgSonoma', 'btnBgWindows', 'btnBgAurora'].forEach(id => {
        const btn = document.getElementById(id);
        btn.classList.remove('bg-white', 'text-black', 'shadow-sm', 'font-semibold');
        btn.classList.add('text-[var(--text-pri)]', 'font-medium');
      });
      
      // Clear background classes
      bgLayer.className = 'fixed inset-0 z-0 transition-all duration-300 flex flex-col overflow-hidden pointer-events-auto';

      if (bgType === 'chrome') {
        bgLayer.classList.add('bg-chrome');
        chromeView.classList.remove('hidden');
        document.getElementById('btnBgChrome').classList.add('bg-white', 'text-black', 'shadow-sm', 'font-semibold');
      } else {
        chromeView.classList.add('hidden');
        if (bgType === 'sonoma') {
          bgLayer.classList.add('bg-sonoma');
          document.getElementById('btnBgSonoma').classList.add('bg-white', 'text-black', 'shadow-sm', 'font-semibold');
        } else if (bgType === 'windows') {
          bgLayer.classList.add('bg-windows');
          document.getElementById('btnBgWindows').classList.add('bg-white', 'text-black', 'shadow-sm', 'font-semibold');
        } else if (bgType === 'aurora') {
          bgLayer.classList.add('bg-aurora');
          document.getElementById('btnBgAurora').classList.add('bg-white', 'text-black', 'shadow-sm', 'font-semibold');
        }
      }
    }

    // Tuning Sliders
    function updateBlur(val) {
      document.getElementById('lblBlur').innerText = val + 'px';
      const filters = document.querySelectorAll('.glass-filter');
      filters.forEach(f => {
        f.style.backdropFilter = `blur(${val}px) saturate(135%) brightness(1.04)`;
        f.style.webkitBackdropFilter = `blur(${val}px) saturate(135%) brightness(1.04)`;
      });
    }

    function updateDistortion(val) {
      currentScale = parseInt(val, 10);
      document.getElementById('lblDistort').innerText = val;
      const filter = document.querySelector('#glass-distortion feDisplacementMap');
      if (filter) {
        filter.setAttribute('scale', val);
      }
    }

    // Copy action
    function copyTableDestinations() {
      const paths = [
        "Sorted\\\\Documents\\\\PDF\\\\document.pdf",
        "Sorted\\\\Documents\\\\TXT\\\\duplicate-a.txt",
        "Sorted\\\\Documents\\\\TXT\\\\duplicate-b.txt",
        "Sorted\\\\Documents\\\\TXT\\\\notes.txt",
        "Sorted\\\\Images\\\\JPG\\\\photo.jpg",
        "Sorted\\\\Images\\\\JPG\\\\photo_dup.jpg",
        "Sorted\\\\Other\\\\XYZ\\\\unknown.xyz"
      ].join('\\n');
      navigator.clipboard.writeText(paths);
      const lbl = document.getElementById('lblCopyPaths');
      lbl.innerText = "Copied 7 Paths!";
      setTimeout(() => { lbl.innerText = "Copy Dest Paths"; }, 1500);
    }

    // Theme toggle
    function toggleTheme() {
      isDarkMode = !isDarkMode;
      document.documentElement.classList.toggle('dark', isDarkMode);
      document.getElementById('themeEmoji').innerText = isDarkMode ? '☀️' : '🌙';
      document.getElementById('themeLabel').innerText = isDarkMode ? 'Light' : 'Dark';
    }

    // Sidebar toggle
    function toggleSidebar() {
      isCollapsed = !isCollapsed;
      const sidebar = document.getElementById('appSidebar');
      const expHeader = document.getElementById('expandedHeader');
      const colHeader = document.getElementById('collapsedHeader');
      const navLabels = document.querySelectorAll('.nav-label');
      const navBadges = document.querySelectorAll('.nav-badge');
      const navBtns = document.querySelectorAll('.nav-btn');
      const navContents = document.querySelectorAll('.nav-content');
      const indicators = document.querySelectorAll('.active-indicator');

      if (isCollapsed) {
        sidebar.classList.remove('w-[244px]');
        sidebar.classList.add('w-[72px]');
        expHeader.classList.add('hidden');
        colHeader.classList.remove('hidden');
        colHeader.classList.add('flex');
        navLabels.forEach(el => el.classList.add('hidden'));
        navBadges.forEach(el => el.classList.add('hidden'));
        navBtns.forEach(el => {
          el.classList.remove('justify-between');
          el.classList.add('justify-center');
          el.classList.remove('px-3');
          el.classList.add('px-0');
        });
        navContents.forEach(el => el.classList.remove('space-x-3'));
        indicators.forEach(el => el.classList.remove('hidden'));
      } else {
        sidebar.classList.remove('w-[72px]');
        sidebar.classList.add('w-[244px]');
        expHeader.classList.remove('hidden');
        colHeader.classList.add('hidden');
        colHeader.classList.remove('flex');
        navLabels.forEach(el => el.classList.remove('hidden'));
        navBadges.forEach(el => el.classList.remove('hidden'));
        navBtns.forEach(el => {
          el.classList.add('justify-between');
          el.classList.remove('justify-center');
          el.classList.add('px-3');
          el.classList.remove('px-0');
        });
        navContents.forEach(el => el.classList.add('space-x-3'));
        indicators.forEach(el => el.classList.add('hidden'));
      }
    }

    // Tab switching
    function switchTab(tab) {
      const tabs = ['overview', 'files', 'categories', 'duplicates', 'organize', 'history'];
      tabs.forEach(t => {
        const pageEl = document.getElementById(`tab-${t}`);
        const navEl = document.getElementById(`nav-${t}`);
        if (pageEl) pageEl.classList.toggle('hidden', t !== tab);
        if (navEl) {
          if (t === tab) {
            navEl.style.background = 'var(--accent-tint)';
            navEl.style.color = 'var(--accent)';
            navEl.classList.add('font-semibold');
            navEl.classList.remove('font-medium');
            const svg = navEl.querySelector('.lucide-icon');
            if (svg) svg.style.stroke = 'var(--accent)';
            const ind = navEl.querySelector('.active-indicator');
            if (ind && isCollapsed) ind.classList.remove('hidden');
          } else {
            navEl.style.background = 'transparent';
            navEl.style.color = 'var(--text-sec)';
            navEl.classList.remove('font-semibold');
            navEl.classList.add('font-medium');
            const svg = navEl.querySelector('.lucide-icon');
            if (svg) svg.style.stroke = 'var(--text-sec)';
            const ind = navEl.querySelector('.active-indicator');
            if (ind) ind.classList.add('hidden');
          }
        }
      });

      const titleMap = {
        overview: ["Overview", "Workspace metrics, storage footprint reconciliation, and deterministic classification"],
        files: ["All Files", "Browse indexed direct files, inspect extensions, and run DAA Merge Sort"],
        categories: ["Categories", "Taxonomy breakdown and target folder routing hierarchy"],
        duplicates: ["Duplicates", "Byte-identical duplicate groups identified via SHA-256"],
        organize: ["Organization Plan", "Review and approve collision-safe move operations with full inspectability"],
        history: ["History & Journal", "Audit trail of completed operations with one-click undo"]
      };
      document.getElementById('pageTitle').innerText = titleMap[tab][0];
      document.getElementById('pageSubtitle').innerText = titleMap[tab][1];
    }

    // Simulated scan
    function runSimulatedScan() {
      const badge = document.getElementById('scanStatusBadge');
      badge.innerText = "Scanning...";
      badge.style.background = "var(--accent-tint)";
      badge.style.color = "var(--accent)";
      
      setTimeout(() => {
        badge.innerText = "Ready";
        badge.style.background = "var(--hover)";
        badge.style.color = "var(--text-sec)";
      }, 350);
    }
  </script>
</body>
</html>
"""

DEST_PATH.write_text(html_content, encoding='utf-8')
print(f"Liquid Glass Preview written successfully to: {DEST_PATH} ({len(html_content)} bytes)")
