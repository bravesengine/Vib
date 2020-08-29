using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HapticManager : MonoBehaviour
{
    public OVRHapticsClip clip;

    public byte[] bytes;
    public string byteString;
 
    private void OnEnable()
    {
        bytes = System.Text.Encoding.UTF8.GetBytes(byteString);
        Debug.Log(bytes.Length);
        Debug.Log("SampleSizeInBytes is " + OVRHaptics.Config.SampleSizeInBytes);

        clip = new OVRHapticsClip(bytes, bytes.Length);
        Debug.Log("clip is *" +  clip + "*******");
    }


    private void Update()
    {
        //index trigger down
		if (OVRInput.GetDown(OVRInput.Button.PrimaryIndexTrigger))
		{
            Debug.Log("IndexTrigger down");
            OVRHaptics.RightChannel.Mix(clip);
		}
    }



}
