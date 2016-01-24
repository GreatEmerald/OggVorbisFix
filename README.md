# OggVorbisFix
Corrupt OGG Vorbis file header fix tool:
If you have an OGG Vorbis file whose header (up to the first ~48 bytes) is corrupt and thus it is no longer recognised by audio players, use this program to recover it. If the corruption does not reach the 22nd byte, this recovers the file completely. Else, after running the tool, you also need to run `ffmpeg -i input.ogg -c:a copy output.ogg` in order to recalculate the CRC checksums.

To build the program, simply use `gcc -o oggvorbisfix oggvorbisfix.c`. To run the program, simply pass a list of corrupt files to `oggvorbisfix` (note that they are modified in-place, so create backups first just in case).
