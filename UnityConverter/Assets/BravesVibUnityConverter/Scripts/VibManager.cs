using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class VibManager : MonoBehaviour
{
    [System.Serializable]
    public class VibDataContent
    {
        public string HeaderDesc;
        public string RawBase64;
        public string RawData;
        public int Size;
        public int Version;
    }

    public static VibManager instance; 
    public static Dictionary<string, OVRHapticsClip>  vibDictionary;

    private void Awake()
    {
        if(instance != null)
        {
            Destroy(this);
        }
        instance = this;
    }

    private void Start()
    {
        ReadVibDataFromJson();
    }


    //Read RawData from each json files in vibDataFiles array and add those data to vibDictionary
    private void ReadVibDataFromJson()
    {
        TextAsset[] vibDataFiles = Resources.LoadAll<TextAsset>("VibData");

        vibDictionary = new Dictionary<string, OVRHapticsClip>();

        for(int _i = 0 ; _i < vibDataFiles.Length ; _i ++)
        {
            VibDataContent vibDataContent = JsonUtility.FromJson<VibDataContent>(vibDataFiles[_i].ToString());  //parse json
            byte[] _bytes = StringToByte(vibDataContent.RawData);              //convert rawdata to byte array
            OVRHapticsClip _clip = new OVRHapticsClip(_bytes, _bytes.Length);
            vibDictionary.Add(vibDataFiles[_i].name, _clip);               //add these data sets to dictionary
        }

        
    }


    private void EnumerizeKeys(TextAsset[] _vibDataFiles)
    {
        string enumTitle = "VibKeys";                   
        List<string> enumEntries = new List<string>();                  //add vibdata names to string list to enumerize them
        for(int _i = 0 ; _i < _vibDataFiles.Length ; _i ++)
        {        
            enumEntries.Add(_vibDataFiles[_i].name);
        }

        using (StreamWriter streamWriter = new StreamWriter("Assets/BravesVibUnityConverter/Scripts/" + enumTitle + ".cs"))
        {
            streamWriter.WriteLine("public enum " + enumTitle);
            streamWriter.WriteLine("{");
            for(int _i = 0 ; _i < enumEntries.Count ; _i++)
            {
                streamWriter.WriteLine("\t" + enumEntries[_i] + ",");
            }
            streamWriter.WriteLine("}");
        }

        UnityEditor.AssetDatabase.Refresh();
    }



    // String to byte array
    private byte[] StringToByte(string str)
    {
        byte[] StrByte = System.Text.Encoding.UTF8.GetBytes(str);
        string returnStr = System.Text.Encoding.Default.GetString(StrByte);

        Debug.Log(returnStr);

        return StrByte;
    }

    // byte array to string
    private string ByteToString(byte[] strByte)
    {
        string str = System.Text.Encoding.Default.GetString(strByte);
        return str;
    }

}
