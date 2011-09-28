using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;

public class WaveInFFT : WaveInPlugin
{
	const int MaxChannelSamples = 16384;
	const int MaxChannelSamplesFFT = MaxChannelSamples / 2;
	const int NumChannels = 2;

	static WaveInFFT Instance;

	short[] scratch;
	object lockObj = new object ();
	float[][] channelSamples = new float[NumChannels][];
	int nextChannelSampleIndex;

	Lomont.LomontFFT FFT = new Lomont.LomontFFT ();
	double[] FFTInOut;

	public bool IsRunning { get; private set; }

	public uint GetNumDevices ()
	{
		return WaveIn_GetNumDevices ();
	}

	public void StartDevice (uint id)
	{
		WaveIn_StartDevice (id, BufferCallback);
	}

	public void StopDevice ()
	{
		WaveIn_StopDevice ();
	}

	public void GetOutputData (float[] samples, int channel)
	{
		if (samples.Length > channelSamples[channel].Length)
			throw new ArgumentException ();
		
		lock (lockObj) {
			if (samples.Length <= nextChannelSampleIndex) {
				Array.Copy (channelSamples[channel], nextChannelSampleIndex - samples.Length, samples, 0, samples.Length);
			} else {
				int dstSplit = samples.Length - nextChannelSampleIndex;
				int srcSplit = channelSamples[channel].Length - dstSplit;
				Array.Copy (channelSamples[channel], srcSplit, samples, 0, dstSplit);
				Array.Copy (channelSamples[channel], 0, samples, dstSplit, nextChannelSampleIndex);
			}
		}
	}

	void GetOutputDataDouble (double[] samples, int channel)
	{
		if (samples.Length > channelSamples[channel].Length)
			throw new ArgumentException ();
		
		lock (lockObj) {
			if (samples.Length <= nextChannelSampleIndex) {
				int di = 0;
				for (int si = nextChannelSampleIndex - samples.Length; si < nextChannelSampleIndex; si++)
					samples[di++] = channelSamples[channel][si];
			} else {
				int dstSplit = samples.Length - nextChannelSampleIndex;
				int srcSplit = MaxChannelSamples - dstSplit;
				int di = 0;
				for (int si = srcSplit; si < MaxChannelSamples; si++)
					samples[di++] = channelSamples[channel][si];
				for (int si = 0; si < nextChannelSampleIndex; si++)
					samples[di++] = channelSamples[channel][si];
			}
		}
	}

	public void GetSpectrumData (float[] samples, int channel)
	{
		if (!Mathf.IsPowerOfTwo (samples.Length) || samples.Length > MaxChannelSamplesFFT)
			throw new ArgumentException ();
		
		int numSamples = 2 * samples.Length;
		if (null == FFTInOut || numSamples != FFTInOut.Length)
			FFTInOut = new double[numSamples];
		
		GetOutputDataDouble (FFTInOut, channel);
		
		FFT.RealFFT (FFTInOut, true);
		samples[0] = samples[samples.Length - 1] = 0.0f;
		for (int i = 2; i < FFTInOut.Length; i += 2) {
			float re = (float)FFTInOut[i];
			float im = (float)FFTInOut[i + 1];
			samples[i / 2] = Mathf.Sqrt (re * re + im * im) / samples.Length;
		}
	}

	void Awake ()
	{
		for (int c = 0; c < NumChannels; c++)
			channelSamples[c] = new float[MaxChannelSamples];
	}

	protected override void Start ()
	{
		base.Start ();
		Instance = this;
	}

	void CopyInterleavedStereoSamples (short[] src, int srcStart, int srcEnd)
	{
		for (int i = srcStart; i < srcEnd; i += 2) {
			float s0 = (src[i] + 0.5f) / 32767.5f;
			float s1 = (src[i + 1] + 0.5f) / 32767.5f;
			channelSamples[0][nextChannelSampleIndex] = s0;
			channelSamples[1][nextChannelSampleIndex] = s1;
			nextChannelSampleIndex++;
		}
		nextChannelSampleIndex = nextChannelSampleIndex % MaxChannelSamples;
	}

	void BufferCallback_Instance (int numSamples, IntPtr samples)
	{
		if (null == scratch || scratch.Length < numSamples)
			scratch = new short[numSamples];
		
		Marshal.Copy (samples, scratch, 0, numSamples);
		
		int numSamplesPerChannel = numSamples / 2;
		
		lock (lockObj) {
			if (nextChannelSampleIndex + numSamplesPerChannel <= MaxChannelSamples) {
				CopyInterleavedStereoSamples (scratch, 0, numSamples);
			} else {
				int top = (MaxChannelSamples - nextChannelSampleIndex) * 2;
				CopyInterleavedStereoSamples (scratch, 0, top);
				CopyInterleavedStereoSamples (scratch, top, numSamples);
			}
		}
	}

	static void BufferCallback (int numSamples, IntPtr samples)
	{
		Instance.BufferCallback_Instance (numSamples, samples);
	}
}
