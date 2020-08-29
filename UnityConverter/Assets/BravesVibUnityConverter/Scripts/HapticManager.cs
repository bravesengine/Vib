using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HapticManager : MonoBehaviour
{
    public OVRHapticsClip clip;

    public byte[] bytes;
    public string byteString;

    private int index;

    private void OnEnable()
    {
        bytes = System.Text.Encoding.UTF8.GetBytes(byteString);
        Debug.Log(bytes.Length);
        Debug.Log("SampleSizeInBytes is " + OVRHaptics.Config.SampleSizeInBytes);

        clip = new OVRHapticsClip(bytes, bytes.Length);
        Debug.Log("clip is *" +  clip + "*******");

        index = 0;
    }


    private void Update()
    {
        //index trigger down
		if (OVRInput.GetDown(OVRInput.Button.PrimaryIndexTrigger))
		{
            Debug.Log("IndexTrigger down");

            string key = "VibSample1";
            switch (index)
            {
                case 0 : key = "VibSample1"; break;
                case 1 : key = "VibSample2"; break;
                case 2 : key = "VibSample3"; break;
            }

            index ++;
            if (index == 3)
                index = 0;
                
            OVRHaptics.RightChannel.Mix(VibManager.vibDictionary[key]);
		}
    }



}
