using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VibrationTester : MonoBehaviour
{

    private void Update()
    {
        //index trigger down
		if (OVRInput.GetDown(OVRInput.Button.PrimaryIndexTrigger))
		{
            OVRHaptics.LeftChannel.Mix(VibManager.vibDictionary[VibManager.testKey]);
		}
		if (OVRInput.GetDown(OVRInput.Button.SecondaryIndexTrigger))
		{
            OVRHaptics.RightChannel.Mix(VibManager.vibDictionary[VibManager.testKey]);
		}
    }
}
