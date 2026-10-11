## todo
- start printing the FPS - inverse of deltaTime or could sample over n number of frames
- temporarily stop doing FPS thresholding/capping
- then do the packed pool optimization and compare to see if it increases uncapped FPS or not
- could also display avg FPS and std dev of FPS using welford algorithm (this doesn't approximate it's actually accurate)
- profile using memcheck and valgrind on my ubuntu machine 😈