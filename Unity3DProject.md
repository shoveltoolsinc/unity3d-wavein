**OS X Lion users:** the plugin crashes the editor; I don't know why. Standalones are fine.

There's a branch with ready-made binaries; to just get the Unity3D project from there you can do this:

`svn export http://unity3d-wavein.googlecode.com/svn/branches/0.1/unity3d 0.1`

Which will check out to a directory called '0.1'

### record.unity ###

Click the button to start recording with the first available device; clicking the stop button or exiting play mode will dump a uniquely named raw file into the persistentDataPath (the file name is output to console); use, for example, the raw import option in Audacity to listen to the recorded samples (recording is always at 44100hz, 16 bit sample values in stereo).

### fft.unity ###

Perform an FFT on input samples with an interface similar to `AudioSource` (`GetOutputData`, `GetSpectrumData`). Uses Chris Lomont's [C# FFT code](http://www.lomont.org/Software/Misc/FFT/LomontFFT.html). The scene has a frequency bin display, but you'll need to hook up an `AudioSource` and start the wave input yourself (the idea being, one frequency bin display shows frequencies for the sound being played through the speakers, the other shows frequencies for that sound recorded from the speakers by wave input).