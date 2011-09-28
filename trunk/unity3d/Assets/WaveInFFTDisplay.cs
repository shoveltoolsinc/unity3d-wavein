using UnityEngine;
using System.Collections;

public class WaveInFFTDisplay : MonoBehaviour
{
	Transform[] bars;

	public void UpdateDisplay (float[] samples, float barScale)
	{
		if (null == bars || bars.Length != samples.Length) {
			if (null != bars)
				foreach (var bar in bars)
					Destroy (bar);
			
			bars = new Transform[samples.Length];
			for (int i = 0; i < bars.Length; i++) {
				bars[i] = GameObject.CreatePrimitive (PrimitiveType.Cube).transform;
				{
					bars[i].transform.parent = transform;
					bars[i].localPosition = new Vector3 (i, 0, 0);
					bars[i].localScale = new Vector3 (1, 0, 1);
				}
			}
		}
		
		for (int i = 0; i < bars.Length; i++) {
			bars[i].localPosition = new Vector3 (i, barScale * samples[i] * 0.5f);
			bars[i].localScale = new Vector3 (1, barScale * samples[i], 1);
		}
	}
}
