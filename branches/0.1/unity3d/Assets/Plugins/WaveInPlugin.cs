using UnityEngine;
using System;
using System.Runtime.InteropServices;

public class WaveInPlugin : MonoBehaviour
{
	public bool IsInitialized { get; private set; }

	protected virtual void Start ()
	{
		if (FindObjectsOfType (typeof(WaveInPlugin)).Length > 1) {
			print ("There can be only one WaveInPlugin; destroying component");
			Destroy (this);
		}
		
		if (!(IsInitialized = (1 == WaveIn_Init ()))) {
			print ("WaveIn_Init failed; destroying component");
			Destroy (this);
		}
		
		print ("WaveInPlugin initialized; found " + WaveIn_GetNumDevices () + " device(s)");
	}

	protected virtual void OnDestroy ()
	{
		if (IsInitialized) {
			WaveIn_Destroy ();
			IsInitialized = false;
			print ("WaveInPlugin destroyed");
		}
	}

	protected delegate void WaveIn_Callback (int numSamples, IntPtr samples);

	const string PLUGIN_DLL_NAME = "wavein";

	[DllImport(PLUGIN_DLL_NAME)]
	protected static extern int WaveIn_Init ();

	[DllImport(PLUGIN_DLL_NAME)]
	protected static extern void WaveIn_Destroy ();

	[DllImport(PLUGIN_DLL_NAME)]
	protected static extern uint WaveIn_GetNumDevices ();

	[DllImport(PLUGIN_DLL_NAME)]
	protected static extern int WaveIn_StartDevice (uint id, WaveIn_Callback callback);

	[DllImport(PLUGIN_DLL_NAME)]
	protected static extern void WaveIn_StopDevice ();
}
