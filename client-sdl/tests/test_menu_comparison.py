"""
Compares the client-sdl port's rendering of every real menu page against the original
(pre-GLES3-port) client, page by page.

This is NOT a pixel-perfect diff - the two builds run at different resolutions and through
completely different rendering paths (desktop GL fixed-function vs GLES3), so exact equality
isn't the goal. Instead this test:

  1. Renders each named page in both builds via their respective "jump straight to this page and
     screenshot" test hooks (client-sdl's --page=/--screenshot= CLI flags, the reference build's
     DYNA_TEST_PAGE/DYNA_TEST_SCREENSHOT env vars - see main.cpp and bombermanclientgui.cpp).
  2. Asserts basic sanity on each screenshot (file exists, non-trivial size, not a single flat
     color - i.e. something real got drawn, not a blank/error screen).
  3. Writes a side-by-side "original | client-sdl port" comparison PNG for every page into
     comparison_output/, for a human to actually look at - this is the real value of the test,
     since "does it look right" isn't something a pixel assertion can answer for a renderer this
     early in its port.

Requires both executables to already be built:
  - client-sdl:  client-sdl/build/Release/dynablaster_sdl.exe
  - reference:   client/build-ref/release/dynablaster.exe (see project memory for how this was
                 set up - it needs a separate Qt5 install, not part of this repo's normal build)

Run with: pytest client-sdl/tests/test_menu_comparison.py -v -s
"""

import os
import subprocess
import time
from pathlib import Path

import pytest
from PIL import Image

REPO_ROOT = Path(__file__).resolve().parents[2]
CLIENT_SDL_EXE = REPO_ROOT / "client-sdl" / "build" / "Release" / "dynablaster_sdl.exe"
CLIENT_SDL_CWD = CLIENT_SDL_EXE.parent
REFERENCE_EXE = REPO_ROOT / "client" / "build-ref" / "release" / "dynablaster.exe"
REFERENCE_CWD = REPO_ROOT / "client"

OUTPUT_DIR = Path(__file__).parent / "comparison_output"
OUTPUT_DIR.mkdir(exist_ok=True)

# (test id, psd path relative to each build's data dir)
PAGES = [
    ("mainmenu", "data/menus/mainmenu.psd"),
    ("selectgame", "data/menus/selectgame.psd"),
    ("creategame", "data/menus/creategame.psd"),
    ("lounge", "data/menus/lounge.psd"),
    ("options_video", "data/menus/options_video.psd"),
    ("options_audio", "data/menus/options_audio.psd"),
    ("options_controls", "data/menus/options_controls.psd"),
    ("options_game", "data/menus/options_game.psd"),
    ("about", "data/menus/about.psd"),
]

PROCESS_TIMEOUT_SECONDS = 20


def _require_executables():
    if not CLIENT_SDL_EXE.exists():
        pytest.skip(f"client-sdl exe not built: {CLIENT_SDL_EXE}")
    if not REFERENCE_EXE.exists():
        pytest.skip(f"reference exe not built: {REFERENCE_EXE}")


def render_client_sdl(psd_path: str, out_path: Path) -> None:
    if out_path.exists():
        out_path.unlink()
    subprocess.run(
        [
            str(CLIENT_SDL_EXE),
            "--selftest",
            "--menu",
            f"--page={psd_path}",
            f"--screenshot={out_path}",
        ],
        cwd=CLIENT_SDL_CWD,
        timeout=PROCESS_TIMEOUT_SECONDS,
        capture_output=True,
    )


def render_reference(psd_path: str, out_path: Path) -> None:
    if out_path.exists():
        out_path.unlink()
    env = os.environ.copy()
    env["DYNA_TEST_PAGE"] = psd_path
    env["DYNA_TEST_SCREENSHOT"] = str(out_path)
    proc = subprocess.Popen([str(REFERENCE_EXE)], cwd=REFERENCE_CWD, env=env)
    try:
        proc.wait(timeout=PROCESS_TIMEOUT_SECONDS)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait()
    # the reference build auto-quits itself once the screenshot is written (see
    # bombermanview.cpp's GameView::paintGL frame==180 hook) - give the filesystem a beat in case
    # the process exit races the file write on a slow disk.
    for _ in range(20):
        if out_path.exists():
            break
        time.sleep(0.25)


def assert_real_screenshot(path: Path, label: str) -> Image.Image:
    assert path.exists(), f"{label}: no screenshot was produced at {path}"
    assert path.stat().st_size > 1000, f"{label}: screenshot file suspiciously small"
    img = Image.open(path).convert("RGB")
    # a fully blank/single-color image means nothing real got drawn (crash, black screen, etc).
    extrema = img.getextrema()
    is_flat = all(lo == hi for lo, hi in extrema)
    assert not is_flat, f"{label}: screenshot is a single flat color - nothing rendered"
    return img


def make_side_by_side(reference_img: Image.Image, port_img: Image.Image, out_path: Path) -> None:
    target_height = 600
    ref_scaled = reference_img.resize(
        (int(reference_img.width * target_height / reference_img.height), target_height)
    )
    port_scaled = port_img.resize(
        (int(port_img.width * target_height / port_img.height), target_height)
    )
    gap = 8
    combined = Image.new(
        "RGB", (ref_scaled.width + gap + port_scaled.width, target_height), (40, 40, 40)
    )
    combined.paste(ref_scaled, (0, 0))
    combined.paste(port_scaled, (ref_scaled.width + gap, 0))
    combined.save(out_path)


@pytest.fixture(scope="module", autouse=True)
def _check_executables():
    _require_executables()


@pytest.mark.parametrize("page_id,psd_path", PAGES)
def test_page_renders_and_compare(page_id, psd_path):
    port_shot = OUTPUT_DIR / f"{page_id}_port.png"
    ref_shot = OUTPUT_DIR / f"{page_id}_reference.png"

    render_client_sdl(psd_path, port_shot)
    render_reference(psd_path, ref_shot)

    port_img = assert_real_screenshot(port_shot, f"client-sdl port ({page_id})")
    ref_img = assert_real_screenshot(ref_shot, f"reference build ({page_id})")

    make_side_by_side(ref_img, port_img, OUTPUT_DIR / f"{page_id}_side_by_side.png")
