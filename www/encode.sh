encode () {
  ~/Downloads/ffmpeg -r 6 -f concat -i $1.txt -framerate 6 -vcodec libx264 -pix_fmt yuv420p -s 600x450 $1.mp4
}

#encode 00000000-00600000
#encode 00600000-01200000
#encode 01200000-01800000
#encode 01800000-02400000
#encode 02400000-03000000
#encode 03000000-03600000
#encode 03600000-04200000
#encode 04200000-04800000
encode 04800000-05400000
