# HCA Decoder

All credits go to the [original authors](https://github.com/KinoMyu/FastHCADecoder) of this repo. I can't believe this efficient, lightweight command line tool remains in obscurity because of language barriers, so I decided to translate the error messages in the source files and add some useful resources like uncovered HCA keys for popular games.

# Usage

```
make
```

```
usage: ./clHCA [options] input.hca

options:
  -o      output filename
  -a      set key (lower 8 hex digits)
  -b      set key (higher 8 hex digits)
  -s      set sub key (4 hex digits, in decimal)
  -c      try decrypting the file without generating an output
  -m      mode
  -l      loop
  -v      volume
  -t      number of threads (to speed up decoding)
  -i      show info about input file and quit
  -h      show this help message
```

This repo contains a sample file `sample.hca` and uses its key `00A06A0B 8D0C10FD` as a built-in. Note that you have to change the key for your HCA files to decode properly. You can find a list of uncovered HCA keys for popular games here:
- https://github.com/losnoco/vgmstream/blob/master/src/meta/hca_keys.h
- https://hcs64.com/mboard/forum.php?showthread=22828

Note that the lower digits `8D0C10FD` go with `-a` and the higher ones `00A06A0B` go with `-b`.

## Decode

```
./clHCA sample.hca -a 8D0C10FD -b 00A06A0B
```

`sample.hca` will decode to `sample.wav`. You should hear footsteps if you play it. If you hear noise instead, please submit a GitHub issue.

## Decrypt (no output)

```
./clHCA sample.hca -a 8D0C10FD -b 00A06A0B -c

Decrypting sample.hca...
Decrypted successfully with the keys provided.
```

## HCA file Info

The ouput below shows that our sample file uses enrcyption keys.

```
./clHCA sample.hca -i

Showing info for sample.hca:
Codec: HCA
version: 2.0
Channel count: stereo (2 channels)
Sampling rate: 48000Hz
Block count: 393
Mute header sample count: 0
Mute footer sample count: 943
Bit rate: 192kbps CBR (default bit rate)
Block size: 0x200
comp1: 1
comp2: 15
comp3: 1
comp4: 0
comp5: 128
comp6: 64
comp7: 64
comp8: 0
Cipher type: Key cipher ※If you do not use the correct key, the output waveform will be incorrect.

```

Another sample file can be found at: https://kinomyu.github.io/bgm.hca. Note that there is no encrytion key.
```
./clHCA bgm.hca -i

Showing info for bgm.hca:
Codec: HCA
version: 2.0
Channel count: stereo (2 channels)
Sampling rate: 44100Hz
Block count: 5563
Mute header sample count: 0
Mute footer sample count: 164
Bit rate: 234.97kbps CBR (default bit rate)
Block size: 0x2AA
comp1: 1
comp2: 15
comp3: 1
comp4: 0
comp5: 128
comp6: 128
comp7: 0
comp8: 0
Cipher type: None
```