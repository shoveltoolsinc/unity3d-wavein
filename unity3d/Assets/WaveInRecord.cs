using UnityEngine;
using System;
using System.IO;
using System.Collections.Generic;
using System.Runtime.InteropServices;

public class WaveInRecord : WaveInPlugin
{
	static WaveInRecord Instance;

	bool isRecording;
	bool toggleRecord;

	short[] scratch;
	List<short> samplesList;

	protected override void Start ()
	{
		base.Start ();
		Instance = this;
	}

	protected override void OnDestroy ()
	{
		if (isRecording)
			StopRecording ();
		base.OnDestroy ();
	}

	void Update ()
	{
		if (toggleRecord) {
			if (isRecording)
				StopRecording ();
			else
				StartRecording ();
			
			toggleRecord = false;
		}
	}

	void OnGUI ()
	{
		if (GUI.Button (new Rect (16, 16, 128, 32), isRecording ? "Stop recording" : "Start recording"))
			toggleRecord = true;
	}

	void StartRecording ()
	{
		samplesList = new List<short> ();
		isRecording = (1 == WaveIn_StartDevice (0, RecordCallback));
		if (isRecording)
			print ("WaveInRecord started");
	}

	void StopRecording ()
	{
		isRecording = false;
		WaveIn_StopDevice ();
		print ("WaveInRecord stopped");
		
		string filePath = Application.persistentDataPath + "/" + Guid.NewGuid ().ToString () + ".pcm";
		using (var fs = new FileStream (filePath, FileMode.Create, FileAccess.Write)) {
			print ("Writing " + filePath);
			using (var bw = new BinaryWriter (fs)) {
				foreach (var s in samplesList)
					bw.Write (s);
			}
		}
	}

	void RecordCallback_Instance (int numSamples, IntPtr samples)
	{
		if (null == scratch || numSamples != scratch.Length)
			scratch = new short[numSamples];
		
		Marshal.Copy (samples, scratch, 0, numSamples);
		samplesList.AddRange (scratch);
	}

	static void RecordCallback (int numSamples, IntPtr samples)
	{
		//print("Got " + numSamples + " samples");
		Instance.RecordCallback_Instance (numSamples, samples);
	}
}
