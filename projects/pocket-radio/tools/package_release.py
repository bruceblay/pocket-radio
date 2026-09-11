#!/usr/bin/env python3
"""Build a local release candidate from an allowlisted, credential-free copy.

Never reads flash, uploads firmware, or publishes anything. Run with the Python
environment containing PlatformIO. See release/README.md for remaining gates.
"""
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile


PROJECT = Path(__file__).resolve().parents[1]
ROOT = PROJECT.parents[1]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(args, log):
    with log.open("ab") as stream:
        subprocess.run([str(a) for a in args], stdout=stream, stderr=subprocess.STDOUT, check=True)


def main():
    version = re.search(r'radioVersion = "([a-zA-Z0-9.-]+)"', (PROJECT / "include/Version.h").read_text())[1]
    output = ROOT / "dist" / f"pocket-radio-{version}"
    output.mkdir(parents=True, exist_ok=False)  # Never overwrite an approved artifact.
    log = output / "build.log"
    with tempfile.TemporaryDirectory(prefix="pocket-radio-release-") as work:
        clean = Path(work) / "pocket-radio"
        clean.mkdir()
        for folder in ("include", "src", "tests", "tools", "release"):
            for original in sorted((PROJECT / folder).rglob("*")):
                if not original.is_file() or original.is_symlink():
                    continue
                if original.suffix not in (".h", ".cpp", ".cjs", ".py", ".md"):
                    continue
                if "secret" in original.name.lower() or "__pycache__" in original.parts:
                    continue
                destination = clean / original.relative_to(PROJECT)
                destination.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(original, destination)
        for name in ("platformio.ini", "README.md", "M5STACK-LICENSE", "LICENSE", "THIRD_PARTY_NOTICES.md"):
            if (PROJECT / name).exists(): shutil.copyfile(PROJECT / name, clean / name)
        # Reject reintroduction of the old compiled credential mechanism.
        for source in list((clean / "src").rglob("*")) + list((clean / "include").rglob("*")):
            if source.is_file() and re.search(r'RADIO_WIFI_|#\s*include\s*[<"]secrets', source.read_text()):
                raise RuntimeError("Legacy compiled credentials detected; packaging stopped.")
        source_hashes = {str(p.relative_to(clean)): digest(p) for p in sorted(clean.rglob("*")) if p.is_file()}
        print("Building isolated release candidate…", flush=True)
        run([sys.executable, "-m", "platformio", "run", "-d", clean], log)
        core = Path(os.environ.get("PLATFORMIO_CORE_DIR", str(Path.home() / ".platformio")))
        framework = core / "packages/framework-arduinoespressif32"
        esptool = core / "packages/tool-esptoolpy/esptool.py"
        build = clean / ".pio/build/sticks3"
        segments = [
            (0x0000, build / "bootloader.bin"),
            (0x8000, build / "partitions.bin"),
            (0xe000, framework / "tools/partitions/boot_app0.bin"),
            (0x10000, build / "firmware.bin"),
        ]
        # These offsets were verified against PlatformIO envdump. Fail if the
        # partition layout changes, rather than silently packaging wrong offsets.
        partitions = (framework / "tools/partitions/default_8MB.csv").read_text()
        rows = [re.sub(r"\s+", "", line).split(',') for line in partitions.splitlines() if line and not line.startswith('#')]
        assert any(row[:5] == ['nvs', 'data', 'nvs', '0x9000', '0x5000'] for row in rows)
        assert any(row[:5] == ['app0', 'app', 'ota_0', '0x10000', '0x330000'] for row in rows)
        assert (build / "firmware.bin").stat().st_size <= 0x330000
        image = output / f"pocket-radio-{version}-factory.bin"
        command = [sys.executable, esptool, "--chip", "esp32s3", "merge_bin", "--output", image,
                   "--flash_mode", "dio", "--flash_freq", "80m", "--flash_size", "8MB", "--fill-flash-size", "8MB"]
        for offset, segment in segments: command.extend([hex(offset), segment])
        run(command, log)
        data = image.read_bytes()
        assert len(data) == 8 * 1024 * 1024
        assert data[0x9000:0xe000] == b"\xff" * 0x5000, "Factory image must contain blank NVS"
        for offset, segment in segments[1:]:
            assert data[offset:offset + segment.stat().st_size] == segment.read_bytes()
        shutil.copyfile(build / "firmware.bin", output / f"pocket-radio-{version}-app.bin")
        # Archive only the original allowlisted inputs, never build artifacts.
        with tarfile.open(output / f"pocket-radio-{version}-source.tar.gz", "w:gz") as archive:
            for relative in source_hashes:
                archive.add(clean / relative, arcname=f"pocket-radio/{relative}")
        notices = output / "dependency-notices"
        for library in ("M5Unified", "M5GFX", "ESP8266Audio"):
            for name in ("LICENSE", "library.json"):
                original = clean / ".pio/libdeps/sticks3" / library / name
                if original.exists():
                    destination = notices / library / name
                    destination.parent.mkdir(parents=True, exist_ok=True)
                    shutil.copyfile(original, destination)
        manifest = {
            "version": version, "device": "M5Stack StickS3", "status": "LOCAL CANDIDATE — NOT PUBLISHED",
            "factory_flash_offset": "0x0", "factory_size": len(data), "blank_nvs_verified": True,
            "factory_install_resets_settings": True, "source_files_sha256": source_hashes,
            "segments": [{"offset": hex(offset), "file": p.name, "sha256": digest(p)} for offset, p in segments],
            "remaining_gates": ["hardware acceptance", "M5Burner import and clean-install test", "license choice and complete corresponding-source review", "public repository and release tag"],
        }
        (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
        (output / "README.md").write_text((PROJECT / "release/README.md").read_text())
        artifacts = [p for p in sorted(output.rglob('*')) if p.is_file() and p.name != 'build.log']
        (output / "SHA256SUMS").write_text(''.join(f"{digest(p)}  {p.relative_to(output)}\n" for p in artifacts))
    print(f"Candidate ready: {output}\nFactory image includes blank settings; not flashed or published.")


if __name__ == "__main__":
    main()
