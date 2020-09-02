using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VibrationTester : MonoBehaviour
{
    public string _str;

    private void Update()
    {

        //index trigger down
		if (OVRInput.GetDown(OVRInput.Button.PrimaryIndexTrigger))
		{
            string[] strigns = _str.Split(',');
            byte[] bytes = new byte[strigns.Length];
            for(int i = 0; i < strigns.Length; i++)
            {
                Debug.Log(int.Parse(strigns[i]) + "  " + System.Convert.ToByte(int.Parse(strigns[i])));
                bytes[i] = System.Convert.ToByte(int.Parse(strigns[i]));
            }


            OVRHapticsClip _clip = new OVRHapticsClip(bytes, bytes.Length);
            OVRHaptics.LeftChannel.Mix(_clip);

            //OVRHaptics.LeftChannel.Mix(VibManager.vibDictionary[VibManager.testKey]);
		}
		if (OVRInput.GetDown(OVRInput.Button.SecondaryIndexTrigger))
		{
            OVRHaptics.RightChannel.Mix(VibManager.vibDictionary[VibManager.testKey]);
		}
    }
}
