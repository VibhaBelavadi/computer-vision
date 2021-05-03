- Programmed/trained OpenCV to detect ‘shh expression’ (silence) and a ‘wink expression’ in images and live video using HAAR cascade classifiers. 

- Achieved 85%+ accuracy on the given set of images

- In detect wink (DetectWink.cpp), we took mulitple cascades for eyes and the face. By doing median filtering we were able to get the cascades working on majority of the images.

- In silence detection (DetectSilence.cpp), by taking multiple cascades for the face and changing the rate of discovering the mouth, we were able to achieve a decent accuracy.
