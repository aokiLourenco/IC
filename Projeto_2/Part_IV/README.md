# To compile the code in this file, run the following command:

```bash
    mkdir build
    cd build
    cmake ..
    make
```

# To run the compiled code, run the following command:

```bash
    ./video_codec <mode> <codec> <input> <output> [options] # For Video Codec
    # or
    ./LossyVideoCodec <input_video> <output_encoded> <quant_level> <predictor_id> # Video codec Lossy
```
```bash
Video Codec Usage:
./codec <mode> <codec> <input> <output> [options]

Modes:
  -e    Encode
  -d    Decode

Codec Types:
  -p    Predictor only
  -i    Intra-frame
  -v    Inter-frame (video)

Common Options:
  --predictor <1-8>   Prediction mode (default: 8)
  --m <value>         Initial M value (default: 4)
  --color <space>     Color space (yuv420/yuv422/yuv444)

Inter-frame Options:
  --block <size>      Block size (default: 16)
  --search <range>    Search range (default: 16)
  --keyframe <int>    Keyframe interval (default: 30)
```