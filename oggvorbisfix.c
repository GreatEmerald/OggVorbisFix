// Corrupted Ogg Vorbis file fixer

/* If you have an OGG Vorbis file whose header (up to the first ~48 bytes) is
 * corrupt, use this program to recover it. If the corruption does not reach the
 * 22nd byte, this recovers the file completely. Else, after running the tool,
 * you also need to run `ffmpeg -i input.ogg -c:a copy output.ogg` in order to
 * recalculate the CRC checksums. */

/* Copyright 2016 GreatEmerald

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stdio.h>

char* OggS = "OggS";
char* OggStart = "\x00\x02";
char* Zeroes = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
char* VorbisStart = "\x01\x1E\x01vorbis";
char* Channels = "\x02"; // Change if you have more than 2 channels
char* SampleRate = "\x44\xAC\x00\x00"; // Change if your sample rate is not 44100 (0xac44 in dec, note little endian)
char* BitRate = "\x00\xE8\x03\x00"; // Change if your bitrate is not 256 kbps (0x03e800 is 256000 in dec)
char* PacketSizes = "\xB8\x01";

int main(int argc, char *argv[])
{
    int count;
    FILE* ogg_file;
    char* buffer;

    for (count = 1; count < argc; count++)
    {
        printf("Processing OGG Vorbis file %d: %s\n", count, argv[count]);
        ogg_file=fopen(argv[count],"rb+");
        // Write header ID (includes start of file bit)
        fwrite(OggS, 14, 1, ogg_file);
        // Write OGG "start of file"
        fseek(ogg_file, 4, SEEK_SET);
        fwrite(OggStart, 2, 1, ogg_file);
        // Versions 0 and so on
        fseek(ogg_file, 6, SEEK_SET);
        fwrite(Zeroes, 8, 1, ogg_file);
        
        // Get first stream GUID
        fseek(ogg_file, 72, SEEK_SET);
        fread(&buffer, 4, 1, ogg_file);
        // Set it as the first stream of the file
        fseek(ogg_file, 14, SEEK_SET);
        fwrite(&buffer, 4, 1, ogg_file);
        
        // Write zeroes to overwrite garbage
        fseek(ogg_file, 18, SEEK_SET);
        fwrite(Zeroes, 4, 1, ogg_file);
        // Skip next four bytes (CRC checksum), will be recalculated by FFmpeg if needed
        fseek(ogg_file, 26, SEEK_SET);
        // Write "vorbis" header
        fwrite(VorbisStart, 9, 1, ogg_file);
        // Vorbis version "0"
        fseek(ogg_file, 35, SEEK_SET);
        fwrite(Zeroes, 4, 1, ogg_file);
        // Audio channels
        fseek(ogg_file, 39, SEEK_SET);
        fwrite(Channels, 1, 1, ogg_file);
        // Sampling rate
        fseek(ogg_file, 40, SEEK_SET);
        fwrite(SampleRate, 4, 1, ogg_file);
        // Bitrate max - don't care, skip
        // Bitrate nominal
        fseek(ogg_file, 48, SEEK_SET);
        fwrite(BitRate, 4, 1, ogg_file);
        // Bitrate min - don't care, skip
        // Packet sizes; 2^8 and 2^B seem standard. Then also 01 for frame end
        fseek(ogg_file, 56, SEEK_SET);
        fwrite(PacketSizes, 2, 1, ogg_file);
        // Start of new OggS
        fseek(ogg_file, 58, SEEK_SET);
        fwrite(OggS, 4, 1, ogg_file);
        // Lots of zeroes are here usually
        fseek(ogg_file, 62, SEEK_SET);
        fwrite(Zeroes, 10, 1, ogg_file);
        fclose(ogg_file);
    }
    return 0;
}
